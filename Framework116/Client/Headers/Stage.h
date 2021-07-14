#pragma once
#ifndef __STAGE_H__

#include "Scene.h"

#include "QuestHandler.h"
#include "ScriptUI.h"

USING(Engine)
class CStage final : public CScene
{
public:
	explicit CStage(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CStage() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

private:
	_uint Stage_Flow(_float fDeltaTime);

private:
	HRESULT Add_Layer_Player(const wstring& LayerTag);
	HRESULT Add_Layer_Terrain(const wstring& LayerTag);
	HRESULT Add_Layer_Cam(const wstring& LayerTag);
	HRESULT Add_Layer_Monster(const wstring& LayerTag);
	HRESULT Add_Layer_Skybox(const wstring& LayerTag);
	HRESULT Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc);
	HRESULT Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc);
	HRESULT Add_Layer_ExplosionSystem(const wstring& LayerTag, const PARTICLESYSTEM_DESC* pParticleSystemDesc);
	HRESULT Add_Layer_LaserSystem(const wstring& LayerTag, const PARTICLESYSTEM_DESC* pParticleSystemDesc);

	HRESULT Add_Layer_Ring(const wstring& LayerTag);
	HRESULT Add_Layer_TargetMonster(const wstring& LayerTag);

	// HUD
	HRESULT Add_Layer_HUD(const wstring& LayerTag);

	// InGame
	HRESULT Add_Layer_ScriptUI(const wstring& LayerTag, EScript eScript);
	HRESULT Add_Layer_MissionUI(const wstring& LayerTag, EQuest eQuest);

	//Sniper
	HRESULT Add_Layer_Sniper(const wstring& LayerTag);

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;

private:
	_float m_fFlowTime = 1.f;
	_uint m_iFlowCount = 0;

private:
	_bool m_bFadeIn = false;
};

#define __STAGE_H__
#endif
