#pragma once

#ifndef __STAGE3_H__
#include "Scene.h"

#include "QuestHandler.h"
#include "ScriptUI.h"

#define QUEST_FAILED 666
#define PLAYER_STAGE3_DEAD 888

USING(Engine)

class CStage3 final : public CScene
{
public:
	explicit CStage3(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CStage3() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

private:
	void Stage_Flow(_float fDeltaTime);
	void All_Monster_Boom(_float fDeltaTime);

public:
	HRESULT Add_Layer_Cam(const wstring& LayerTag);

	HRESULT Add_Layer_ExplosionSystem(const wstring& LayerTag, const PARTICLESYSTEM_DESC* pParticleSystemDesc);
	HRESULT Add_Layer_LaserSystem(const wstring& LayerTag, const PARTICLESYSTEM_DESC* pParticleSystemDesc);

	// hud
	HRESULT Add_Layer_HUD(const wstring& LayerTag);

	HRESULT Add_Layer_Monster(const wstring& LayerTag);
	HRESULT Add_Layer_Skybox(const wstring& LayerTag);
	HRESULT Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc);
	HRESULT Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc);
	HRESULT Add_Layer_Boss_Monster(const wstring& LayerTag);
	HRESULT Add_Layer_ScriptUI(const wstring& LayerTag, EScript eScript);
	HRESULT Add_Layer_MissionUI(const wstring& LayerTag, EQuest eQuest);
	HRESULT Add_Layer_Sniper(const wstring& LayerTag);

public:
	static CStage3* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;

private:
	_float m_fFlowTime = 1.f;
	_uint m_iFlowCount = 0;

private:
	_bool m_bFadeIn = false;
	_bool m_bSceneChange = false;

private:
	_bool m_IsAllMonsterBoom = false;
	_bool m_IsAllBoom = false;
	_bool m_IsGameOver = false;
	_float m_fBoomTime = 0.f;
	//list<class CGameObject*> m_listObjectList;
};

#define __STAGE3_H__
#endif
