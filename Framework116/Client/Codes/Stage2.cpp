#include "stdafx.h"
#include "Stage2.h"
#include "MainCam.h"
#include"Player.h"
#include"Pipeline.h"
#include"Loading.h"
CStage2::CStage2(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CStage2::Ready_Scene()
{
	CScene::Ready_Scene();
	::SetWindowText(g_hWnd, L"CStage2");
	m_pManagement->StopSound(CSoundMgr::BGM);

	if (FAILED(Add_Layer_Player(L"Layer_Player")))
		return E_FAIL;

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
	
	if (FAILED(Add_Layer_Cam(L"Layer_Cam")))
		return E_FAIL;

	if (FAILED(Add_Layer_Skybox(L"Layer_Skybox")))
		return E_FAIL;

	LIGHT_DESC lightDesc;
	lightDesc.eLightType = ELightType::Directional;
	lightDesc.tLightColor = D3DCOLOR_XRGB(135, 135, 135);
	if (FAILED(Add_Layer_Light(L"Layer_Light", &lightDesc)))
		return E_FAIL;

	if (FAILED(Add_Layer_HUD(L"Layer_HUD")))
		return E_FAIL;

	Ready_Asteroid();

	return S_OK;
}

_uint CStage2::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);
	m_pManagement->PlaySound(L"Tutorial_Ambience.ogg", CSoundMgr::BGM);

	CQuestHandler::Get_Instance()->Update_Quest();
	CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player");
	CTransform* pPlayerTransform = (CTransform*)pPlayer->Get_Component(L"Com_Transform");

	switch (Stage2_Flow(fDeltaTime))
	{
	case -1:
		return UPDATE_ERROR;
	case TRUE:
		break;
	case UPDATE_FLYAWAY:
		AsteroidFlyingAway(fDeltaTime, 200.f, 200.f, 200.f, 200.f, pPlayerTransform, 30, 60.f, 300.f);
		break;
	case PLAYER_DEAD:
		m_fDelaySceneChange += fDeltaTime;
		if (m_fDelaySceneChange >= 5.f)
		{
			m_pManagement->Clear_NonStatic_Resources();
			if (FAILED(CManagement::Get_Instance()->Setup_CurrentScene((_uint)ESceneType::Loading,
				CLoading::Create(m_pDevice, ESceneType::Stage2))))
			{
				PRINT_LOG(L"Error", L"Failed To Setup Stage Scene");
				return E_FAIL;
			}
			return CHANGE_SCENE;
		}
		break;
	case CLEAR_FLYAWAY:
		if (!m_bLoadMapNavi)
		{
			CStreamHandler::Load_PassData_Map(L"../../Resources/Data/Map/stage1.map");
			CStreamHandler::Load_PassData_Navi(L"../../Resources/Data/Navi/stage1.navi");
			m_bLoadMapNavi = TRUE;
		}
		break;
	}

	return _uint();
}

_uint CStage2::LateUpdate_Scene(_float fDeltaTime)
{
	CScene::LateUpdate_Scene(fDeltaTime);

	return _uint();
}

HRESULT CStage2::Add_Layer_Player(const wstring & LayerTag)
{


		GAMEOBJECT_DESC tDesc;
		tDesc.wstrMeshName = L"Component_Mesh_BigShip";
		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::Static,
			L"GameObject_Player",
			LayerTag, (void**)&tDesc)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Player In Layer");
			return E_FAIL;
		}

		return S_OK;
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

HRESULT CStage2::Add_Layer_ScriptUI(const wstring & LayerTag, EScript eScript)
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

HRESULT CStage2::Add_Layer_MissionUI(const wstring & LayerTag, EQuest eQuest)
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
HRESULT CStage2::Add_Layer_Asteroid(const wstring & LayerTag, GAMEOBJECT_DESC tDesc)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Asteroid",
		LayerTag, (void**)&tDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Sniper In Layer");
		return E_FAIL;
	}
	return S_OK;
}


