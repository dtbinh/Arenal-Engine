/********************************************************
*
* Author: Carlos Chac�n N.
*
* Copyright 2012-2015
*
*********************************************************/

/**********************
*   System Includes   *
***********************/

/*************************
*   3rd Party Includes   *
**************************/
#include "cppformat\format.h"
#include "boost\filesystem.hpp"

/***************************
*   Game Engine Includes   *
****************************/
#include "ImporterHLSL.h"
#include "Logger\Logger.h"
#include "Base\BaseFunctions.h"
#include "Content\ShaderContent.h"
#include "Localization\LocalizationManager.h"
#include "Localization\LocalizationManagerDefs.h"
#include "Shaders\Variables\ShaderCustomVariable.h"

//Always include last
#include "Memory\MemLeaks.h"

/************
*   Using   *
*************/

/********************
*   Function Defs   *
*********************/
AETODO("Using D3DCompiler, add Editor Flag");
ImporterHLSL::ImporterHLSL()
	: m_IncludeInterfaceHLSL(boost::filesystem::current_path().c_str())
{
}

ImporterHLSL::~ImporterHLSL()
{
}

AEResult ImporterHLSL::BuildShaderTarget(ShaderType shaderType, ShaderModel shaderModel, std::string& target)
{
	target = "";

	switch (shaderType)
	{
		case ShaderType::VertexShader:
			target = "vs_";
			break;
		case ShaderType::PixelShader:
			target = "ps_";
			break;
		case ShaderType::GeometryShader:
			target = "gs_";
			break;
		case ShaderType::HullShader:
			target = "hs_";
			break;
		case ShaderType::ComputeShader:
			target = "cs_";
			break;
		case ShaderType::DomainShader:
			target = "ds_";
			break;
		default:
			AETODO("add log");
			return AEResult::InvalidShaderType;
			break;
	}

	AETODO("add to game app config options, what shader model should be use");
	switch (shaderModel)
	{
		case ShaderModel::SM_3_0:
			target += "3_0";
			break;
		case ShaderModel::SM_4_0:
			target += "4_0";
			break;
		case ShaderModel::SM_4_1:
			target += "4_1";
			break;
		case ShaderModel::SM_5_0:
			target += "5_0";
			break;
		default:
			AETODO("add log");
			return AEResult::InvalidShaderModel;
			break;
	}

	return AEResult::Ok;
}

AEResult ImporterHLSL::ImportShader(const std::wstring& filename, ShaderType shaderType, ShaderModel shaderModel, ShaderContent** shaderContent)
{
	AEAssert(shaderContent != nullptr);
	if(shaderContent == nullptr)
	{
		return AEResult::NullParameter;
	}
	
	std::string shaderTarget = "";
	ID3DBlob* shaderDX = nullptr;
	ID3DBlob* err = nullptr;

	/////////////////////////////////////////////////////
	//Build Shader Target

	AEResult ret = BuildShaderTarget(shaderType, shaderModel, shaderTarget);

	if(ret != AEResult::Ok)
	{
		return ret;
	}
	
	/////////////////////////////////////////////////////
	//Set Compiler Flags
	UINT compilerFlags = (D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR);
	
#if defined(DEBUG) | defined(_DEBUG)
	compilerFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
#else
	compilerFlags |= (D3DCOMPILE_OPTIMIZATION_LEVEL3);
#endif
	
	/////////////////////////////////////////////////////
	//Compile Shader
	HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr, &m_IncludeInterfaceHLSL, "main", shaderTarget.c_str(), compilerFlags, 0, &shaderDX, &err);
	
	if(hr != S_OK)
	{
		DisplayError(hr);

		char* errorDX = reinterpret_cast<char*>(err->GetBufferPointer());

		std::wstring strErrorDX = AE_Base::String2WideStr(errorDX);
		std::wstring name = AE_Base::GetFilenameOnly(filename);;

		ReleaseCOM(err);

		std::wstring msg_error = fmt::format(AELOCMAN->GetLiteral(L"SHADER_COMPILARION_FAILED_ERR_MSG"), name, strErrorDX);
		AELOGGER->AddNewLog(LogLevel::Error, msg_error);

		return AEResult::ShaderCompiledFail;
	}
	
	/////////////////////////////////////////////////////
	//Create Shader Content
	ShaderContent* shaderContentTemp = new ShaderContent();

	shaderContentTemp->m_Name			= AE_Base::GetFilenameOnly(filename);

	shaderContentTemp->m_ShaderType		= shaderType;
	shaderContentTemp->m_ByteCodeSize	= (uint32_t)shaderDX->GetBufferSize();
	shaderContentTemp->m_ShaderByteCode	= new char[shaderContentTemp->m_ByteCodeSize];

	memset(shaderContentTemp->m_ShaderByteCode, 0, sizeof(char) * shaderContentTemp->m_ByteCodeSize);

	memcpy(shaderContentTemp->m_ShaderByteCode, shaderDX->GetBufferPointer(), shaderContentTemp->m_ByteCodeSize);

	/////////////////////////////////////////////////////
	//Extract Shader Information
	ret = ExtractShaderProperties(shaderContentTemp, shaderDX);

	if(ret != AEResult::Ok)
	{
		DeleteMem(shaderContentTemp);

		ReleaseCOM(shaderDX);

		return ret;
	}

	/////////////////////////////////////////////////////
	//Finish
	*shaderContent = shaderContentTemp;

	ReleaseCOM(shaderDX);
	
	return AEResult::Ok;
}

