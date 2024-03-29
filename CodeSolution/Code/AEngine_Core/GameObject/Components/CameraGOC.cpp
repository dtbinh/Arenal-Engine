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

/***************************
*   Game Engine Includes   *
****************************/
#include "CameraGOC.h"
#include "Camera\Camera.h"
#include "GraphicDevice.h"
#include "Base\BaseFunctions.h"
#include "Camera\CameraManager.h"
#include "GameObject\GameObject.h"
#include "Camera\GameObjectCamera.h"

//Always include last
#include "Memory\MemLeaks.h"

/********************
*   Function Defs   *
*********************/

AETODO("Add mutex");
CameraGOC::CameraGOC(GameObject* gameObject, CameraManager* cameraManager, GraphicDevice* graphicDevice)
	: GameObjectComponent(gameObject, GameObjectComponentType::Camera)
	, m_CameraManager(cameraManager)
	, m_GraphicDevice(graphicDevice)
{
	AEAssert(m_CameraManager != nullptr);
	AEAssert(m_GraphicDevice != nullptr);

	if (m_GraphicDevice != nullptr)
	{
		AETODO("See how to add better name");
		AETODO("Far and Near Att");

		glm::ivec2 dimension(m_GraphicDevice->GetGraphicPP().m_BackBufferWidth, m_GraphicDevice->GetGraphicPP().m_BackBufferHeight);
		m_Camera = new GameObjectCamera(L"Game Object Camera", AEMathHelpers::Vec3fZero, AEMathHelpers::Vec3fFwrZP, AEMathHelpers::Vec3fUp, dimension, 45.0f, 1.0f, 1000.0f);
	}

	if (m_CameraManager != nullptr && m_Camera != nullptr)
	{
		m_CameraManager->AddCamera(m_Camera);

		m_IsReady = true;
	}
}

CameraGOC::~CameraGOC()
{
	if (m_IsReady)
	{
		m_CameraManager->RemoveCamera(m_Camera->GetUniqueID());
	}
}

bool CameraGOC::IsDefaultCamera()
{
	if (!m_IsReady)
	{
		return false;
	}

	return (m_CameraManager->GetDefaultCameraID() == m_Camera->GetUniqueID());
}

AEResult CameraGOC::SetAsDefaultCamera()
{
	if (!m_IsReady)
	{
		return AEResult::NotReady;
	}

	return m_CameraManager->SetDefaultCamera(m_Camera->GetUniqueID());
}
