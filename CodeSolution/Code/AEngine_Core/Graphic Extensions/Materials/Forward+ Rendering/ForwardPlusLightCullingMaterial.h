/********************************************************
*
* Author: Carlos Chac�n N.
*
* Copyright 2012-2015
*
*********************************************************/

#pragma once

#ifndef _FORWARD_PLUS_LIGHT_CULLING_MATERIAL_H
#define _FORWARD_PLUS_LIGHT_CULLING_MATERIAL_H

/**********************
*   System Includes   *
***********************/
#include <string>
#include <vector>

/*************************
*   3rd Party Includes   *
**************************/

/***************************
*   Game Engine Includes   *
****************************/
#include "Base\Base.h"
#include "Graphic Extensions\Materials\Material.h"
#include "Graphic Extensions\Materials\BuiltInMaterialsDefs.h"

/************
*   Using   *
*************/

/********************
*   Forward Decls   *
*********************/
class GraphicDevice;
class GameResourceManager;

/*****************
*   Class Decl   *
******************/
class ForwardPlusLightCullingMaterial sealed : public Material
{
	private:

		/*************************
		 *   Private Variables   *
		 *************************/
#pragma region Private Variables

#pragma endregion

		/***********************
		 *   Private Methods   *
		 ***********************/
#pragma region Private Methods

		AEResult CreateComputeShader();

		AEResult CreateComputeShaderConstantBuffer();

		AEResult CreateComputeShaderTextureBinding();

		AEResult CreateComputeShaderStructuredBuffers();

		AEResult CreateComputeShaderSimpleBuffers();

#pragma endregion

	public:

		/****************************************
		 *   Constructor & Destructor Methods   *
		 ****************************************/
#pragma region Constructor & Destructor Methods

		/// <summary>
		/// ForwardPlusLightCullingMaterial Constructor
		/// </summary>
		/// <param name="graphicDevice">Graphic Device to be associated with</param>
		/// <param name="gameResourceManager">Game Resource Manager to get the manage resources</param>
		/// <param name="name">Name of the Effect</param>
		ForwardPlusLightCullingMaterial(GraphicDevice* graphicDevice, GameResourceManager* gameResourceManager, const std::wstring& name = AE_FORWARD_PLUS_LIGHT_CULLING_MAT_NAME);

		/// <summary>
		/// Default ForwardPlusLightCullingMaterial Destructor
		/// </summary>
		virtual ~ForwardPlusLightCullingMaterial();

#pragma endregion

		/*******************
		 *   Get Methods   *
		 *******************/
#pragma region Get Methods

#pragma endregion
		
		/*******************
		 *   Set Methods   *
		 *******************/
#pragma region Set Methods

#pragma endregion

		/*************************
		 *   Framework Methods   *
		 *************************/
#pragma region Framework Methods

		AEResult LoadContent() override;

#pragma endregion

};

#endif