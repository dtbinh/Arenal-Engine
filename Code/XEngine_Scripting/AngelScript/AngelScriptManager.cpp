/********************************************************
*
* Author: Carlos Chac�n N.
*
* Desc:
*
*
* Copyright 2012-2014
*
*********************************************************/

/**********************
*   System Includes   *
***********************/
#include <string>
#include <fstream>
#include <istream>

/*************************
*   3rd Party Includes   *
**************************/
#include "fastformat\fastformat.hpp"
#include "AngelScript-Add-Ons\scriptarray\scriptarray.h"
#include "AngelScript-Add-Ons\scripthandle\scripthandle.h"
#include "AngelScript-Add-Ons\scriptbuilder\scriptbuilder.h"
#include "AngelScript-Add-Ons\scriptstdwstring\scriptstdwstring.h"
#include "AngelScript-Add-Ons\scriptwdictionary\scriptwdictionary.h"

/***************************
*   Game Engine Includes   *
****************************/
#include "Logger\Logger.h"
#include "AngelScriptDefs.h"
#include "AngelScriptManager.h"
#include "Base\BaseFunctions.h"
#include "Add-ons\AngelScriptAddOn.h"
#include "Localization\LocalizationManager.h"
#include "Localization\LocalizationManagerDefs.h"

//Always include last
#include "Memory\MemLeaks.h"

/********************
*   Function Defs   *
*********************/
XETODO("Check for mutex");
AngelScriptManager::AngelScriptManager()
{
}

AngelScriptManager::~AngelScriptManager()
{
	ReleaseAngel(m_ASEngine);
}

XEResult AngelScriptManager::Initialize()
{
	if(m_IsReady)
	{
		return XEResult::Ok;
	}

	////////////////////////////////////////////////////////////////////////
	//Initialize AS Engine
	m_ASEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	
	if(m_ASEngine == nullptr)
	{
		std::wstring errMsg = L"";
		fastformat::fmt(errMsg, XELOCMAN->GetLiteral(L"AS_CREATE_ENGINE_FAIL_ERR_MSG"), __FUNCTIONW__);
		XELOGGER->AddNewLog(LogLevel::Error, errMsg);

		return XEResult::Fail;
	}

	////////////////////////////////////////////////////////////////////////
	// Set engine to use UTF-16 encoding for string literals
	m_ASEngine->SetEngineProperty(asEP_SCRIPT_SCANNER, 1);
	m_ASEngine->SetEngineProperty(asEP_STRING_ENCODING, 1);
	m_ASEngine->SetEngineProperty(asEP_REQUIRE_ENUM_SCOPE, 1);

	int ret = 0;

	////////////////////////////////////////////////////////////////////////
	//The script compiler will write any compiler messages to the callback.
	//We want to log this messages in the logger
	ret = m_ASEngine->SetMessageCallback(asMETHOD(AngelScriptManager, ASMessageCallback), this, asCALL_THISCALL);
	if(ret < 0)
	{
		std::wstring errMsg = L"";
		fastformat::fmt(errMsg, XELOCMAN->GetLiteral(L"AS_SET_MSG_CALLBACK_ERR_MSG"), __FUNCTIONW__);
		XELOGGER->AddNewLog(LogLevel::Error, errMsg);

		return XEResult::Fail;
	}

	XEResult rst = XEResult::Ok;

	////////////////////////////////////////////////////////////////////////
	//Register Array Helper
	RegisterScriptArray(m_ASEngine, true);

	////////////////////////////////////////////////////////////////////////
	//Register STD WString
	RegisterStdWString(m_ASEngine);

	////////////////////////////////////////////////////////////////////////
	//Register STD WString
	RegisterStdWStringUtils(m_ASEngine);

	////////////////////////////////////////////////////////////////////////
	//Register Dictionary Helper
	RegisterScriptDictionaryW(m_ASEngine);

	////////////////////////////////////////////////////////////////////////
	//Register the generic handle type, called 'ref' in the script
	RegisterScriptHandle(m_ASEngine);

	////////////////////////////////////////////////////////////////////////
	//Set Ready Flag
	m_IsReady = true;

	return XEResult::Ok;
}

XEResult AngelScriptManager::RegisterAddOn(AngelScriptAddOn& addOn)
{
	if (!m_IsReady)
	{
		return XEResult::NotReady;
	}

	if (m_AddOnsRegisterIDs.find(addOn.GetAddOnID()) != m_AddOnsRegisterIDs.end())
	{
		return XEResult::ObjExists;
	}

	XEResult ret = addOn.Register(m_ASEngine);

	if (ret != XEResult::Ok)
	{
		XETODO("Add Log");
		return ret;
	}

	m_AddOnsRegisterIDs.insert(addOn.GetAddOnID());

	return XEResult::Ok;
}

