
/********************************************************
*
* Author: Carlos Chac�n N.
*
* Created: 11/10/2012
*
* Last Major Update: 12/30/2013
* 
* Desc:
*
*
* Copyright 2013
*
*********************************************************/


/**********************
*   System Includes   *
***********************/

/*************************
*   3rd Party Includes   *
**************************/

/***************************
*   Game Engine Includes   *
****************************/
#include "GraphicDevice.h"
#include "Base\BaseFunctions.h"
#include "Shaders\PixelShader.h"
#include "Shaders\VertexShader.h"
#include "VarianceShadowMaterial.h"
#include "Resource\GameResourceManager.h"
#include "Shaders\Buffers\ConstantBuffer.h"
#include "Shaders\Variables\ShaderCustomVariable.h"
#include "Graphic Extensions\Shader Extensions\Properties\ShaderProperties.h"

#if defined(DEBUG) | defined(_DEBUG)
	#if defined(_WIN64) | defined (WIN64) 
#include "Compiled Materials\HLSL\VarianceShadowVS_x64_d.h"
#include "Compiled Materials\HLSL\VarianceShadowPS_x64_d.h"
	#else
#include "Compiled Materials\HLSL\VarianceShadowVS_x86_d.h"
#include "Compiled Materials\HLSL\VarianceShadowPS_x86_d.h"
	#endif
#else
	#if defined(_WIN64) | defined (WIN64) 
#include "Compiled Materials\HLSL\VarianceShadowVS_x64.h"
#include "Compiled Materials\HLSL\VarianceShadowPS_x64.h"
	#else
#include "Compiled Materials\HLSL\VarianceShadowVS_x86.h"
#include "Compiled Materials\HLSL\VarianceShadowPS_x86.h"
	#endif
#endif

//Always include last
#include "Memory\MemLeaks.h"

/********************
*   Function Defs   *
*********************/
VarianceShadowMaterial::VarianceShadowMaterial(GraphicDevice* graphicDevice, GameResourceManager* gameResourceManager, const std::wstring& name)
	: Material(graphicDevice, gameResourceManager, name)
{
}

VarianceShadowMaterial::~VarianceShadowMaterial()
{
}

XEResult VarianceShadowMaterial::CreateVertexShader()
{
	XEResult ret = XEResult::Ok;

	/////////////////////////////////////////////////////
	//Get Vertex Shader from Game Resources
	m_VertexShader = (VertexShader*)m_GameResourceManager->AcquireGameResourceByStringID(XE_VARIANCE_SHADOW_MAT_VS_NAME, GameResourceType::VertexShader);

	if(m_VertexShader == nullptr)
	{
		m_VertexShader = new VertexShader(m_GraphicDevice, XE_VARIANCE_SHADOW_MAT_VS_NAME);
		ret = m_VertexShader->LoadShader(VarianceShadowVS, sizeof(VarianceShadowVS));
		if(ret != XEResult::Ok)
		{
			return XEResult::VertexShaderLoadFailed;
		}

		ret = m_GameResourceManager->ManageGameResource(m_VertexShader, XE_VARIANCE_SHADOW_MAT_VS_NAME);
		if(ret != XEResult::Ok)
		{
			return XEResult::ResourceManagedFailed;
		}
	}

	/////////////////////
	//Create Properties
	m_VSProps = new ShaderProperties(ShaderType::VertexShader, m_GraphicDevice);

	ret = CreateVertexShaderConstantBuffer();

	if(ret != XEResult::Ok)
	{
		return XEResult::ConstantBufferInitFailed;
	}

	return XEResult::Ok;
}

XEResult VarianceShadowMaterial::CreateVertexShaderConstantBuffer()
{
	XEResult ret = XEResult::Ok;
	
	/////////////////////////////////////////////////////
	//Create Constant Buffer 
	//	- _XE_CB_World_View_Proj
	//
	
	/****************************************************************************
	*Constant Buffer #1: _XE_CB_World_View_Proj
	****************************************************************************/
	ConstantBuffer* cbWVP = nullptr;

	ret = XEBuiltInMaterialsHelpers::BuildCBWVP(m_GraphicDevice, &cbWVP);

	if(ret != XEResult::Ok)
	{
		return XEResult::ConstantBufferInitFailed;
	}

	ret = m_VSProps->AddConstantBuffer(cbWVP);

	if(ret != XEResult::Ok)
	{
		DeleteMem(cbWVP);

		return XEResult::ConstantBufferInitFailed;
	}

	/****************************************************************************
	*Finish
	****************************************************************************/
	return XEResult::Ok;
}

XEResult VarianceShadowMaterial::CreatePixelShader()
{
	XEResult ret = XEResult::Ok;

	/////////////////////////////////////////////////////
	//Get Pixel Shader from Game Resources
	m_PixelShader = (PixelShader*)m_GameResourceManager->AcquireGameResourceByStringID(XE_VARIANCE_SHADOW_MAT_PS_NAME, GameResourceType::PixelShader);

	if (m_PixelShader == nullptr)
	{
		m_PixelShader = new PixelShader(m_GraphicDevice, XE_VARIANCE_SHADOW_MAT_PS_NAME);
		ret = m_PixelShader->LoadShader(VarianceShadowPS, sizeof(VarianceShadowPS));
		if (ret != XEResult::Ok)
		{
			return XEResult::VertexShaderLoadFailed;
		}

		ret = m_GameResourceManager->ManageGameResource(m_PixelShader, XE_VARIANCE_SHADOW_MAT_PS_NAME);
		if (ret != XEResult::Ok)
		{
			return XEResult::ResourceManagedFailed;
		}
	}

	/////////////////////
	//Create Properties
	m_PSProps = new ShaderProperties(ShaderType::PixelShader, m_GraphicDevice);

	return XEResult::Ok;
}

XEResult VarianceShadowMaterial::LoadContent()
{
	XEAssert(m_GraphicDevice != nullptr);

	if(m_GraphicDevice == nullptr)
	{
		return XEResult::GraphicDeviceNull;
	}

	XEAssert(m_GameResourceManager != nullptr);

	if(m_GameResourceManager == nullptr)
	{
		return XEResult::GameResourceManagerNull;
	}

	if(m_IsReady)
	{
		return XEResult::Ok;
	}

	////////////////////////////////////
	//Clean up memory
	CleanUp();

	XEResult ret = XEResult::Ok;

	/***************************
	*Vertex Shader
	***************************/
	ret = CreateVertexShader();

	if(ret != XEResult::Ok)
	{
		CleanUp();

		return ret;
	}

	/***************************
	*Pixel Shader
	***************************/
	ret = CreatePixelShader();

	if (ret != XEResult::Ok)
	{
		CleanUp();

		return ret;
	}

	/***************************
	*Finish
	***************************/
	m_IsReady = true;

	return XEResult::Ok;
}