AEResult ImporterHLSL::ExtractShaderProperties(ShaderContent* content, ID3DBlob* shaderBlob)
{
	AEAssert(content != nullptr);
	AEAssert(shaderBlob != nullptr);
	if(content == nullptr || shaderBlob == nullptr)
	{
		return AEResult::NullParameter;
	}

	ID3D11ShaderReflection* shaderReflector = nullptr; 
	AEResult ret = AEResult::Ok;
	
	//////////////////////////////////////////////////
	//Get Shader Properties using reflection
	HRESULT hr = D3DReflect(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&shaderReflector);

	if(hr != S_OK)
	{
		DisplayError(hr);

		return AEResult::Fail;
	}

	//////////////////////////////////////////////////
	//Extract Shader Constant Buffer Information
	ret = ExtractShaderConstantBuffers(content, shaderReflector);

	if(ret != AEResult::Ok)
	{
		AETODO("Log error");

		return AEResult::ShaderExtractConstantBuffersFail;
	}

	//////////////////////////////////////////////////
	//Extract Shader Bound Resources Information
	ret = ExtractBoundResources(content, shaderReflector);

	if(ret != AEResult::Ok)
	{
		AETODO("Log error");

		return ret;
	}

	//////////////////////////////////////////////////
	//Finish
	return AEResult::Ok;
}

