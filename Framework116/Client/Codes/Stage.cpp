#include "stdafx.h"
#include"Stage.h"
#include "Camera.h"
#include "StreamHandler.h"
#include "Asteroid.h"
#include "MissionUI.h"
#include "MainCam.h"
#include "Ring.h"
#include "ScriptUI.h"
#include "Loading.h"
#include"Pipeline.h"
#include"Player.h"
#include"HP_Bar.h"
#include "Ending.h"

CStage::CStage(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CStage::Ready_Scene()
{
	CScene::Ready_Scene();

	::SetWindowText(g_hWnd, L"Stage");
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

	CStreamHandler::Load_PassData_Map(L"../../Resources/Data/Map/tutorial.map");
	CStreamHandler::Load_PassData_Navi(L"../../Resources/Data/Navi/guide.navi");

	if (FAILED(Add_Layer_Cam(L"Layer_Cam")))
		return E_FAIL;

	if (FAILED(Add_Layer_Skybox(L"Layer_Skybox")))
		return E_FAIL;

	// 전역조명 : Directional Light
	// 행성조명 : Point Light
	// 플레이어 조명 : Sport Light

	LIGHT_DESC lightDesc;
	lightDesc.eLightType = ELightType::Directional;
	lightDesc.tLightColor = D3DCOLOR_XRGB(227, 204, 178);
	if (FAILED(Add_Layer_Light(L"Layer_Light", &lightDesc)))
		return E_FAIL;

	if (FAILED(Add_Layer_HUD(L"Layer_HUD")))
		return E_FAIL;

	m_bSceneChange = false;
	m_bFadeIn = false;
	m_bLeaveScene = false;
	m_IsRadioOff = false;
	m_eBGM = STAGE1_BGM::Opening;

	return S_OK;
}

_uint CStage::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);

	CQuestHandler::Get_Instance()->Update_Quest();

	_uint iResult = Stage_Flow(fDeltaTime);

	if (iResult == CHANGE_SCENE)
		return iResult;

	switch (m_eBGM)
	{
	case STAGE1_BGM::Opening:
		if (m_IsRadioOff == false)
		{
			m_pManagement->PlaySound(L"Tutorial_Opening.mp3", CSoundMgr::BGM);
		}

		break;
	case STAGE1_BGM::Change:
		m_pManagement->PlaySound(L"Tutorial_ChangeBGM.ogg", CSoundMgr::BGM);
		break;
	case STAGE1_BGM::Ring:
		m_pManagement->PlaySound(L"Tutorial_Ring.ogg", CSoundMgr::BGM);
		break;
	default:
		break;
	}

	return _uint();
}

_uint CStage::LateUpdate_Scene(_float fDeltaTime)
{

	CScene::LateUpdate_Scene(fDeltaTime);

	// Monster
	CCollisionHandler::Collision_SphereToSphere_Damage(L"Layer_Player_Bullet", L"Layer_Drone");
	CCollisionHandler::Collision_SphereToSphere_Damage(L"Layer_Player_Missile", L"Layer_Drone");


	CCollisionHandler::Collision_SphereToSphere_Damage(L"Layer_Player_Bullet", L"Layer_Asteroid");

	// Ring
	CCollisionHandler::Collision_PlayerToObject(L"Layer_Player", L"Layer_Ring");

	//Sniper
	CCollisionHandler::Collision_SphereToSphere_Damage(L"Layer_Player_Bullet", L"Layer_Sniper");
	CCollisionHandler::Collision_SphereToSphere_Damage(L"Layer_Player_Missile", L"Layer_Sniper");

	// TargetMonster
	CCollisionHandler::Collision_SphereToSphere_Damage(L"Layer_Player_Bullet", L"Layer_TargetMonster");

	// Planet
	 CCollisionHandler::Collision_SphereToSphere_Damage(L"Layer_Player_Bullet", L"Layer_Planet");

	// 몬스터Bullet과 플레이어의 실드배터리
	CCollisionHandler::Collision_SphereToSphere_Damage(L"Layer_Monster_Bullet", L"Layer_Shield_Battery");

	// 데미지 Src가 데미지를 입는다

	return _uint();
}


