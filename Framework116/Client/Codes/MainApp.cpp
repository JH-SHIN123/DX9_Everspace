#include "stdafx.h"
#include "..\Headers\MainApp.h"
#include "Logo.h"
#include "Player.h"
#include "MainCam.h"

CMainApp::CMainApp()
	: m_pManagement(CManagement::Get_Instance())
{
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(m_pManagement->Ready_Game(g_hWnd, WINCX, WINCY, EDisplayMode::Win)))
	{
		PRINT_LOG(L"Error", L"Failed To Ready Game");
		return E_FAIL;
	}

	m_pDevice = m_pManagement->Get_Device();
	Safe_AddRef(m_pDevice);
	if (nullptr == m_pDevice)
	{
		PRINT_LOG(L"Error", L"m_pDevice is nullptr");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Setup_CurrentScene((_uint)ESceneType::Logo, CLogo::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Setup Logo Scene");
		return E_FAIL;
	}

	if (FAILED(Ready_StaticResources()))
	{
		PRINT_LOG(L"Error", L"Failed To Ready Static Resources");
		return E_FAIL;
	}	

	if (FAILED(Setup_DefaultSetting()))
	{
		PRINT_LOG(L"Error", L"Failed To Setup Default Setting");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMainApp::Update_MainApp()
{
	m_pManagement->Update_Game();

	return _uint();
}

HRESULT CMainApp::Ready_StaticResources()
{
#pragma region GameObjects
	/* For.GameObject_Player */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Player",
		CPlayer::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Player");
		return E_FAIL;
	}

	/* For.GameObject_MainCam */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_MainCam",
		CMainCam::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_MainCam");
		return E_FAIL;
	}
#pragma endregion

#pragma region Components
	/* For.Component_VIBuffer_TriColor */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_TriColor",
		CVIBuffer_TriColor::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_TriColor");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_RectColor */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_RectColor",
		CVIBuffer_RectColor::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_RectColor");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_RectTexture */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture",
		CVIBuffer_RectTexture::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_RectTexture");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_CubeTexture */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_CubeTexture",
		CVIBuffer_CubeTexture::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_CubeTexture");
		return E_FAIL;
	}

	/* For.Component_Transform */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Transform",
		CTransform::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Transform");
		return E_FAIL;
	}

	/* For.Component_Texture_Player */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Texture_Player",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../Resources/Player%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Player");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Setup_DefaultSetting()
{
	// 조명 off
	if (FAILED(m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE)))
	{
		PRINT_LOG(L"Error", L"Failed To Set Lighting false");
		return E_FAIL;
	}
	
	///*
	//D3DFILL_WIREFRAME: 색을 채우지말고 외곽선만 그려라.
	//D3DFILL_SOLID: 색을 채워넣어라(디폴트)
	//*/
	//if (FAILED(m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME)))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Set wireframe");
	//	return E_FAIL;
	//}

	/*
	D3DCULL_CCW(Counter Clock Wise): 반시계 방향으로 구성한 폴리곤을 추려낸다. (디폴트값)
	D3DCULL_CW(Clock Wise): 시계 방향으로 구성한 폴리곤을 추려낸다.
	D3DCULL_NONE: 후면추려내기 안함.
	*/
	if (FAILED(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE)))
	{
		PRINT_LOG(L"Error", L"Failed To Set D3DRS_CULLMODE");
		return E_FAIL;
	}	

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp;
	if (FAILED(pInstance->Ready_MainApp()))
	{
		PRINT_LOG(L"Error", L"Failed To Ready_MainApp");
		delete pInstance;
		return nullptr;
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pDevice);

	if (Safe_Release(m_pManagement))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Management");
	}
}