AEResult ImporterHLSL::ExtractShaderConstantBuffers(ShaderContent* content, ID3D11ShaderReflection* shaderReflector)
{
	AEAssert(content != nullptr);
	AEAssert(shaderReflector != nullptr);

	if(content == nullptr || shaderReflector == nullptr)
	{
		return AEResult::NullParameter;
	}

	//////////////////////////////////////////////////
	//Get number of Constant Buffers
	D3D11_SHADER_DESC descShader;
	shaderReflector->GetDesc(&descShader);
	uint32_t numConstantBuffers = (uint32_t)descShader.ConstantBuffers;

	//////////////////////////////////////////////////
	//Extract Constant Buffers
	for (uint32_t i = 0; i < numConstantBuffers; i++)
	{
		//////////////////////////////////////////////////
		//Get Constant Buffer Info
		ID3D11ShaderReflectionConstantBuffer* constantBufferReflect = shaderReflector->GetConstantBufferByIndex(i);		
		D3D11_SHADER_BUFFER_DESC descCB;
		constantBufferReflect->GetDesc(&descCB);

		if(descCB.Type != D3D_CT_CBUFFER)
		{
			AETODO("add warning");

			continue;
		}

		//////////////////////////////////////////////////
		//Set Constant Buffer Basic Info
		ConstantBufferShaderHolder cbHolder;
		cbHolder.m_Name = AE_Base::String2WideStr(descCB.Name);

		//////////////////////////////////////////////////
		//Extract Variable Information
		uint32_t currentOffset = 0;
		uint32_t numVars = (uint32_t)descCB.Variables;
		for (uint32_t j = 0; j < numVars; j++)
		{
			//////////////////////////////////////////////////
			//Get Variable Info
			ID3D11ShaderReflectionVariable* cbVariable = constantBufferReflect->GetVariableByIndex(j);
			D3D11_SHADER_VARIABLE_DESC descVar;
			cbVariable->GetDesc(&descVar);

			//Gets Specific information for the Variable
			ID3D11ShaderReflectionType* varType = cbVariable->GetType();
			D3D11_SHADER_TYPE_DESC descVarType;
			varType->GetDesc(&descVarType);

			//Calculates Variable Offset between each variable
			uint32_t variableOffset = descVar.StartOffset - currentOffset;
			currentOffset += (variableOffset + (uint32_t)descVar.Size);

			//////////////////////////////////////////////////
			//Set Variable Info
			ShaderCustomVariable shaderVar;

			shaderVar.m_Name				= AE_Base::String2WideStr(descVar.Name);
			shaderVar.m_StartOffset			= descVar.StartOffset;
			shaderVar.m_Offset				= variableOffset;
			shaderVar.m_Size				= (uint32_t)descVar.Size;
			shaderVar.m_Elements			= descVarType.Elements;
			shaderVar.m_IsArray				= (shaderVar.m_Elements != 0);
			shaderVar.m_ElementSize			= ( (shaderVar.m_Elements != 0) ? (shaderVar.m_Size / shaderVar.m_Elements) : shaderVar.m_Size );
			shaderVar.m_UserVariable		= (shaderVar.m_Name.compare(0, 3, L"_AE") != 0); //If no _AE prefix then its a user variable
			shaderVar.m_Columns				= descVarType.Columns;
			shaderVar.m_Rows				= descVarType.Rows;
			shaderVar.m_ShaderVariableClass	= GetShaderVariableClass(descVarType.Class);
			shaderVar.m_ShaderVariableType	= GetShaderVariableType(descVarType.Type);

			//////////////////////////////////////////////////
			//Add Variable to Constant Buffer Holder
			cbHolder.m_ShaderCustomVariableList.push_back(shaderVar);
		}

		content->m_ConstantBufferShaderHolderList.push_back(cbHolder);
	}

	//////////////////////////////////////////////////
	//Set Bind Index for the Constant Buffers we have
	uint32_t boundResources = descShader.BoundResources;
	for (uint32_t i = 0; i < boundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC descBoundResources;
		shaderReflector->GetResourceBindingDesc(i, &descBoundResources);

		std::wstring brName = AE_Base::String2WideStr(descBoundResources.Name);

		for(ConstantBufferShaderHolder& cbHolder : content->m_ConstantBufferShaderHolderList)
		{
			if(cbHolder.m_Name.compare(brName) == 0)
			{
				cbHolder.m_BindIndex = descBoundResources.BindPoint;
				break;
			}
		}
	}

	/////////////////////////////////////////////////////
	//Finish
	return AEResult::Ok;
}

