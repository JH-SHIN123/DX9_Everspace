#include "stdafx.h"
#include "..\Headers\MainApp.h"
#include "Logo.h"
#include "Player.h"
#include "MainCam.h"
#include "UI.h"
#include "Player_Bullet.h"
#include "Player_Lazer.h"
#include "Player_Missile.h"
#include "Light.h"
#include "AlertArrow.h"

CMainApp::CMainApp()
	: m_pManagement(CManagement::Get_Instance())
{
}

HRESULT CMainApp::Ready_MainApp()
{
	if (FAILED(m_pManagement->Ready_Game(g_hWnd, WINCX, WINCY, EDisplayMode::Win, 120.f)))
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

	if (FAILED(m_pManagement->Setup_CurrentScene((_uint)ESceneType::Logo, CLogo::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Setup Logo Scene");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMainApp::Update_MainApp()
{
	if (m_pManagement->FrameLock())
	{
		m_pManagement->ShowFrame(g_hWnd);
		m_pManagement->Update_Game();
	}

	return _uint();
}

HRESULT CMainApp::Ready_StaticResources()
{
#pragma region GameObjects
	/* For.GameObject_Player */
	CStreamHandler::Load_PassData_Object(
		L"../../Resources/PrototypeData/StaticPlayer.object"
		, EResourceType::Static);

	/* For.GameObject_MainCam */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_MainCam",
		CMainCam::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_MainCam");
		return E_FAIL;
	}

	/* For.GameObject_UI */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_UI",
		CUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_UI");
		return E_FAIL;
	}

	/* For.GameObject_Light */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Light",
		CLight::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Light");
		return E_FAIL;
	}

	/* For.GameObject_Player_Bullet */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Player_Bullet",
		CPlayer_Bullet::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Player_Bullet");
		return E_FAIL;
	}

	/* For.GameObject_Player_Lazer */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Player_Lazer",
		CPlayer_Lazer::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Player_Lazer");
		return E_FAIL;
	}

	/* For.GameObject_Player_Missile */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Player_Missile",
		CPlayer_Missile::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Player_Missile");
		return E_FAIL;
	}

	/* For.GameObject_AlertArrow */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_AlertArrow",
		CAlertArrow::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_AlertArrow");
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

	/* For.Component_Mesh_BigShip */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_BigShip",
		CModelMesh::Create(m_pDevice,L"../../Resources/Models/ship.X", L"../../Resources/Textures/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	/* For.Component_Mesh_Axis */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Axis",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/axis.X", L"../../Resources/Textures/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_Axis");
		return E_FAIL;
	}

	/* For.Component_CollideSphere */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_CollideSphere",
		CCollideSphere::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_CollideSphere");
		return E_FAIL;
	}

	/* For.Component_Controller */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Controller",
		CController::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Controller");
		return E_FAIL;
	}

	/* For.Component_Texture_Player_Bullet */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Texture_Player_Bullet",
		CTexture::Create(m_pDevice, ETextureType::Cube, L"../../Resources/Textures/Player_Bullet%d.dds", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Bullet");
		return E_FAIL;
	}

	/* For.Component_GeoMesh_Player_Lazer */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_GeoMesh_Player_Lazer",
		CGeoMesh_Cylinder::Create(m_pDevice, 0.1f, 0.1f, 0.1f))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_GeoMesh_Player_Lazer");
		return E_FAIL;
	}

	/* For.Component_GeoMesh_Player_Missile */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_GeoMesh_Player_Missile",
		CGeoMesh_Cylinder::Create(m_pDevice, 3.f, 2.f, 15.f))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_GeoMesh_Player_Lazer");
		return E_FAIL;
	}

	/* For.Component_Texture_AlertArrow */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Texture_AlertArrow",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/AlertArrow%d.png", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_AlertArrow");
		return E_FAIL;
	}

	//if (FAILED(m_pManagement->Add_Component_Prototype(
	//	EResourceType::Static,
	//	L"Component_Texture_Player_Lazer",
	//	CTexture::Create(m_pDevice, ETextureType::Cube, L"../../Resources/Textures/Player_Lazer%d.dds", 1))))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Bullet");
	//	return E_FAIL;
	//}

	//For. Static Texture Component Prototype
	if (FAILED(CStreamHandler::Load_PassData_Resource(L"../../Resources/Data/Static.txt", TRUE)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Controller");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CMainApp::Setup_DefaultSetting()
{
	//
	// Set lighting related render states.
	//
	m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, false);

	//if (FAILED(m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE)))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Set Lighting false");
	//	return E_FAIL;
	//}
	
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
	if (FAILED(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW)))
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
