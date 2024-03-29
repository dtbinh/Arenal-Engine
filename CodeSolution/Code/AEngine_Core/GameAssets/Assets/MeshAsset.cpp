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
#include "MeshAsset.h"
#include "Models\Mesh.h"
#include "Base\BaseFunctions.h"
#include "Resource\GameResourceManager.h"

//Always include last
#include "Memory\MemLeaks.h"

/********************
*   Function Defs   *
*********************/
AETODO("Check if this class needs a mutex");
MeshAsset::MeshAsset(const std::wstring& filePath, GameResourceManager* gameResourceManager, GraphicDevice* graphicDevice)
	: GameAsset(GameContentType::Mesh, filePath, gameResourceManager)
	, m_GraphicDevice(graphicDevice)
{
	AEAssert(m_GraphicDevice != nullptr);
}

MeshAsset::~MeshAsset()
{
	AERelease(m_Mesh);
}

Mesh* MeshAsset::GetMeshReference()
{
	if(m_Mesh == nullptr)
	{
		return nullptr;
	}

	return reinterpret_cast<Mesh*>(m_Mesh->AddRef());
}

AEResult MeshAsset::LoadAssetResource()
{
	AEAssert(m_GraphicDevice != nullptr);
	if (m_GraphicDevice == nullptr)
	{
		return AEResult::GraphicDeviceNull;
	}

	AEAssert(m_GameResourceManager != nullptr);
	if (m_GameResourceManager == nullptr)
	{
		return AEResult::GameResourceManagerNull;
	}

	AEAssert(!m_FilePath.empty());
	if(m_FilePath.empty())
	{
		return AEResult::EmptyFilename;
	}

	AEResult ret = AEResult::Ok;

	AETODO("Look into what happens if file changes location");

	if(m_Mesh != nullptr)
	{
		AETODO("Check return value");
		ret = m_Mesh->Load();
	}
	else
	{
		/////////////////////////////////////////////
		//Check if Game Resources contains this Mesh
		m_Mesh = (Mesh*)m_GameResourceManager->AcquireGameResourceByStringID(m_FilePath, GameResourceType::Mesh);

		if(m_Mesh != nullptr)
		{
			AETODO("Check if we always need to reload");
			AETODO("Check return value");
			m_Mesh->Load();

			return AEResult::Ok;
		}
		
		/////////////////////////////////////////////
		//Create Resource
		m_Mesh = new Mesh(m_GraphicDevice, m_Name);
		
		/////////////////////////////////////////////
		//Set File Name and Load
		m_Mesh->SetFileName(m_FilePath);
				
		AETODO("Check return value");
		m_Mesh->Load();

		/////////////////////////////////////////////
		//Add to Resource Manager
		ret = m_GameResourceManager->ManageGameResource(m_Mesh, m_FilePath);
		if(ret != AEResult::Ok)
		{
			AETODO("Add log");

			AERelease(m_Mesh);

			return AEResult::ResourceManagedFailed;
		}
	}

	return AEResult::Ok;
}