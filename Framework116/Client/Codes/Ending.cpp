#include "stdafx.h"
#include "Ending.h"
#include "Loading.h"

CEnding::CEnding(LPDIRECT3DDEVICE9 pDevice, const ESceneType eSceneType)
	: CScene(pDevice)
	, m_eSceneType(eSceneType)
{
}

HRESULT CEnding::Ready_Scene()
{
	CScene::Ready_Scene();

	::SetWindowText(g_hWnd, L"Logo");
	//m_pManagement->StopSound(CSoundMgr::BGM);
	m_pManagement->StopAll();
	m_pManagement->Clear_NonStatic_Resources();

	const TCHAR* videoPath = nullptr;
	switch (m_eSceneType)
	{
	case ESceneType::Stage:
		videoPath = L"../../Resources/Video/ending_stage1.wmv";
		m_pAudioFileName = L"Ending_Video_Stage1.ogg";
		break;
	case ESceneType::Stage2:
		videoPath = L"../../Resources/Video/ending_stage2.wmv";
		m_pAudioFileName = L"Ending_Video_Stage2.ogg";
		break;
	case ESceneType::Stage3:
		videoPath = L"../../Resources/Video/ending_stage3.wmv";
		m_pAudioFileName = L"Ending_Video_Stage3.ogg";
		break;
	default:
		PRINT_LOG(L"Error", L"Ending Error");
		break;
	} 

	m_pManagement->Create_MCIVideoEx(g_hWnd, videoPath, WINCX, WINCY);

	return S_OK;
}

_uint CEnding::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);
	if (m_bStart)
	{
		m_pManagement->Play_MCIVideoEx();
		m_pManagement->PlaySound(m_pAudioFileName, CSoundMgr::LOADING);
		m_bStart = false;
	}

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

_uint CEnding::LateUpdate_Scene(_float fDeltaTime)
{
	CScene::LateUpdate_Scene(fDeltaTime);

	return _uint();
}

CEnding* CEnding::Create(LPDIRECT3DDEVICE9 pDevice, const ESceneType eSceneType)
{
	if (nullptr == pDevice)
	{
		PRINT_LOG(L"Error", L"Device is nullptr");
		return nullptr;
	}

	CEnding* pInstance = new CEnding(pDevice, eSceneType);
	if (FAILED(pInstance->Ready_Scene()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnding::Free()
{
	m_pManagement->StopAll();
	m_pManagement->Release_MCIVideoEx();
	CScene::Free(); // 2.부모 리소스 정리
}
