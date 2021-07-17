#pragma once

#ifndef __STAGE2_H__
#include "Scene.h"

#include "QuestHandler.h"
#include"ScriptUI.h"
USING(Engine)

class CStage2 final : public CScene
{
public:
	explicit CStage2(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CStage2() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

public:
	HRESULT Add_Layer_Player(const wstring& LayerTag);
	HRESULT Add_Layer_Cam(const wstring& LayerTag);
	HRESULT Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc);
	HRESULT Add_Layer_Skybox(const wstring& LayerTag);
	HRESULT Add_Layer_HUD(const wstring& LayerTag);
	HRESULT Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc);
	HRESULT Add_Layer_Asteroid(const wstring& LayerTag, GAMEOBJECT_DESC tDesc);
	// InGame
	HRESULT Add_Layer_ScriptUI(const wstring& LayerTag, EScript eScript);
	HRESULT Add_Layer_MissionUI(const wstring& LayerTag, EQuest eQuest);

private:
	_uint Stage2_Flow(_float fDeltaTime);
private:
	void Ready_Asteroid();
	_bool AsteroidFlyingAway(_float fDeltaTime, _float fMaxXDist, _float fMaxYDist, _float fMaxZDist,
		_float fMinZDist, CTransform* pTargetTransform, _uint iRockAmount, _float fRockSpeed,
		_float fDistFromTarget,_float fFinishTime);

public:
	static CStage2* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;
	
private:
	_bool m_bStageClear = false;

private:
	//For.FlyingAsteroid
	_float m_fFlyingAsteroidTime = 1.f;
	_bool m_bStartFlyAway = FALSE;
	_bool m_bFinishFlyAway = FALSE;
	_bool m_bPlayPlayerDeadScript = FALSE;
#define UPDATE_FLYAWAY 3
#define PLAYER_DEAD    4
#define CLEAR_FLYAWAY  5
#define UPDATE_RESQUE  6
#define CLEAR_RESQUE   7
	_float m_fDelaySceneChange = 0.f;
	_bool m_bLoadMapNavi = FALSE;
private:
	_float m_fFlowTime = 1.f;
	_uint m_iFlowCount = 0;
	
private:
	_bool m_bFPS = false;
private:
	_bool m_bFadeIn = false;
	_bool m_bSceneChange = false;

};

#define __STAGE2_H__
#endif
