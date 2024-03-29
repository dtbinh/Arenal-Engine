/********************************************************
*
* Author: Carlos Chac�n N.
*
* Copyright 2012-2015
*
*********************************************************/

#pragma once

#ifndef _AE_XML_PARSER_H
#define _AE_XML_PARSER_H

/**********************
*   System Includes   *
***********************/
#include <string>
#include <stdint.h>

/*************************
*   3rd Party Includes   *
**************************/
#include "glm\glm.hpp"
#include "libxml\parser.h"

/***************************
*   Game Engine Includes   *
****************************/
#include "Base\Base.h"
#include "Base\AEObject.h"
#include "Math\AEMathDefs.h"

/************
*   Using   *
*************/

/********************
*   Forward Decls   *
*********************/

/*****************
*   Class Decl   *
******************/
class AEXMLParser sealed : public AEObject
{
	private:
		bool			m_IsReady = false;
		bool			m_ParentNode = false;
		xmlDocPtr		m_LibXMLDoc = nullptr;
		xmlNodePtr		m_LibXMLNode = nullptr;

		//Private Methods
		void			CleanUp				();
		AEResult		FindNode			(const xmlChar* nodeNameMB, xmlNodePtr xmlLibNode, AEXMLParser& node);
		xmlChar*		GetNodeProperty		(const std::wstring& propName);

	public:

		/***************************************
		*   Constructor & Destructor Methods   *
		****************************************/
#pragma region Constructor & Destructor Methods

		/// <summary>
		/// AEXMLParser Constructor
		/// </summary>
		AEXMLParser();

		/// <summary>
		/// Default AEXMLParser Destructor
		/// </summary>
		virtual ~AEXMLParser();

#pragma endregion

		//Gets
		bool IsReady() const
		{
			return m_IsReady;
		}

		//Framework Methods
		AEResult		LoadFile			(const std::wstring& file);
		AEResult		GetNode				(const std::wstring& nodeName, AEXMLParser& node);
		AEResult		GetChildNode		(uint32_t index, AEXMLParser& node);
		uint32_t		GetNumChildren		();
		std::wstring	GetName				();
		int8_t			GetInt8				(const std::wstring& propName, int8_t defaultValue = 0, bool warning = true);
		uint8_t			GetUInt8			(const std::wstring& propName, uint8_t defaultValue = 0, bool warning = true);
		int16_t			GetInt16			(const std::wstring& propName, int16_t defaultValue = 0, bool warning = true);
		uint16_t		GetUInt16			(const std::wstring& propName, uint16_t defaultValue = 0, bool warning = true);
		int32_t			GetInt				(const std::wstring& propName, int32_t defaultValue = 0, bool warning = true);
		uint32_t		GetUInt				(const std::wstring& propName, uint32_t defaultValue = 0, bool warning = true);
		int64_t			GetInt64			(const std::wstring& propName, int64_t defaultValue = 0, bool warning = true);
		uint64_t		GetUInt64			(const std::wstring& propName, uint64_t defaultValue = 0, bool warning = true);
		float			GetFloat			(const std::wstring& propName, float defaultValue = 0.0f, bool warning = true);
		double			GetDouble			(const std::wstring& propName, double defaultValue = 0.0, bool warning = true);
		bool			GetBool				(const std::wstring& propName, bool defaultValue = false, bool warning = true);
		std::wstring	GetString			(const std::wstring& propName, const std::wstring& defaultValue = L"", bool warning = true);
		glm::vec2		GetVect2f			(const std::wstring& propName, const glm::vec2& defaultValue = AEMathHelpers::Vec2fZero, bool warning = true);
		glm::vec3		GetVect3f			(const std::wstring& propName, const glm::vec3& defaultValue = AEMathHelpers::Vec3fZero, bool warning = true);
		glm::vec4		GetVect4f			(const std::wstring& propName, const glm::vec4& defaultValue = AEMathHelpers::Vec4fZero, bool warning = true);
		glm::dvec2		GetVect2d			(const std::wstring& propName, const glm::dvec2& defaultValue = AEMathHelpers::Vec2dZero, bool warning = true);
		glm::dvec3		GetVect3d			(const std::wstring& propName, const glm::dvec3& defaultValue = AEMathHelpers::Vec3dZero, bool warning = true);
		glm::dvec4		GetVect4d			(const std::wstring& propName, const glm::dvec4& defaultValue = AEMathHelpers::Vec4dZero, bool warning = true);
		glm::bvec2		GetVect2b			(const std::wstring& propName, const glm::bvec2& defaultValue = AEMathHelpers::Vec2bZero, bool warning = true);
		glm::bvec3		GetVect3b			(const std::wstring& propName, const glm::bvec3& defaultValue = AEMathHelpers::Vec3bZero, bool warning = true);
		glm::bvec4		GetVect4b			(const std::wstring& propName, const glm::bvec4& defaultValue = AEMathHelpers::Vec4bZero, bool warning = true);
		glm::ivec2		GetVect2i			(const std::wstring& propName, const glm::ivec2& defaultValue = AEMathHelpers::Vec2iZero, bool warning = true);
		glm::ivec3		GetVect3i			(const std::wstring& propName, const glm::ivec3& defaultValue = AEMathHelpers::Vec3iZero, bool warning = true);
		glm::ivec4		GetVect4i			(const std::wstring& propName, const glm::ivec4& defaultValue = AEMathHelpers::Vec4iZero, bool warning = true);
		glm::mat2		GetMat2f			(const std::wstring& propName, const glm::mat2& defaultValue = AEMathHelpers::Mat2Identity, bool warning = true);
		glm::mat3		GetMat3f			(const std::wstring& propName, const glm::mat3& defaultValue = AEMathHelpers::Mat3Identity, bool warning = true);
		glm::mat4		GetMat4f			(const std::wstring& propName, const glm::mat4& defaultValue = AEMathHelpers::Mat4Identity, bool warning = true);

		//Operator Overload
		AEXMLParser operator[] (const std::wstring& nodeName);

		AEXMLParser operator() (uint32_t index);
};

#endif