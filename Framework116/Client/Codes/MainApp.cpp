#include "stdafx.h"
#include "..\Headers\MainApp.h"
#include "Logo.h"
#include "Player.h"
#include "MainCam.h"
#include "UI.h"
#include "Player_Bullet.h"
#include "Player_Missile.h"
#include "Light.h"
#include "AlertArrow.h"
#include "ScriptUI.h"
#include "HP_Bar.h"
#include "Loading.h"
#include "Shield_Battery.h"
#include "HUD_Effect_Damage.h"
#include "HUD_Effect_Boost.h"
#include "QuestHandler.h"
#include "Status_Info.h"
#include "FadeIn.h"
#include "FadeOut.h"

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
	/* For.GameObject
	*/
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Player",
		CPlayer::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_MainCam");
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

	/* For.GameObject_UI */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_UI",
		CUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_UI");
		return E_FAIL;
	}

	/* For.GameObject_Font */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_MyFont",
		CMyFont::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_MyFont");
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

	/* For.GameObject_AlertArrow */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Shield_Battery",
		CShield_Battery::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Shield_Battery");
		return E_FAIL;
	}

	/* For.GameObject_HUD_Effect_Damage */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_HUD_Effect_Damage",
		CHUD_Effect_Damage::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_UI");
		return E_FAIL;
	}

	/* For.GameObject_HUD_Effect_Damage */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_HUD_Effect_Boost",
		CHUD_Effect_Boost::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_UI");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_FadeIn",
		CFadeIn::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_FadeIn");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_FadeOut",
		CFadeOut::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_FadeOut");
		return E_FAIL;
	}

#pragma endregion

#pragma region Components
	/* For.Component_VIBuffer_RectTexture */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture",
		CVIBuffer_RectTexture::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_RectTexture");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_RectTexture_HP_Bar */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture_HP_Bar",
		CVIBuffer_RectTexture_HP_Bar::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_RectTexture_HP_Bar");
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
		CModelMesh::Create(m_pDevice,L"../../Resources/Models/ship.X", L"../../Resources/Textures/PlayerShip/"))))
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

	// m_tInfo
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Status_Info",
		CStatus_Info::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Status_Info");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Texture_FadeOut",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/FadeOut.png", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_AlertArrow");
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
	CQuestHandler::Get_Instance()->Release_Ref();
	
	Safe_Release(m_pDevice);
	
	if (Safe_Release(m_pManagement))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Management");
	}
}
