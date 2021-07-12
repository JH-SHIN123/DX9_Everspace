#include "stdafx.h"
#include "..\Headers\Lobby.h"
#include "Camera.h"
#include"Player.h"
#include"Loading.h"
#include"LobbyUI.h"
#include"LobbyModel.h"
#include"LobbyCam.h"
#include"GatchaBox.h"
#include"StatusBoard.h"
#include"Status.h"
#include"LobbyScriptUI.h"
CLobby::CLobby(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CLobby::Ready_Scene()
{
	::SetWindowText(g_hWnd, L"Lobby");

	m_pManagement->StopSound(CSoundMgr::BGM);

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
	if (FAILED(Add_Layer_StatusBoard(L"Layer_StatusBoard")))
		return E_FAIL;
	if (FAILED(Add_Layer_Status(L"Layer_Status")))
		return E_FAIL;
	if (FAILED(Add_Layer_Cursor(L"Layer_Cursor")))
		return E_FAIL;
	
	LIGHT_DESC lightDesc;
	lightDesc.eLightType = ELightType::Directional;
	lightDesc.vLightDir = { 1.0f, -0.0f, 0.25f };
	lightDesc.tLightColor = D3DCOLOR_XRGB(255, 255, 255);
	if (FAILED(Add_Layer_Light(L"Layer_Light", &lightDesc)))
		return E_FAIL;

	m_tUnitInfo = { 20,30,50,30,40,70 };
	return S_OK;
}

_uint CLobby::Update_Scene(_float fDeltaTime)
{
	m_pManagement->PlaySound(L"Garage_BGM.ogg", CSoundMgr::BGM);

	if (m_bSceneChange)
	{
		m_pManagement->Clear_NonStatic_Resources();
		if (FAILED(CManagement::Get_Instance()->Setup_CurrentScene((_uint)ESceneType::Loading,
			CLoading::Create(m_pDevice, ESceneType::Stage))))
		{
			PRINT_LOG(L"Error", L"Failed To Setup Stage Scene");
			return E_FAIL;
		}
		return CHANGE_SCENE;
	}
	if (m_bIsGatcha)
	{
		if (!m_pManagement->Get_GameObjectList(L"Layer_ScriptUI"))
		{
			Add_Layer_ScriptUI(L"Layer_ScriptUI");
		}
		else
		{
			if (!m_pManagement->Get_GameObjectList(L"Layer_ScriptUI")->size())
			{
				Add_Layer_ScriptUI(L"Layer_ScriptUI");
			}
		}
	}
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
	if (m_bStartUnPacking)
	{
		for (auto& pDst : *m_pManagement->Get_GameObjectList(L"Layer_UI"))
		{
			static_cast<CLobbyUI*>(pDst)->Set_StartUnPacking(TRUE);
		}
		CLobbyCam* pCam = (CLobbyCam*)m_pManagement->Get_GameObject(L"Layer_Cam");
		pCam->Set_StartUnPacking(TRUE);
	}
	else
	{
		for (auto& pDst : *m_pManagement->Get_GameObjectList(L"Layer_UI"))
		{
			static_cast<CLobbyUI*>(pDst)->Set_StartUnPacking(FALSE);
		}
		CLobbyCam* pCam = (CLobbyCam*)m_pManagement->Get_GameObject(L"Layer_Cam");
		pCam->Set_StartUnPacking(FALSE);
	}
	
	return _uint();
}

HRESULT CLobby::Add_Layer_Light(const wstring & LayerTag, const LIGHT_DESC * pLightDesc)
{

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_Light",
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
	CLobbyModel* pModel = (CLobbyModel*)m_pManagement->Get_GameObject(LayerTag);
	pModel->Set_Scene(this);

	
	return S_OK;


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
		if (!static_cast<CLobbyUI*>(pDst)->Get_Scene())
		{
			static_cast<CLobbyUI*>(pDst)->Set_Scene(this);
		}
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
	CGatchaBox* pBox = (CGatchaBox*)(m_pManagement->Get_GameObject(LayerTag));
	pBox->Set_Scene(this);

	return S_OK;

}

HRESULT CLobby::Add_Layer_StatusBoard(const wstring & LayerTag)
{

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_StatusBoard",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add StatusBoard In Layer");
		return E_FAIL;
	}
	CStatusBoard* pBoard = (CStatusBoard*)m_pManagement->Get_GameObject(LayerTag);
	pBoard->Set_Scene(this);
	return S_OK;
}

HRESULT CLobby::Add_Layer_Status(const wstring & LayerTag)
{

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Status",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add StatusBoard In Layer");
		return E_FAIL;
	}
	CStatus* pBoard = (CStatus*)m_pManagement->Get_GameObject(LayerTag);
	pBoard->Set_Scene(this);
	return S_OK;
}

HRESULT CLobby::Add_Layer_ScriptUI(const wstring & LayerTag)
{
	UI_DESC Desc;
	Desc.tTransformDesc.vPosition = { 0.f, 0.f ,0.f };
	Desc.tTransformDesc.vScale = { 1.f, 1.f,0.f };
	Desc.wstrTexturePrototypeTag = L"Component_Texture_ScriptUI_Script";
	
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_LobbyScriptUI",
		LayerTag, &Desc)))
		{
			PRINT_LOG(L"Error", L"Failed To Add ScriptUI In Layer");
			return E_FAIL;
		}
	CLobbyScriptUI* pUI = (CLobbyScriptUI*)m_pManagement->Get_GameObject(LayerTag);
	pUI->Set_Scene(this);
	return S_OK;
}

HRESULT CLobby::Add_Layer_Cursor(const wstring & LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_LobbyCursor",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add ScriptUI In Layer");
		return E_FAIL;
	}
	return S_OK;
}





void CLobby::Set_GotoNextScene(_bool bSet)
{
	m_bIsGatcha = !bSet;
	m_bIsSetPlayerModel = !bSet;
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

void CLobby::Set_SceneChange(_bool bSet)
{
	m_bSceneChange = bSet;
}

void CLobby::Set_StartUnPacking(_bool bSet)
{
	m_bStartUnPacking = bSet;
}

void CLobby::Set_SceneSelect(_bool bSet)
{
	m_bSelectScene = bSet;
}



_bool CLobby::Get_IsGatcha() const
{
	return m_bIsGatcha;
}

_bool CLobby::Get_IsSetPlayerModel() const
{
	return m_bIsSetPlayerModel;
}

_bool CLobby::Get_GotoNextScene() const
{
	return m_bGotoNextScene;
}

_bool CLobby::Get_SceneSelect() const
{
	return m_bSelectScene;
}

_bool CLobby::Get_StartUnPacking() const
{
	return m_bStartUnPacking;
}

_uint CLobby::Get_Money() const
{
	return m_iMoney;
}

UNIT_INFO * CLobby::Get_UnitInfo()
{
	return &m_tUnitInfo;
}

void CLobby::Set_UnitInfo(UNIT_INFO _tUnitInfo)
{
	m_tUnitInfo = _tUnitInfo;
}

void CLobby::Set_Money(_uint _iMoney)
{
	m_iMoney += _iMoney;
}

_bool CLobby::GetStageLock(_uint iStageIdx)
{
	if (iStageIdx > 3)
		return TRUE;
	return m_bStageLock[iStageIdx];
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

	Safe_Release(m_pPlayer);
	CScene::Free();
}