AEResult ImporterHLSL::ExtractBoundResources(ShaderContent* content, ID3D11ShaderReflection* shaderReflector)
{
	AEAssert(content != nullptr);
	AEAssert(shaderReflector != nullptr);

	if(content == nullptr || shaderReflector == nullptr)
	{
		return AEResult::NullParameter;
	}

	AEResult ret = AEResult::Ok;

	//////////////////////////////////////////////////
	//Get number of Bound Resources
	D3D11_SHADER_DESC descShader;
	shaderReflector->GetDesc(&descShader);
	uint32_t numBoundResources = (uint32_t)descShader.BoundResources;

	//////////////////////////////////////////////////
	//Extract Texture Inputs
	for (uint32_t i = 0; i < numBoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC descBoundResources;
		shaderReflector->GetResourceBindingDesc(i, &descBoundResources);

		D3D_SHADER_INPUT_TYPE inputType = descBoundResources.Type;
		D3D_SRV_DIMENSION dimType = descBoundResources.Dimension;

		if( (inputType == D3D_SIT_STRUCTURED || inputType == D3D11_SIT_UAV_RWSTRUCTURED) && dimType == D3D_SRV_DIMENSION_BUFFER )
		{
			ret = ExtractShaderStructuredBuffers(content, descBoundResources);

			if(ret != AEResult::Ok)
			{
				AETODO("Create error log");

				return AEResult::ShaderExtractStructuredBuffersFail;
			}
		}
		else if( (inputType == D3D_SIT_UAV_RWTYPED || inputType == D3D_SIT_TEXTURE) && dimType == D3D_SRV_DIMENSION_BUFFER )
		{
			ret = ExtractShaderSimpleBuffers(content, descBoundResources);

			if(ret != AEResult::Ok)
			{
				AETODO("Create error log");

				return AEResult::ShaderExtractSimpleBuffersFail;
			}
		}
		else if (inputType == D3D_SIT_SAMPLER)
		{
			ret = ExtractShaderSamplers(content, descBoundResources);

			if (ret != AEResult::Ok)
			{
				AETODO("Create error log");

				AETODO("Add better return code");
				return AEResult::Fail;
			}
		}
		else if(inputType == D3D_SIT_TEXTURE)
		{
			ret = ExtractShaderTextureInputs(content, descBoundResources);

			if(ret != AEResult::Ok)
			{
				AETODO("Create error log");

				return AEResult::ShaderExtractTextureInputFail;
			}
		}
		else
		{
			AETODO("Log debug informing that this is not been imported");
		}
	}

	/////////////////////////////////////////////////////
	//Finish
	return AEResult::Ok;
}

AEResult ImporterHLSL::ExtractShaderSimpleBuffers(ShaderContent* content, const D3D11_SHADER_INPUT_BIND_DESC& descBoundResources)
{
	AEAssert(content != nullptr);
	if(content == nullptr)
	{
		return AEResult::NullParameter;
	}

	AEAssert( descBoundResources.Type == D3D_SIT_UAV_RWTYPED || descBoundResources.Type == D3D_SIT_TEXTURE );
	AEAssert( descBoundResources.Dimension == D3D_SRV_DIMENSION_BUFFER );
	if( (descBoundResources.Type != D3D_SIT_UAV_RWTYPED && descBoundResources.Type != D3D_SIT_TEXTURE) || descBoundResources.Dimension != D3D_SRV_DIMENSION_BUFFER )
	{
		return AEResult::InvalidObjType;
	}

	SimpleBufferShaderHolder simpleBufferShaderHolder;

	simpleBufferShaderHolder.m_Name			= AE_Base::String2WideStr(descBoundResources.Name);
	simpleBufferShaderHolder.m_BindIndex	= descBoundResources.BindPoint;
	simpleBufferShaderHolder.m_IsRW			=(descBoundResources.Type == D3D_SIT_UAV_RWTYPED);

	switch (descBoundResources.ReturnType)
	{
		case D3D_RETURN_TYPE_SINT:
			simpleBufferShaderHolder.m_VariableType = ShaderVariableType::Int;
			break;

		case D3D_RETURN_TYPE_UINT:
			simpleBufferShaderHolder.m_VariableType = ShaderVariableType::UInt;
			break;

		case D3D_RETURN_TYPE_FLOAT:
			simpleBufferShaderHolder.m_VariableType = ShaderVariableType::Float;
			break;

		case D3D_RETURN_TYPE_DOUBLE:
			simpleBufferShaderHolder.m_VariableType = ShaderVariableType::Double;
			break;

		default:
			AETODO("Log error");

			return AEResult::InvalidObjType;
	}

	if(descBoundResources.uFlags & D3D_SIF_TEXTURE_COMPONENTS)
	{
		simpleBufferShaderHolder.m_VariableClass = ShaderVariableClass::Vector;

		if( (descBoundResources.uFlags & D3D_SIF_TEXTURE_COMPONENT_0 ) && ( descBoundResources.uFlags & D3D_SIF_TEXTURE_COMPONENT_1) )
		{
			simpleBufferShaderHolder.m_ElementCount = 4;
		}
		else if(descBoundResources.uFlags & D3D_SIF_TEXTURE_COMPONENT_1)
		{
			simpleBufferShaderHolder.m_ElementCount = 3;
		}
		else if(descBoundResources.uFlags & D3D_SIF_TEXTURE_COMPONENT_0)
		{
			simpleBufferShaderHolder.m_ElementCount = 2;
		}
		else
		{
			AETODO("Log error");

			return AEResult::InvalidObjType;
		}
	}
	else
	{
		simpleBufferShaderHolder.m_VariableClass = ShaderVariableClass::Scalar;
		simpleBufferShaderHolder.m_ElementCount = 1;
	}

	content->m_SimpleBufferShaderHolderList.push_back(simpleBufferShaderHolder);

	/////////////////////////////////////////////////////
	//Finish
	return AEResult::Ok;
}

