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
	HRESULT Ready_LobbyResources();
	HRESULT Ready_StageResources();
	HRESULT Ready_Stage2Resources();
	HRESULT Ready_Stage3Resources();

public: /* For.Loading UI Resource */
	HRESULT Load_HUD_Resources();
	HRESULT Load_ScriptUI_Resources();

public: /* For Loading Monster Resource */
	HRESULT Load_Stage1_Prop_Resources();
	HRESULT Load_Stage2_Prop_Resources();
	HRESULT Load_Stage3_Prop_Resources();

public: /* For Loading Effect Resource */
	HRESULT Load_StageEffect_Resources();

private:
	HRESULT Ready_LoadingResources();

private:
	_bool m_bStart = true;
	_bool m_IsFinished = false;
	_bool m_bFadeIn = false;

private:
	ESceneType m_eNextSceneID = ESceneType::None;

	HANDLE m_hLoadingThread = nullptr;
	CRITICAL_SECTION m_CriticalSection;

private:
	//HWND	m_hVideo;
	CGameObject* m_pLoadingUI = nullptr;
};

#define __LOADING_H__
#endif
