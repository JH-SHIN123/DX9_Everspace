#include "stdafx.h"
#include "..\Headers\Logo.h"
#include "Loading.h"
#include "MainCam.h"

CLogo::CLogo(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CLogo::Ready_Scene()
{
	CScene::Ready_Scene();

	::SetWindowText(g_hWnd, L"Logo");

	m_pManagement->Create_MCIVideoEx(g_hWnd, L"../../Resources/Video/logo.wmv", WINCX, WINCY);
	m_pManagement->Play_MCIVideoEx();
	
	return S_OK;
}

_uint CLogo::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);

	if (false == m_pManagement->IsPlaying_MCIVideoEx())
	{
		if (false == m_bFadeIn) {
			if (FAILED(m_pManagement->Add_GameObject_InLayer(
				EResourceType::Static,
				L"GameObject_FadeIn",
				L"Layer_Fade",
				this)))
			{
				PRINT_LOG(L"Error", L"Failed To Add Boss_Monster In Layer");
				return E_FAIL;
			}

			m_bFadeIn = true;
			return NO_EVENT;
		}
	}

	//FadeIn이 끝났을때 씬전환 
	if (m_bLeaveScene)
	{
		if (FAILED(CManagement::Get_Instance()->Setup_CurrentScene((_uint)ESceneType::Loading,
			CLoading::Create(m_pDevice, ESceneType::Lobby))))
		{
			PRINT_LOG(L"Error", L"Failed To Setup Stage Scene");
			return E_FAIL;
		}

		
		m_bLeaveScene = false;
		return CHANGE_SCENE;
	}

	return _uint();
}

_uint CLogo::LateUpdate_Scene(_float fDeltaTime)
{
	CScene::LateUpdate_Scene(fDeltaTime);

	return _uint();
}

CLogo * CLogo::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice)
	{
		PRINT_LOG(L"Error", L"Device is nullptr");
		return nullptr;
	}

	CLogo* pInstance = new CLogo(pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo::Free()
{
	m_pManagement->StopAll();
	m_pManagement->Release_MCIVideoEx();
	CScene::Free(); // 2.부모 리소스 정리
}
