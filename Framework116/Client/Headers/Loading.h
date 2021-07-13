#pragma once
#ifndef __LOADING_H__

#include "Scene.h"

USING(Engine)
class CLoading final : public CScene
{
public:
	explicit CLoading(LPDIRECT3DDEVICE9 pDevice, ESceneType eNextSceneID);
	virtual ~CLoading() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pDevice, ESceneType eNextSceneID);
	virtual void Free() override;

public: /* For.Loading Thread */
	static unsigned __stdcall ThreadMain(void* pArg);
	HRESULT Ready_StageResources();
	HRESULT Ready_Stage2Resources();
	HRESULT Ready_Stage3Resources();

	HRESULT Ready_LobbyResources();
	HRESULT Ready_StageEffect();
	HRESULT Ready_BossAndOthers();
	HRESULT Ready_HUD_Resources();
	HRESULT Ready_ScriptUI_Resources();
	HRESULT Ready_Stage1();
	HRESULT Ready_Map_Effect_Resources();
	
private:
	ESceneType m_eNextSceneID = ESceneType::None;

	HANDLE m_hLoadingThread = nullptr;
	CRITICAL_SECTION m_CriticalSection;
	_bool m_IsFinished = false;
};

#define __LOADING_H__
#endif
