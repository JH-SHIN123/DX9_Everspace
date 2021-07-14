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

	// 동영상 재생

	return S_OK;
}

_uint CLogo::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);

	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
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

	// FadeIn이 끝났을때 씬전환
	if (m_bLeaveScene)
	{
		if (FAILED(CManagement::Get_Instance()->Setup_CurrentScene((_uint)ESceneType::Loading,
<<<<<<< HEAD
			CLoading::Create(m_pDevice, ESceneType::Lobby))))
=======
			CLoading::Create(m_pDevice, ESceneType::Stage2))))
>>>>>>> origin/main

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
	/* 자식의 소멸자 호출 순서처럼 Free도 같은 순서로 호출해주자*/
	/* 1.자식 리소스 먼저 정리하고난 뒤 */
	m_pManagement->StopAll();

	CScene::Free(); // 2.부모 리소스 정리

	
}
