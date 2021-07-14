#include "stdafx.h"
#include "Stage2.h"
#include "MainCam.h"

CStage2::CStage2(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CStage2::Ready_Scene()
{
	CScene::Ready_Scene();
	::SetWindowText(g_hWnd, L"CStage2");
	m_pManagement->StopSound(CSoundMgr::BGM);

	// Fade Out
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_FadeOut",
		L"Layer_Fade",
		this)))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_FadeOut In Layer");
		return E_FAIL;
	}

	CStreamHandler::Load_PassData_Map(L"../../Resources/Data/Map/stage1.map");
	CStreamHandler::Load_PassData_Navi(L"../../Resources/Data/Navi/stage1.navi");
	
	if (FAILED(Add_Layer_Cam(L"Layer_Cam")))
		return E_FAIL;

	if (FAILED(Add_Layer_Skybox(L"Layer_Skybox")))
		return E_FAIL;

	LIGHT_DESC lightDesc;
	lightDesc.eLightType = ELightType::Directional;
	lightDesc.tLightColor = D3DCOLOR_XRGB(255, 255, 255);
	if (FAILED(Add_Layer_Light(L"Layer_Light", &lightDesc)))
		return E_FAIL;

	if (FAILED(Add_Layer_HUD(L"Layer_HUD")))
		return E_FAIL;

	if (FAILED(Add_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	/*if (FAILED(Add_Layer_Sniper(L"Layer_Sniper")))
		return E_FAIL;*/

	return S_OK;
}

_uint CStage2::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);


	return _uint();
}

_uint CStage2::LateUpdate_Scene(_float fDeltaTime)
{
	CScene::LateUpdate_Scene(fDeltaTime);

	return _uint();
}