_uint CStage::Stage_Flow(_float fDeltaTime)
{
	switch (m_iFlowCount)
	{
	case 0: // 스크립트 시작
		if (m_fFlowTime >= 0)
		{
			SetCursorPos(WINCX >> 1, (WINCY >> 1) - 5);

			m_fFlowTime -= fDeltaTime;

			if (m_fFlowTime <= 0)
			{
				if (FAILED(Add_Layer_ScriptUI(L"Layer_ScriptUI", EScript::Tutorial)))
					return E_FAIL;
				++m_iFlowCount;
			}
		}
		return S_OK;

	case 1:
	{
		if (((CMainCam*)(m_pManagement->Get_GameObject(L"Layer_Cam")))->Get_SoloMoveMode() == ESoloMoveMode::End)
		{
			if (FAILED(Add_Layer_MissionUI(L"Layer_MissionUI", EQuest::Stage_1_Ring)))
				return E_FAIL;
			++m_iFlowCount;
		}
		return S_OK;
	}

	case 2:
	{
		if (CQuestHandler::Get_Instance()->Get_IsSpecial_Script(EQuest::Stage_1_Ring) == true)
		{
			m_IsRadioOff = true;
			if (FAILED(Add_Layer_ScriptUI(L"Layer_ScriptUI", EScript::Tutorial_ChangeBGM)))
				return E_FAIL;
			++m_iFlowCount;
		}
		return S_OK;
	}

	case 3:
	{
		_bool Check = (m_pManagement->Get_GameObjectList(L"Layer_ScriptUI"))->empty();
		if (Check == true)
		{
			CQuestHandler::Get_Instance()->Set_ClearStage(EStageClear::Stage_2);
			m_eBGM = STAGE1_BGM::Change;
			++m_iFlowCount;
		}
		return S_OK;
	}
	case 4:
	{
		if (CQuestHandler::Get_Instance()->Get_IsClear())
		{
			if (FAILED(Add_Layer_ScriptUI(L"Layer_ScriptUI", EScript::Tutorial_Ring_Clear)))
				return E_FAIL;
			++m_iFlowCount;
		}
		return S_OK;
	}

	case 5:
	{
		_bool Check = (m_pManagement->Get_GameObjectList(L"Layer_ScriptUI"))->empty();
		if (Check == true)
		{
			CQuestHandler::Get_Instance()->Set_Start_Quest(EQuest::Stage_1_Target);
			++m_iFlowCount;
		}
		return S_OK;
	}
	case 6:
	{
		if (CQuestHandler::Get_Instance()->Get_IsClear())
		{
			if (FAILED(Add_Layer_ScriptUI(L"Layer_ScriptUI", EScript::Tutorial_Target_Clear)))
				return E_FAIL;
			++m_iFlowCount;
		}
		return S_OK;
	}
	case 7:
	{
		_bool Check = (m_pManagement->Get_GameObjectList(L"Layer_ScriptUI"))->empty();
		if (Check == true)
		{
			CQuestHandler::Get_Instance()->Set_ClearStage(EStageClear::Stage_2);
			m_bStageClear = true;
			++m_iFlowCount;
		}
		return S_OK;
	}

	case 8:
	{
		_bool Check = (m_pManagement->Get_GameObjectList(L"Layer_ScriptUI"))->empty();
		if (Check == true)
		{
			m_bSceneChange = true;
			++m_iFlowCount;
		}

	}
	return S_OK;
	default:
		break;
	}

	if (m_bSceneChange)
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
		if (m_bLeaveScene)
		{
			if (m_bStageClear)
			{
				if (FAILED(CManagement::Get_Instance()->Setup_CurrentScene((_uint)ESceneType::Ending,
					CEnding::Create(m_pDevice, ESceneType::Stage))))
				{
					PRINT_LOG(L"Error", L"Failed To Setup Stage Scene");
				}
			}
			else
			{
				if (FAILED(CManagement::Get_Instance()->Setup_CurrentScene((_uint)ESceneType::Loading,
					CLoading::Create(m_pDevice, ESceneType::Lobby))))

				{
					PRINT_LOG(L"Error", L"Failed To Setup Stage Scene");
				}
			}
			m_bLeaveScene = false;
			return CHANGE_SCENE;
		}
	}

	return S_OK;
}

HRESULT CStage::Add_Layer_Cam(const wstring & LayerTag)
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

	

	HRESULT CStage::Add_Layer_Skybox(const wstring& LayerTag)
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

HRESULT CStage::Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc)
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