AEResult ImporterHLSL::ExtractShaderStructuredBuffers(ShaderContent* content, const D3D11_SHADER_INPUT_BIND_DESC& descBoundResources)
{
	AEAssert(content != nullptr);
	if(content == nullptr)
	{
		return AEResult::NullParameter;
	}

	AEAssert( descBoundResources.Type == D3D_SIT_STRUCTURED || descBoundResources.Type == D3D_SIT_UAV_RWSTRUCTURED );
	AEAssert( descBoundResources.Dimension == D3D_SRV_DIMENSION_BUFFER );
	if( (descBoundResources.Type != D3D_SIT_STRUCTURED && descBoundResources.Type != D3D_SIT_UAV_RWSTRUCTURED) || descBoundResources.Dimension != D3D_SRV_DIMENSION_BUFFER )
	{
		return AEResult::InvalidObjType;
	}

	StructuredBufferShaderHolder structuredBufferHolder;

	structuredBufferHolder.m_Name			= AE_Base::String2WideStr(descBoundResources.Name);
	structuredBufferHolder.m_BindIndex		= descBoundResources.BindPoint;
	structuredBufferHolder.m_IsRW			=(descBoundResources.Type == D3D_SIT_UAV_RWSTRUCTURED);

	content->m_StructuredBufferShaderHolderList.push_back(structuredBufferHolder);

	/////////////////////////////////////////////////////
	//Finish
	return AEResult::Ok;
}

