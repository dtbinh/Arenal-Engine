/********************************************************
*
* Author: Carlos Chac�n N.
*
* Copyright 2012-2015
*
*********************************************************/

#pragma once

#ifndef _GAME_OBJECTS_DEBUG_VISUALIZER_H
#define _GAME_OBJECTS_DEBUG_VISUALIZER_H

/**********************
*   System Includes   *
***********************/
#include <string>

/*************************
*   3rd Party Includes   *
**************************/

/***************************
*   Game Engine Includes   *
****************************/
#include "Base\Base.h"
#include "Color\Color.h"
#include "Math\AEMathDefs.h"
#include "AEGameComponentsDefs.h"
#include "GameObjectsDebugDefs.h"
#include "GameUtils\DrawableGameComponent.h"

/************
*   Using   *
*************/

/********************
*   Forward Decls   *
*********************/
class Light;
class Camera;
class GameApp;
class Frustum;
class LightGOC;
class OmniLight;
class SpotLight;
class Texture2D;
class GameObject;
class QuadShape3D;
struct TimerParams;
class FrustumShape;
class CameraUpdater;
class OmniLightShape;
class SpotLightShape;
class DirectionalLight;
class BasicLineMaterial;
class BasicColorMaterial;
class DirectionalLightShape;
class DiffuseTextureBasicMaterial;

/*****************
*   Class Decl   *
******************/
class GameObjectsDebugVisualizer sealed : public DrawableGameComponent
{
	private:

		GameObjectsDebugVisualizerConfig m_GameObjectsDebugVisualizerConfig;

		glm::mat4 m_LightIconsScale = AEMathHelpers::Mat4Identity;

		CameraUpdater* m_CameraUpdater;

		BasicLineMaterial* m_BasicLineMaterial = nullptr;

		BasicColorMaterial* m_BasicColorMaterial = nullptr;

		DiffuseTextureBasicMaterial* m_DiffuseTextureBasicMaterial = nullptr;

		QuadShape3D* m_QuadShape3D = nullptr;

		OmniLightShape* m_OmniLightShape = nullptr;

		SpotLightShape* m_SpotLightShape = nullptr;

		DirectionalLightShape* m_DirectionalLightShape = nullptr;

		Texture2D* m_OmniLightIconTex = nullptr;

		Texture2D* m_DirLightIconTex = nullptr;

		Texture2D* m_SpotLightIconTex = nullptr;

		Texture2D* m_CameraIconTex = nullptr;

		FrustumShape* m_FrustumShape = nullptr;

		AEResult SelectedGameObjectDebugRender();

		AEResult DrawFrustum(const Frustum* frustum, bool simple, const Camera* camera);

		AEResult GameObjectCameraRender(const GameObject* gameObject, const Camera* camera);

		AEResult GameObjectLightRender(const GameObject* gameObject, const Camera* camera);

		AEResult GameObjectOmniLightRender(const GameObject* gameObject, const OmniLight* omnilight, const Camera* camera, bool drawDebug, bool renderFrustum, bool frustumSimple);

		AEResult GameObjectSpotLightRender(const GameObject* gameObject, const SpotLight* spotlight, const Camera* camera, bool drawDebug, bool renderFrustum, bool frustumSimple);

		AEResult GameObjectDirectionalLightRender(const GameObject* gameObject, const DirectionalLight* dirlight, const Camera* camera, bool drawDebug, bool renderFrustum, bool frustumSimple, bool drawFustumCascades);

		AEResult LightIconDraw();

		AEResult CameraIconDraw();

	public:
		//Constructor Destructor.
		GameObjectsDebugVisualizer(GameApp* gameApp, const std::wstring& gameComponentName = AE_GAME_OBJECTS_DEBUG_VISUALIZER_DEF_COMPONENT_NAME, const std::wstring& cameraServiceName = AE_CAMERA_UPDATER_DEF_SERVICE_NAME, uint32_t callOrder = AEGameComponentCallOrder::_AE_GCCO_GameObjectsDebugVisualizer);
		virtual ~GameObjectsDebugVisualizer();

		void					Initialize					() override;
		void					LoadContent					() override;
		void					Render						(const TimerParams& timerParams) override;
};

#endif