HRESULT CStage::Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc)
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

CStage * CStage::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice)
	{
		PRINT_LOG(L"Error", L"Device is nullptr");
		return nullptr;
	}

	CStage* pInstance = new CStage(pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Stage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage::Free()
{
	/* 자식의 소멸자 호출 순서처럼 Free도 같은 순서로 호출해주자*/
	/* 1.자식 리소스 먼저 정리하고난 뒤 */
	CScene::Free(); // 2.부모 리소스 정리	
}

HRESULT CStage::Add_Layer_HUD(const wstring& LayerTag)
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
	//UI_DESC HUD_Boarder_Gatling;
	//HUD_Boarder_Gatling.tTransformDesc.vPosition = { -300.f, 435.f, 0.f };
	//HUD_Boarder_Gatling.tTransformDesc.vScale = { 201.f, 123.f, 0.f };
	//HUD_Boarder_Gatling.wstrTexturePrototypeTag = L"Component_Texture_HUD_Boarder";
	//if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Boarder_Gatling)))
	//	return E_FAIL;

	// Skill OverDrive

	//UI_DESC OverdriveHUD;
	//OverdriveHUD.tTransformDesc.vPosition = { -20.f, 435.f, 0.f };
	//OverdriveHUD.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
	//OverdriveHUD.wstrTexturePrototypeTag = L"Component_Texture_Overdrive_HUD";
	//if (FAILED(Add_Layer_UI(L"Layer_HUD", &OverdriveHUD)))
	//	return E_FAIL;

	//UI_DESC HUD_Boarder_OverDrive;
	//HUD_Boarder_OverDrive.tTransformDesc.vPosition = { -20.f, 435.f, 0.f };
	//HUD_Boarder_OverDrive.tTransformDesc.vScale = { 201.f, 123.f, 0.f };
	//HUD_Boarder_OverDrive.wstrTexturePrototypeTag = L"Component_Texture_HUD_Boarder";
	//if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Boarder_OverDrive)))
	//	return E_FAIL;

	//// Skill Shield
	//UI_DESC HUD_Shield_Battery;
	//HUD_Shield_Battery.tTransformDesc.vPosition = { 260.f, 435.f, 0.f };
	//HUD_Shield_Battery.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
	//HUD_Shield_Battery.wstrTexturePrototypeTag = L"Component_Texture_Shield_Battery_HUD";
	//if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Shield_Battery)))
	//	return E_FAIL;

	//UI_DESC HUD_Boarder_Shield;
	//HUD_Boarder_Shield.tTransformDesc.vPosition = { 260.f, 435.f, 0.f };
	//HUD_Boarder_Shield.tTransformDesc.vScale = { 201.f, 123.f, 0.f };
	//HUD_Boarder_Shield.wstrTexturePrototypeTag = L"Component_Texture_HUD_Boarder";
	//if (FAILED(Add_Layer_UI(L"Layer_HUD", &HUD_Boarder_Shield)))
	//	return E_FAIL;

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

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_HUD_AimPanel",
		L"Layer_HUD")))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage::Add_Layer_ScriptUI(const wstring & LayerTag, EScript eScript)
{
	UI_DESC Desc;
	Desc.tTransformDesc.vPosition = { 0.f, 0.f ,0.f };
	Desc.tTransformDesc.vScale = { 1.f, 1.f,0.f };
	Desc.wstrTexturePrototypeTag = L"Component_Texture_ScriptUI_Script";

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ScriptUI",
		LayerTag, &Desc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add ScriptUI In Layer");
		return E_FAIL;
	}

	((CScriptUI*)m_pManagement->Get_GameObject(LayerTag))->Set_Script(eScript);

	return S_OK;
}

HRESULT CStage::Add_Layer_MissionUI(const wstring & LayerTag, EQuest eQuest)
{
	CQuestHandler::Get_Instance()->Set_Start_Quest(eQuest);

	UI_DESC Desc;
	Desc.tTransformDesc.vPosition = { 835.f, -50.f ,0.f };
	Desc.tTransformDesc.vScale = { 201.f, 123.f,0.f };
	Desc.wstrTexturePrototypeTag = L"Component_Texture_HUD_Mission";
	
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_MissionUI",
		LayerTag, &Desc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add ScriptUI In Layer");
		return E_FAIL;
	}


	return S_OK;
}