HRESULT CStage2::Add_Layer_Cam(const wstring& LayerTag)
{
	CAMERA_DESC CameraDesc;
	CameraDesc.fFovY = D3DXToRadian(90.f);
	CameraDesc.fAspect = (_float)WINCX / WINCY;
	CameraDesc.fNear = 1.f;
	CameraDesc.fFar = 10000.f;

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_MainCam",
		LayerTag,
		&CameraDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Player In Main Cam");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage2::Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_Light",
		LayerTag,
		(void*)pLightDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Light In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage2::Add_Layer_Skybox(const wstring& LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Skybox",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Skybox In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage2::Add_Layer_HUD(const wstring& LayerTag)
{
	// Crosshair
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Crosshair",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Crosshair In Layer");
		return E_FAIL;
	}

	// AimAssist
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_AimAssist",
		L"Layer_AimAssist")))
	{
		PRINT_LOG(L"Error", L"Failed To Add Layer_AimAssist In Layer");
		return E_FAIL;
	}

	// AimAssist
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_AimAssist2",
		L"Layer_AimAssist2")))
	{
		PRINT_LOG(L"Error", L"Failed To Add Layer_AimAssist2 In Layer");
		return E_FAIL;
	}



	// Weapon Gatling -> 테두리 빼고 플레이어로 통합.

	UI_DESC HUD_Boarder_Gatling;
	HUD_Boarder_Gatling.tTransformDesc.vPosition = { -300.f, 435.f, 0.f };
	HUD_Boarder_Gatling.tTransformDesc.vScale = { 201.f, 123.f, 0.f };
	HUD_Boarder_Gatling.wstrTexturePrototypeTag = L"Component_Texture_HUD_Boarder";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Boarder_Gatling)))
		return E_FAIL;

	// Skill OverDrive

	UI_DESC OverdriveHUD;
	OverdriveHUD.tTransformDesc.vPosition = { -20.f, 435.f, 0.f };
	OverdriveHUD.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
	OverdriveHUD.wstrTexturePrototypeTag = L"Component_Texture_Overdrive_HUD";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &OverdriveHUD)))
		return E_FAIL;

	UI_DESC HUD_Boarder_OverDrive;
	HUD_Boarder_OverDrive.tTransformDesc.vPosition = { -20.f, 435.f, 0.f };
	HUD_Boarder_OverDrive.tTransformDesc.vScale = { 201.f, 123.f, 0.f };
	HUD_Boarder_OverDrive.wstrTexturePrototypeTag = L"Component_Texture_HUD_Boarder";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Boarder_OverDrive)))
		return E_FAIL;

	// Skill Shield
	UI_DESC HUD_Shield_Battery;
	HUD_Shield_Battery.tTransformDesc.vPosition = { 260.f, 435.f, 0.f };
	HUD_Shield_Battery.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
	HUD_Shield_Battery.wstrTexturePrototypeTag = L"Component_Texture_Shield_Battery_HUD";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Shield_Battery)))
		return E_FAIL;

	UI_DESC HUD_Boarder_Shield;
	HUD_Boarder_Shield.tTransformDesc.vPosition = { 260.f, 435.f, 0.f };
	HUD_Boarder_Shield.tTransformDesc.vScale = { 201.f, 123.f, 0.f };
	HUD_Boarder_Shield.wstrTexturePrototypeTag = L"Component_Texture_HUD_Boarder";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Boarder_Shield)))
		return E_FAIL;

	// Player Status (Shield, HP)

	UI_DESC HUD_Shield;
	HUD_Shield.tTransformDesc.vPosition = { -858.f, 460.f, 0.f };
	HUD_Shield.tTransformDesc.vScale = { 30.f, 30.f, 0.f };
	HUD_Shield.wstrTexturePrototypeTag = L"Component_Texture_HUD_Shield";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Shield)))
		return E_FAIL;

	UI_DESC HUD_Hp;
	HUD_Hp.tTransformDesc.vPosition = { -858.f, 424.f, 0.f };
	HUD_Hp.tTransformDesc.vScale = { 30.f, 30.f, 0.f };
	HUD_Hp.wstrTexturePrototypeTag = L"Component_Texture_HUD_Hp";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Hp)))
		return E_FAIL;

	// 없는게 낫네 ㅋㅋ
	//UI_DESC HUD_Boarder;
	//HUD_Boarder.tTransformDesc.vPosition = { -295.f, 250.f, 0.f };
	//HUD_Boarder.tTransformDesc.vScale = { 160.f, 61.f, 0.f };
	//HUD_Boarder.wstrTexturePrototypeTag = L"Component_Texture_HUD_Boarder";
	//if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Boarder)))
	//	return E_FAIL;

	UI_DESC HUD_Shield_InBar;
	HUD_Shield_InBar.tTransformDesc.vPosition = { -700.f, 460.f, 0.f };
	HUD_Shield_InBar.tTransformDesc.vScale = { 260.f, 12.f, 0.f };
	HUD_Shield_InBar.wstrTexturePrototypeTag = L"Component_Texture_HUD_In_Bar";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Shield_InBar)))
		return E_FAIL;

	UI_DESC HUD_Shield_OutBar;
	HUD_Shield_OutBar.tTransformDesc.vPosition = { -700.f, 460.f, 0.f };
	HUD_Shield_OutBar.tTransformDesc.vScale = { 262.f, 14.f, 0.f };
	HUD_Shield_OutBar.wstrTexturePrototypeTag = L"Component_Texture_HUD_Out_Bar";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Shield_OutBar)))
		return E_FAIL;

	UI_DESC HUD_HP_InBar;
	HUD_HP_InBar.tTransformDesc.vPosition = { -700.f, 424.f, 0.f };
	HUD_HP_InBar.tTransformDesc.vScale = { 260.f, 12.f, 0.f };
	HUD_HP_InBar.wstrTexturePrototypeTag = L"Component_Texture_HUD_In_Bar";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_HP_InBar)))
		return E_FAIL;

	UI_DESC HUD_HP_OutBar;
	HUD_HP_OutBar.tTransformDesc.vPosition = { -700.f, 424.f, 0.f };
	HUD_HP_OutBar.tTransformDesc.vScale = { 262.f, 14.f, 0.f };
	HUD_HP_OutBar.wstrTexturePrototypeTag = L"Component_Texture_HUD_Out_Bar";
	if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_HP_OutBar)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStage2::Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_UI",
		LayerTag,
		(void*)pUIDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage2::Add_Layer_Monster(const wstring & LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Monster",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Monster In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage2::Add_Layer_Sniper(const wstring & LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Sniper",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Sniper In Layer");
		return E_FAIL;
	}
	return S_OK;
}

CStage2* CStage2::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice)
	{
		PRINT_LOG(L"Error", L"Device is nullptr");
		return nullptr;
	}

	CStage2* pInstance = new CStage2(pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Stage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage2::Free()
{
	CScene::Free(); // 2.부모 리소스 정리	
}