AEResult ImporterHLSL::ExtractShaderTextureInputs(ShaderContent* content, const D3D11_SHADER_INPUT_BIND_DESC& descBoundResources)
{
	AEAssert(content != nullptr);
	if(content == nullptr)
	{
		return AEResult::NullParameter;
	}

	AEAssert(descBoundResources.Type == D3D_SIT_TEXTURE);
	if(descBoundResources.Type != D3D_SIT_TEXTURE)
	{
		return AEResult::InvalidObjType;
	}

	bool isTextArray = false;
	TextureShaderVariableHolder tsHolder;
	TextureArrayShaderVariableHolder tsArrayHolder;
	
	//////////////////////////////////////////////////
	//Get what type of texture we are expecting
	switch (descBoundResources.Dimension)
	{
		case D3D_SRV_DIMENSION_TEXTURE2D:
			tsHolder.m_TextureType = TextureType::Texture2D;
			break;

		case D3D_SRV_DIMENSION_TEXTURECUBE:
			tsHolder.m_TextureType = TextureType::TextureCube;
			break;

		case D3D_SRV_DIMENSION_TEXTURE2DARRAY:
			isTextArray = true;
			tsArrayHolder.m_TextureType = TextureType::Texture2D;
			break;

		case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:
			isTextArray = true;
			tsArrayHolder.m_TextureType = TextureType::TextureCube;
			break;

		default:
			AEAssert(false);
			AETODO("Log Error");

			return AEResult::InvalidObjType;
	}

	if (isTextArray)
	{
		tsArrayHolder.m_Name = AE_Base::String2WideStr(descBoundResources.Name);
		tsArrayHolder.m_BindIndex = descBoundResources.BindPoint;

		content->m_TextureArrayShaderVariableHolderList.push_back(tsArrayHolder);
	}
	else
	{
		tsHolder.m_Name = AE_Base::String2WideStr(descBoundResources.Name);
		tsHolder.m_BindIndex = descBoundResources.BindPoint;

		content->m_TextureShaderVariableHolderList.push_back(tsHolder);
	}

	/////////////////////////////////////////////////////
	//Finish
	return AEResult::Ok;
}

AEResult ImporterHLSL::ExtractShaderSamplers(ShaderContent* content, const D3D11_SHADER_INPUT_BIND_DESC& descBoundResources)
{
	AEAssert(content != nullptr);
	if (content == nullptr)
	{
		return AEResult::NullParameter;
	}

	AEAssert(descBoundResources.Type == D3D_SIT_SAMPLER);
	if (descBoundResources.Type != D3D_SIT_SAMPLER)
	{
		return AEResult::InvalidObjType;
	}

	SamplerShaderHolder samplerShaderHolder;

	samplerShaderHolder.m_Name			= AE_Base::String2WideStr(descBoundResources.Name);
	samplerShaderHolder.m_BindIndex		= descBoundResources.BindPoint;

	content->m_SamplerShaderHolderList.push_back(samplerShaderHolder);

	/////////////////////////////////////////////////////
	//Finish
	return AEResult::Ok;
}

ShaderVariableClass ImporterHLSL::GetShaderVariableClass(D3D_SHADER_VARIABLE_CLASS dxClass)
{
	switch (dxClass)
	{
		case D3D_SVC_SCALAR:
			return ShaderVariableClass::Scalar;
		case D3D_SVC_VECTOR:
			return ShaderVariableClass::Vector;
		case D3D_SVC_MATRIX_COLUMNS:
			return ShaderVariableClass::Matrix;
		case D3D_SVC_STRUCT:
			return ShaderVariableClass::Structure;

		default:
			AEAssert(false);
			AETODO("add error");
			return ShaderVariableClass::Scalar;
	}
}

ShaderVariableType ImporterHLSL::GetShaderVariableType(D3D_SHADER_VARIABLE_TYPE  dxType)
{
	switch (dxType)
	{
		case D3D_SVT_VOID:
			return ShaderVariableType::Void;
		case D3D_SVT_BOOL:
			return ShaderVariableType::Boolean;
		case D3D_SVT_INT:
			return ShaderVariableType::Int;
		case D3D_SVT_FLOAT:
			return ShaderVariableType::Float;
		case D3D_SVT_TEXTURE2D:
			return ShaderVariableType::Texture2D;
		case D3D_SVT_TEXTURE3D:
			return ShaderVariableType::Texture3D;
		case D3D_SVT_TEXTURECUBE:
			return ShaderVariableType::TextureCube;
		case D3D_SVT_SAMPLER:
			return ShaderVariableType::Sampler;
		case D3D_SVT_UINT:
			return ShaderVariableType::UInt;
		case D3D_SVT_UINT8:
			return ShaderVariableType::UInt8;

		default:
			AEAssert(false);
			AETODO("add error");
			return ShaderVariableType::Float;
	}
}