/********************************************************
*
* Author: Carlos Chac�n N.
*
* Created: 2/14/2012
*
* Desc:
*
*
* Copyright 2012
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
#include "GameComponent.h"
#include "GameApp\GameApp.h"
#include "Time\XETimeDefs.h"
#include "Base\BaseFunctions.h"
#include "Resource\GameResourceManager.h"

//Always include last
#include "Memory\MemLeaks.h"
/********************
*   Function Defs   *
*********************/
XETODO("add mutex");
GameComponent::GameComponent(GameApp* gameApp, const std::wstring& name, uint32_t callOrder)
	: GameService(name)
	, m_GameApp(gameApp)
	, m_CallOrder(callOrder)
{
	XEAssert(gameApp != nullptr);

	m_GameResourceManager = gameApp->GetGameResourceManager();
}

GameComponent::~GameComponent()
{
	UnLoadContent();
}

void GameComponent::SetCallOrder(uint32_t callOrder)
{
	if(m_CallOrder == callOrder)
	{
		return;
	}

	m_CallOrder = callOrder;

	if(m_NeedSortChangeCallback != nullptr)
	{
		m_NeedSortChangeCallback();
	}
}

void GameComponent::ConstantUpdate(const TimerParams& timerParams)
{
}

void GameComponent::Update(const TimerParams& timerParams)
{
}

void GameComponent::PostUpdate(const TimerParams& timerParams)
{
}

void GameComponent::Initialize()
{
}

void GameComponent::LoadContent()
{
}

void GameComponent::UnLoadContent()
{
}

void GameComponent::OnLostDevice()
{
}

void GameComponent::OnResetDevice()
{
}