void CStage2::Ready_Asteroid()
{
	GAMEOBJECT_DESC tDesc;
	tDesc.wstrMeshName = L"Component_Mesh_Rock_Generic_001";
	tDesc.tTransformDesc.vScale = { 1.f,1.f,1.f };
	tDesc.tTransformDesc.fSpeedPerSec = 1.f;
	tDesc.tTransformDesc.fRotatePerSec = D3DXToRadian(CPipeline::GetRandomFloat(30, 45));
	CTransform* pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	_uint iRockCount = 20;
	for (_uint i = 0; i < iRockCount; i++)
	{
		_float3 vRockPos = pPlayerTransform->Get_TransformDesc().vPosition;
		if (i != iRockCount - 1)
		{

			if (i % 4 == 0)
			{
				vRockPos.x -= CPipeline::GetRandomFloat(10, 100);
				vRockPos.y -= CPipeline::GetRandomFloat(10, 100);
			}
			else if (i % 4 == 1)
			{
				vRockPos.x += CPipeline::GetRandomFloat(10, 100);
				vRockPos.y += CPipeline::GetRandomFloat(10, 100);
			}
			else if (i % 4 == 2)
			{
				vRockPos.x += CPipeline::GetRandomFloat(10, 100);
				vRockPos.y -= CPipeline::GetRandomFloat(10, 100);
			}
			else
			{
				vRockPos.x -= CPipeline::GetRandomFloat(10, 100);
				vRockPos.y += CPipeline::GetRandomFloat(10, 100);
			}
		}
		vRockPos.z = 300.f + ((i + 1) * 10.f);
		tDesc.tTransformDesc.vPosition = vRockPos;
		Add_Layer_Asteroid(L"Layer_Asteroid", tDesc);
	}
}
_uint CStage2::Stage2_Flow(_float fDeltaTime)
{
	//if (!m_bEnterScene)
	//	return TRUE;
	CPlayer* pPlayer = (CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player");
	if (pPlayer)
	{
		if (pPlayer->Get_IsDead())
			m_iFlowCount = PLAYER_DEAD;
	}
	switch (m_iFlowCount)
	{
	case 0: // 스크립트 시작
		if (m_fFlowTime >= 0)
		{
			SetCursorPos(WINCX >> 1, (WINCY >> 1) - 5);

			m_fFlowTime -= fDeltaTime;

			if (m_fFlowTime <= 0)
			{
				if (FAILED(Add_Layer_ScriptUI(L"Layer_ScriptUI", EScript::Stg2_Begin)))
					return -1;
				++m_iFlowCount;
			}
		}
		return TRUE;
	case 1: // 스크립트 시작
		if (((CMainCam*)(m_pManagement->Get_GameObject(L"Layer_Cam")))->Get_SoloMoveMode() == ESoloMoveMode::End)
		{
			if (FAILED(Add_Layer_ScriptUI(L"Layer_ScriptUI", EScript::Stg2_AfterCamProduction)))
				return -1;
			++m_iFlowCount;
		}
		return TRUE;
	case 2:
		if (!m_pManagement->Get_GameObjectList(L"Layer_ScriptUI")->size())
		{
			if (m_fFlowTime <= 1)
			{
				m_fFlowTime += fDeltaTime;
				if (m_fFlowTime >= 1)
				{
					++m_iFlowCount;
				}
			}
		}
		return TRUE;
	case 3:
		if (m_bFinishFlyAway)
		{
			if (m_pManagement->Get_GameObjectList(L"Layer_Asteroid")->size())
			{
				for (auto& pDst : *m_pManagement->Get_GameObjectList(L"Layer_Asteroid"))
				{
					pDst->Set_IsDead(TRUE);
				}
			}
			if (FAILED(Add_Layer_ScriptUI(L"Layer_ScriptUI", EScript::Stg2_Finish_AsteroidFlyAway)))
				return -1;
			m_iFlowCount = CLEAR_FLYAWAY;
			return TRUE;
		}
		return UPDATE_FLYAWAY;
	case 4:
	{
		if (!m_bPlayPlayerDeadScript&& pPlayer->Get_IsDead())
		{
			if (FAILED(Add_Layer_ScriptUI(L"Layer_ScriptUI", EScript::Stg2_PlayerDead)))
				return -1;
			m_bPlayPlayerDeadScript = TRUE;
		}
	}
	return PLAYER_DEAD;
	case 5:
		if (!m_pManagement->Get_GameObjectList(L"Layer_ScriptUI")->size())
		{
			return CLEAR_FLYAWAY;
		}
	default:
		return TRUE;
	}

	return S_OK;
}


//TRUE반환시 끝났음
_bool CStage2::AsteroidFlyingAway(_float fDeltaTime, _float fMaxXDist, _float fMaxYDist,
	_float fMaxZDist, _float fMinZDist, CTransform* pTargetTransform, _uint iRockAmount,
	_float fRockSpeed, _float fDistFromTarget)
{
	if (nullptr == pTargetTransform)
	{
		PRINT_LOG(L"Err", L"pTargetTransform is nullptr");
		return FALSE;
	}
	_float fFinishTime = 60.f;
	m_fFlyingAsteroidTime += fDeltaTime;
	if (m_fFlyingAsteroidTime >= fFinishTime)
	{
		m_bFinishFlyAway = TRUE;
		return TRUE;
	}

	if (!m_bStartFlyAway)
	{
		_float3 vRockPos = pTargetTransform->Get_TransformDesc().vPosition;
		GAMEOBJECT_DESC tDesc;
		tDesc.wstrMeshName = L"Component_Mesh_Rock_Generic_001";
		tDesc.tTransformDesc.vScale = { 1.f,1.f,1.f };
		tDesc.tTransformDesc.fSpeedPerSec = 1.f;
		tDesc.tTransformDesc.fRotatePerSec = D3DXToRadian(CPipeline::GetRandomFloat(30, 45));
		for (_uint i = 0; i < iRockAmount; i++)
		{
			vRockPos.x += CPipeline::GetRandomFloat(0, fMaxXDist / 2.f);
			vRockPos.x -= CPipeline::GetRandomFloat(0, fMaxXDist / 2.f);
			vRockPos.y += CPipeline::GetRandomFloat(0, fMaxYDist / 2.f);
			vRockPos.y -= CPipeline::GetRandomFloat(0, fMaxYDist / 2.f);

			vRockPos.z += CPipeline::GetRandomFloat(0, fMaxZDist) + fMinZDist;
			tDesc.tTransformDesc.vPosition = vRockPos;
			Add_Layer_Asteroid(L"Layer_Asteroid", tDesc);
			if (iRockAmount < m_pManagement->Get_GameObjectList(L"Layer_Asteroid")->size())
				break;
		}
		for (auto& pDst : *m_pManagement->Get_GameObjectList(L"Layer_Asteroid"))
			pDst->Update_GameObject(fDeltaTime);
		m_bStartFlyAway = TRUE;
		return FALSE;
	}
	_float fLongLange = fMaxZDist + fMinZDist + fDistFromTarget;
	_float fNeedToFinishTime = fLongLange / fRockSpeed;
	_float3 vTargetPos = pTargetTransform->Get_TransformDesc().vPosition;
	_float3 vDir = { 0,0,-1 };
	CTransform* pTransform = nullptr;
	for (auto& pObj : *m_pManagement->Get_GameObjectList(L"Layer_Asteroid"))
	{
		_float3 vRockPos = vTargetPos;
		pTransform = (CTransform*)pObj->Get_Component(L"Com_Transform");
		pTransform->Go_Dir(vDir, fRockSpeed * fDeltaTime);
		pTransform->RotateX(fDeltaTime);
		//다지나가는데 필요한 시간

		if (m_fFlyingAsteroidTime <= fFinishTime - fNeedToFinishTime)
		{
			if (pTransform->Get_TransformDesc().vPosition.z
				<= vTargetPos.z - fDistFromTarget)
			{
				vRockPos.x += CPipeline::GetRandomFloat(0, fMaxXDist / 2.f);
				vRockPos.x -= CPipeline::GetRandomFloat(0, fMaxXDist / 2.f);

				vRockPos.y += CPipeline::GetRandomFloat(0, fMaxYDist / 2.f);
				vRockPos.y -= CPipeline::GetRandomFloat(0, fMaxYDist / 2.f);

				vRockPos.z += CPipeline::GetRandomFloat(fMaxZDist*0.5f, fMaxZDist) + fMinZDist;
				pTransform->Set_Position(vRockPos);
			}
		}


	}

	return FALSE;
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
