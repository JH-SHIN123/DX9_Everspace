#pragma once

#ifndef __STAGE3_H__
#include "Scene.h"

#include "QuestHandler.h"
#include "ScriptUI.h"

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

public:
	HRESULT Add_Layer_Player(const wstring& LayerTag);
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

};

#define __STAGE3_H__
#endif