XEResult AngelScriptManager::LoadScript(const std::wstring& scriptName, const std::wstring& module, asIScriptModule** asModule)
{
	if(!m_IsReady)
	{
		return XEResult::NotReady;
	}

	std::string sScriptName = XE_Base::WideStr2String(scriptName);
	std::string sModule = XE_Base::WideStr2String(module);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Check if the script has already been loaded
	asIScriptModule* tempASModule = m_ASEngine->GetModule(sModule.c_str());
	if (tempASModule != nullptr)
	{
		if (asModule != nullptr)
		{
			*asModule = tempASModule;
		}

		return XEResult::ASModuleAlreadyCreated;
	}

	CScriptBuilder builder;
	int ret = 0;

	ret = builder.StartNewModule(m_ASEngine, sModule.c_str());
	if(ret < 0)
	{
		return XEResult::ASModuleCreateFail;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Let the builder load the script, and do the necessary pre-processing (include files, etc)
	ret = builder.AddSectionFromFile(sScriptName.c_str());
	if(ret < 0)
	{
		m_ASEngine->DiscardModule(sModule.c_str());
		return XEResult::ASAddSecModuleFail;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Build Module
	ret = builder.BuildModule();
	if(ret < 0)
	{
		m_ASEngine->DiscardModule(sModule.c_str());
		return XEResult::ASBuildModuleFail;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Assigned Module to Parameter if non null
	if (asModule != nullptr)
	{
		tempASModule = m_ASEngine->GetModule(sModule.c_str());

		*asModule = tempASModule;
	}

	return XEResult::Ok;
}

XEResult AngelScriptManager::LoadScript(const std::vector<std::wstring>& scriptNames, const std::wstring& module, asIScriptModule** asModule)
{
	if(!m_IsReady)
	{
		return XEResult::NotReady;
	}

	if(scriptNames.size() == 0)
	{
		return XEResult::ZeroSize;
	}

	std::string sModule = XE_Base::WideStr2String(module);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Check if the script has already been loaded
	asIScriptModule* tempASModule = m_ASEngine->GetModule(sModule.c_str());

	if (tempASModule != nullptr)
	{
		if (asModule != nullptr)
		{
			*asModule = tempASModule;
		}

		return XEResult::ASModuleAlreadyCreated;
	}

	CScriptBuilder builder;
	int ret = 0;

	ret = builder.StartNewModule(m_ASEngine, sModule.c_str());
	if(ret < 0)
	{
		return XEResult::ASModuleCreateFail;
	}

	uint32_t size = (uint32_t)scriptNames.size();
	for(uint32_t i = 0; i < size; ++i)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// Let the builder load the script, and do the necessary pre-processing (include files, etc)
		std::string sScriptName = XE_Base::WideStr2String(scriptNames[i]);

		ret = builder.AddSectionFromFile(sScriptName.c_str());
		if(ret < 0)
		{
			m_ASEngine->DiscardModule(sModule.c_str());
			return XEResult::ASAddSecModuleFail;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Build Module
	ret = builder.BuildModule();
	if(ret < 0)
	{
		m_ASEngine->DiscardModule(sModule.c_str());
		return XEResult::ASBuildModuleFail;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Assigned Module to Parameter if non null
	if (asModule != nullptr)
	{
		tempASModule = m_ASEngine->GetModule(sModule.c_str());

		*asModule = tempASModule;
	}

	return XEResult::Ok;
}

XEResult AngelScriptManager::RemoveModule(const std::wstring& module)
{
	if(!m_IsReady)
	{
		return XEResult::NotReady;
	}
	
	std::string sModule = XE_Base::WideStr2String(module);

	int ret = m_ASEngine->DiscardModule(sModule.c_str());
	if(ret < 0)
	{
		return XEResult::ASDiscardModuleFail;
	}

	return XEResult::Ok;
}

void AngelScriptManager::ASMessageCallback(const asSMessageInfo& msg)
{
	LogLevel logLVL = LogLevel::Info;

	switch(msg.type)
	{
		case asMSGTYPE_INFORMATION:
			logLVL = LogLevel::Info;
			break;
		case asMSGTYPE_WARNING:
			logLVL = LogLevel::Warning;
			break;
		case asMSGTYPE_ERROR:
			logLVL = LogLevel::Error;
			break;
		default:
			logLVL = LogLevel::Debug;
			break;
	}

	std::wstring errMsg = L"";
	std::wstring wSection = XE_Base::String2WideStr(msg.section);
	std::wstring wMessage = XE_Base::String2WideStr(msg.message);
	fastformat::fmt(errMsg, XELOCMAN->GetLiteral(L"AS_ENGINE_CALLBACK_ERR_MSG"), __FUNCTIONW__, wSection, msg.row, msg.col, wMessage);
	XELOGGER->AddNewLog(logLVL, errMsg);
}