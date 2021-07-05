#include "stdafx.h"
#include "..\Headers\Lobby.h"
#include "Camera.h"
#include"Player.h"
#include"Loading.h"
#include"LobbyUI.h"
#include"LobbyModel.h"
#include"LobbyCam.h""
CLobby::CLobby(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CLobby::Ready_Scene()
{
	::SetWindowText(g_hWnd, L"Lobby");

		CScene::Ready_Scene();
	if (FAILED(Add_Layer_Lobby_Model(L"Layer_Lobby_Model")))
		return E_FAIL;

	if (FAILED(Add_Layer_LobbyCam(L"Layer_Cam")))
		return E_FAIL;

	if (FAILED(Add_Layer_Skybox(L"Layer_Skybox")))
		return E_FAIL;

	if (FAILED(Add_Layer_UI(L"Layer_Ui")))
		return E_FAIL;
	if (FAILED(Add_Layer_GatchaBox(L"Layer_GatchaBox")))
		return E_FAIL;

	LIGHT_DESC lightDesc;
	lightDesc.eLightType = ELightType::Directional;
	lightDesc.vLightDir = { 1.0f, -0.0f, 0.25f };
	lightDesc.tLightColor = D3DCOLOR_XRGB(255, 255, 255);
	lightDesc.iLightIndex = 0;
	if (FAILED(Add_Layer_Light(L"Layer_DirectionalLight", &lightDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CLobby::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);

	
	return _uint();
}

_uint CLobby::LateUpdate_Scene(_float fDeltaTime)
{
	CScene::LateUpdate_Scene(fDeltaTime);
	if (m_bGotoNextScene && !m_bIsGatcha)
	{
		for (auto& pDst : *m_pManagement->Get_GameObjectList(L"Layer_Lobby_Model"))
		{
			static_cast<CLobbyModel*>(pDst)->Set_GotoNextScene(TRUE);
		}
		for (auto& pDst : *m_pManagement->Get_GameObjectList(L"Layer_UI"))
		{
			static_cast<CLobbyUI*>(pDst)->Set_GotoNextScene(TRUE);
		}
		for (auto& pCam : *m_pManagement->Get_GameObjectList(L"Layer_Cam"))
		{
			static_cast<CLobbyCam*>(pCam)->Set_GotoNextScene(TRUE);
		}
	}
	return _uint();
}

HRESULT CLobby::Add_Layer_Light(const wstring & LayerTag, const LIGHT_DESC * pLightDesc)
{

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_DirectionalLight",
		LayerTag,
		(void*)pLightDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Directional Light In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLobby::Add_Layer_Lobby_Model(const wstring & LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Lobby_Model",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Model In Layer");
		return E_FAIL;
	}
}

HRESULT CLobby::Add_Layer_LobbyCam(const wstring & LayerTag)
{
	CAMERA_DESC CameraDesc;
	CameraDesc.vEye = _float3(7.5f, 0.f, 7.5f);
	CameraDesc.fFovY = D3DXToRadian(90.f);
	CameraDesc.fAspect = (_float)WINCX / WINCY;
	CameraDesc.fNear = 1.f;
	CameraDesc.fFar = 1000.f;

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_LobbyCam",
		LayerTag,
		&CameraDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Player In Main Cam");
		return E_FAIL;
	}
	CLobbyCam* pLobbyCam = (CLobbyCam*)(m_pManagement->Get_GameObject(LayerTag));
	pLobbyCam->Set_Scene(this);
	AddRef();
	return S_OK;
}


HRESULT CLobby::Add_Layer_Skybox(const wstring& LayerTag)
{
	wstring strTextureTag = L"Component_Texture_LobbySkybox";
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Lobby_Skybox",
		LayerTag,(void**)&strTextureTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Skybox In Layer");
		return E_FAIL;
	}

	return S_OK;
}



HRESULT CLobby::Add_Layer_UI(const wstring& LayerTag)
{
	
	if (FAILED(CStreamHandler::Load_PassData_UI(L"../../Resources/Data/Lobby_Ui.txt", FALSE)))
	{
		PRINT_LOG(L"Error", L"Failed To Load UI In Layer");
		return E_FAIL;
	}
	for (auto& pDst : *m_pManagement->Get_GameObjectList(L"Layer_UI"))
	{
		static_cast<CLobbyUI*>(pDst)->Set_Scene(this);
	}
	return S_OK;
}

HRESULT CLobby::Add_Layer_GatchaBox(const wstring & LayerTag)
{

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_GatchaBox",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Skybox In Layer");
		return E_FAIL;
	}
}


void CLobby::Set_GotoNextScene(_bool bSet)
{
	m_bGotoNextScene = bSet;
}

void CLobby::Set_IsGatcha(_bool bSet)
{
	m_bIsSetPlayerModel = !bSet;
	m_bIsGatcha = bSet;
}

void CLobby::Set_IsSetPlayerModel(_bool bSet) 
{
	m_bIsGatcha = !bSet;
	m_bIsSetPlayerModel = bSet;
}



_bool CLobby::Get_IsGatcha() const
{
	return m_bIsGatcha;
}

_bool CLobby::Get_IsSetPlayerModel() const
{
	return m_bIsSetPlayerModel;
}

CLobby * CLobby::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice)
	{
		PRINT_LOG(L"Error", L"Device is nullptr");
		return nullptr;
	}

	CLobby* pInstance = new CLobby(pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Stage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLobby::Free()
{
	/* 자식의 소멸자 호출 순서처럼 Free도 같은 순서로 호출해주자*/
	/* 1.자식 리소스 먼저 정리하고난 뒤 */
	Safe_Release(m_pPlayer);
	CScene::Free(); // 2.부모 리소스 정리	
}
