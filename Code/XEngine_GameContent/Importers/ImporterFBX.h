
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

#pragma once

#ifndef _IMPORTER_FBX_H
#define _IMPORTER_FBX_H

/**********************
*   System Includes   *
***********************/
#include <map>

/*************************
*   3rd Party Includes   *
**************************/
#include "fbxsdk.h"
#include "glm\glm.hpp"

/***************************
*   Game Engine Includes   *
****************************/
#include "Base\Base.h"
#include "ImportersDefs.h"
#include "Base\XEObject.h"
#include "Math\XEMathDefs.h"
#include "Vertex\VertexDefs.h"
#include "Content\ContentDefs.h"

/********************
*   Forward Decls   *
*********************/
class Bone;
struct TreeBone;
struct ModelContent;
struct AnimationContent;

/*****************
*   Class Decl   *
******************/

/// <summary>
/// Imports an FBX file to memory
/// </summary>
class ImporterFBX sealed : public XEObject
{

	private:

		/************************
		*   Private Variables   *
		*************************/
#pragma region Private Variables

		/// <summary>
		/// FBX SDK Manager
		/// </summary>
		FbxManager* m_FBXSdkManager = nullptr;

		/// <summary>
		/// FBX Scene 
		/// </summary>
		FbxScene* m_Scene = nullptr;

		bool m_Clockwise = false;

		bool m_ImportTangentBinormal = false;

		bool m_BurnTransformation = false;

		bool m_GenerateTangenteBinormal = false;

		bool m_HasSecondUV = false;

		bool m_HasUV = false;

		bool m_HasNormal = false;

		bool m_HasColor = false;

		bool m_HasAnimation = false;

		uint32_t m_FramesPerSecond = 0;

		float m_SecondsInFrame = 0.0f;

		int32_t m_StartFrame = 0;

		int32_t m_EndFrame = 0;

		ModelContent* m_Model = nullptr;

		std::map<int32_t, uint64_t> m_MatIdxToMatIDMap;

		std::map<uint64_t, MaterialHolder> m_MaterialHolderMap;

		std::map<uint64_t, Bone*> m_BoneMap;

#pragma endregion

		/**********************
		*   Private Methods   *
		***********************/
#pragma region Private Methods

		inline void GetVec3FromFBXVec4(const FbxVector4& fbxVec4, glm::vec3& vec)
		{
			vec.x = (float)fbxVec4[0];
			vec.y = (float)fbxVec4[2];
			vec.z = (float)fbxVec4[1];
		}

		inline void GetVec4FromFBXVec4(const FbxVector4& fbxVec4, glm::vec4& vec)
		{
			vec.x = (float)fbxVec4[0];
			vec.y = (float)fbxVec4[2];
			vec.z = (float)fbxVec4[1];
			vec.w = (float)fbxVec4[3];
		}

		/// <summary>
		/// Initializes the FBX SDK Objects
		/// </summary>
		/// <returns>Return XEResult::Ok if Initialize succeeded</returns>
		XEResult InitializeFBXSDK();

		/// <summary>
		/// Loads the File FBX Scene to Memory
		/// </summary>
		/// <returns>Returns XEResult::Ok if Scene was loaded successfully<seealso cref="XEResult"/></returns>
		XEResult LoadScene(const std::wstring& filename);

		XEResult BuildSkeletonHierarchy();

		XEResult BuildSkeletonHierarchy(FbxNode* node, const glm::mat4& parentTransform = XEMathHelpers::Mat4Identity, TreeBone* childBone = nullptr);

		XEResult ExtractContent();

		XEResult ExtractContent(FbxNode* node, const glm::mat4& parentTransform = XEMathHelpers::Mat4Identity);

		XEResult ExtractMesh(FbxNode* node, MeshHolder& meshHolder);

		XEResult GetNodeLocalTransform(FbxNode* node, glm::mat4& localTransform);

		RotationOrder GetRotationOrderFromFBX(FbxEuler::EOrder fbxRotationOrder);

		XEResult GetMeshVertices(FbxMesh* fbxMesh, MeshHolder& meshHolder);

		XEResult GetPolygonUV(FbxMesh* fbxMesh, int32_t controlPointIndex, int32_t polygonIndex, int32_t positionInPolygon, glm::vec2& uv, glm::vec2& uv2);

		XEResult GetPolygonNormal(FbxMesh* fbxMesh, int32_t controlPointIndex, int32_t vertexId, glm::vec3& normal);

		XEResult GetPolygonTangent(FbxMesh* fbxMesh, int32_t controlPointIndex, int32_t vertexId, glm::vec4& tangent);

		XEResult GetPolygonBinormal(FbxMesh* fbxMesh, int32_t controlPointIndex, int32_t vertexId, glm::vec4& binormal);

		XEResult GetMeshMaterialMapping(FbxMesh* fbxMesh, MeshHolder& meshHolder);

		XEResult GetMaterialProperties(FbxGeometry* fbxGeometry);

		XEResult GetMaterialTextures(FbxGeometry* fbxGeometry);

		XEResult GetMeshBlendIndicesWeight(FbxGeometry* fbxGeometry, MeshHolder& meshHolder);

		XEResult CreateMeshParts(MeshHolder& meshHolder);

		/// <summary>
		/// Returns Vertex Type of Current Mesh
		/// </summary>
		/// <returns>Returns Vertex Type<seealso cref="VertexType"/></returns>
		VertexType GetVertexType();

		XEResult ExtractAnimation();

		XEResult ExtractAnimation(FbxAnimStack* animStack, FbxNode* node);

		XEResult ExtractAnimation(AnimationContent& animClip, FbxAnimLayer* animLayer, FbxNode* node);

		XEResult ExtractAnimationChannels(AnimationContent& animClip, FbxNode* node, FbxAnimLayer* animLayer);

		XEResult ExtractAnimationCurve(AnimationContent& animClip, FbxAnimCurve* animCurve[], int32_t boneIndex, const glm::quat& preRotQuad, RotationOrder rotationOrder);

		void CleanUp();

#pragma endregion

	public:

		/***************************************
		*   Constructor & Destructor Methods   *
		****************************************/
#pragma region Constructor & Destructor Methods

		/// <summary>
		/// ImporterFBX Constructor
		/// </summary>
		ImporterFBX();

		/// <summary>
		/// Default ImporterFBX Destructor
		/// </summary>
		virtual ~ImporterFBX();

		/******************
		*   Get Methods   *
		*******************/
#pragma region Get Methods

#pragma endregion

#pragma endregion

		/************************
		*   Framework Methods   *
		*************************/
#pragma region Framework Methods

		/// <param name="clockwise">How to import a Model for cull mode</param>
		XEResult ImportFBXFile(const std::wstring& filename, ModelContent** model, bool burnTransformation = true, bool importTangentBinormal = true, bool clockwise = true);

#pragma endregion

};

#endif