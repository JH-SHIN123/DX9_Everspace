
#include "stdafx.h"
#include "..\Headers\Loading.h"

#pragma region Scenes
#include "Stage.h"
#include"Lobby.h"
#pragma endregion

#pragma region GameObjects
#include "Terrain.h"
#include "Monster.h"
#include "Grass.h"
#include "Skybox.h"
#include "ExplosionSystem.h"
#include "LaserSystem.h"	
#include "Boss_Monster.h"
#include"LobbyCam.h"
#include"LobbyModel.h"
#include "Bullet_EnergyBall.h"
#include "Bullet_Laser.h"
#include "Bullet_EMP_Bomb.h"
#include "Crosshair.h"
#include"LobbyUI.h"
#include"GatchaBox.h"
#include "Boss_Warmhole.h"
#include "Boss_Spawn_Monster.h"
#include "Ring.h"
#include "TargetMonster.h"
#include "FollowSystem.h"
#include "EngineEffectSystem.h"
#include "LockOn.h"
#include "Planet.h"
#include "Meteor.h"
#include "TutorialUI.h"
#include "Drone.h"
#include "WingBoost_System.h"
#include"Product.h"
#include"StatusBoard.h"
#include"Status.h"
#include"VIBuffer_HexagonTex.h"
#include "Meteor.h"
#include "ScriptUI.h"
#include "HP_Bar.h"
#include "HP_Bar_Border.h"
#include "Stamina_Bar.h"
#include "Asteroid.h"
#include "MissionUI.h"
#include "BackUI.h"
#include "NaviArrow.h"
#include "AimAssist.h"
#include "AimAssist2.h"
#include"LobbyBackUI.h"
#include"LobbyScriptUI.h"
#include"LobbyCursor.h"

// 3Stage필요
#include "Sniper.h"
#pragma endregion


CLoading::CLoading(LPDIRECT3DDEVICE9 pDevice, ESceneType eNextSceneID)
	: CScene(pDevice)
	, m_eNextSceneID(eNextSceneID)

{
}

HRESULT CLoading::Ready_Scene()
{
	CScene::Ready_Scene();

	::SetWindowText(g_hWnd, L"Loading");

	m_hLoadingThread = (HANDLE)_beginthreadex(0, 0, ThreadMain, this, 0, 0);
	if (nullptr == m_hLoadingThread)
	{
		PRINT_LOG(L"Error", L"Failed To _beginthreadex");
		return E_FAIL;
	}
	m_pManagement->StopSound(CSoundMgr::BGM);
	InitializeCriticalSection(&m_CriticalSection);

	return S_OK;
}

_uint CLoading::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);
	
	m_pManagement->PlaySound(L"Loading_Ambience.ogg", CSoundMgr::BGM);
	if (m_IsFinished)
	{
		//
		CScene* pNextScene = nullptr;
		switch (m_eNextSceneID)
		{
		case ESceneType::Stage:
			pNextScene = CStage::Create(m_pDevice);
			break;
		case ESceneType::Lobby:
			pNextScene = CLobby::Create(m_pDevice);
			break;
		}

		if (FAILED(m_pManagement->Setup_CurrentScene((_uint)m_eNextSceneID, pNextScene)))
		{
			PRINT_LOG(L"Error", L"Failed To Setup Next Scene");
			return UPDATE_ERROR;
		}

		return CHANGE_SCENE;
	}

	return _uint();
}

_uint CLoading::LateUpdate_Scene(_float fDeltaTime)
{
	CScene::LateUpdate_Scene(fDeltaTime);

	return _uint();
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pDevice, ESceneType eNextSceneID)
{
	if (nullptr == pDevice)
	{
		PRINT_LOG(L"Error", L"Device is nullptr");
		return nullptr;
	}

	CLoading* pInstance = new CLoading(pDevice, eNextSceneID);
	if (FAILED(pInstance->Ready_Scene()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free()
{
	/* 자식의 소멸자 호출 순서처럼 Free도 같은 순서로 호출해주자*/
	/* 1.자식 리소스 먼저 정리하고난 뒤 */
	CloseHandle(m_hLoadingThread);
	DeleteCriticalSection(&m_CriticalSection);

	CScene::Free(); // 2.부모 리소스 정리


}

unsigned CLoading::ThreadMain(void * pArg)

{

	CLoading* pLoading = (CLoading*)pArg;
	EnterCriticalSection(&pLoading->m_CriticalSection);

	HRESULT hr = 0;
	switch (pLoading->m_eNextSceneID)
	{
	case ESceneType::Stage:

		hr = pLoading->Ready_StageResources();
		break;
	case ESceneType::Lobby:
		hr = pLoading->Ready_LobbyResources();
		break;
	default:
		break;
	}

	if (FAILED(hr))
	{
		LeaveCriticalSection(&pLoading->m_CriticalSection);
		return LOADING_ERROR;
	}

	pLoading->m_IsFinished = true;
	LeaveCriticalSection(&pLoading->m_CriticalSection);

	return NO_EVENT;
}

HRESULT CLoading::Ready_StageResources()
{
	m_pManagement->Clear_NonStatic_Resources();

#pragma region GameObjects
	/* For.GameObject_Monster */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Monster",
		CMonster::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Monster");
		return E_FAIL;
	}

	/* For.GameObject_Sniper */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Sniper",
		CSniper::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Sniper");
		return E_FAIL;
	}

	/* For.GameObject_Skybox */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Skybox",
		CSkybox::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Skybox");
		return E_FAIL;
	}

	/* 임시 보스 몬스터 입니다. */
	Ready_BossAndOthers();

	//////////////////////////////3스테이지에도 필요!!/////////////////////////////////////////////////////////////////////////////////
	/*  HUD Crosshair 입니다 */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Crosshair",
		CCrosshair::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Crosshair");
		return E_FAIL;
	}

	/*  HUD AimAssist 입니다 */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_AimAssist",
		CAimAssist::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_AimAssist");
		return E_FAIL;
	}

	/*  HUD AimAssist 입니다 */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_AimAssist2",
		CAimAssist2::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_AimAssist2");
		return E_FAIL;
	}

	/*  HUD LockOn 입니다 */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LockOn",
		CLockOn::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_LockOn");
		return E_FAIL;
	}


	/* For.GameObject_Planet */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Planet",
		CPlanet::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Planet");
		return E_FAIL;
	}

	/* For.GameObject_HP_Bar */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_HP_Bar",
		CHP_Bar::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_HP_Bar");
		return E_FAIL;
	}

	/* For.GameObject_HP_Bar_Border */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_HP_Bar_Border",
		CHP_Bar_Border::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_HP_Bar_Border");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_NaviArrow",
		CNaviArrow::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_NaviArrow");
		return E_FAIL;
	}

	/* For.GameObject_Stamina_Bar */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Stamina_Bar",
		CStamina_Bar::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Stamina_Bar");
		return E_FAIL;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* For.GameObject_Planet */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Asteroid",
		CAsteroid::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Asteroid");
		return E_FAIL;
	}

	/* For.GameObject_Drone */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Drone",
		CDrone::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Drone");
		return E_FAIL;
	}

#pragma endregion

#pragma region Components

	/* For.Component_Texture_Crosshair */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Crosshair",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Crosshair%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Crosshair");
		return E_FAIL;
	}

	/* For.Component_Texture_Crosshair_Missile */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Crosshair_Missile",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Crosshair_Missile%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Crosshair_Missile");
		return E_FAIL;
	}

	/* For.Component_Texture_Crosshair_Gatling */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Crosshair_Gatling",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Crosshair_Gatling%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Crosshair_Gatling");
		return E_FAIL;
	}

	/* For.Component_Texture_Crosshair_Dot */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Crosshair_Dot",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Crosshair_Dot%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Crosshair_Dot");
		return E_FAIL;
	}

	/* For.Component_Texture_AimAssist */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_AimAssist",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/AimAssist%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_AimAssist");
		return E_FAIL;
	}

	/* For.Component_Texture_AimAssist2 */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_AimAssist2",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/AimAssista%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_AimAssist2");
		return E_FAIL;
	}

	/* For.Component_Texture_LockOn */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_LockOn",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/LockOn%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_LockOn");
		return E_FAIL;
	}

	/* For.Component_Texture_Monster */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Monster",
		CTexture::Create(m_pDevice, ETextureType::Cube, L"../../Resources/Textures/Monster%d.dds", 2))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Monster");
		return E_FAIL;
	}


	/* For.Component_Texture_Skybox */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Skybox",
		CTexture::Create(m_pDevice, ETextureType::Cube, L"../../Resources/Textures/Skybox%d.dds", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Skybox");
		return E_FAIL;
	}
	/* For.Component_Texture_Skybox */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Skybox_Stage3",
		CTexture::Create(m_pDevice, ETextureType::Cube, L"../../Resources/Textures/Skybox_Stage3.dds", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Skybox_Stage3");
		return E_FAIL;
	}


	/* For.Component_Texture_TestCube */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_TestCube",
		CTexture::Create(m_pDevice, ETextureType::Cube, L"../../Resources/Textures/XYZ_Test_Cube%d.dds", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_TestCube");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_EnergyBall",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/EnergyBall%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_EnergyBall");
		return E_FAIL;
	}

	/* For.Component_Mesh_Earth */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Earth",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Planet/Earth.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Earth");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Planet",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/planet.X", L""))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	Ready_Stage1();

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Rock_Generic_001",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/Asteroid/rock_generic_001.X", L"../../Resources/Textures/Asteroid/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Rock_Generic_002",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/Asteroid/rock_generic_002.X", L"../../Resources/Textures/Asteroid/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Rock_Cloud",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/Asteroid/cloud.X", L"../../Resources/Textures/Asteroid/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_NaviArrow",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/naviArrow.X", L"../../Resources/Textures/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_NaviArrow");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Boss",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/boss.X", L"../../Resources/Textures/Boss/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_Boss");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Drone",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/drone.X", L"../../Resources/Textures/Enemy/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}
	/*로딩시간때문에 잠시 주석해놓음 지우지 말것!*/
	//if (FAILED(m_pManagement->Add_Component_Prototype(
	//	EResourceType::Static,
	//	L"Component_Mesh_Delivery",
	//	CModelMesh::Create(m_pDevice, L"../../Resources/Models/delivery.X", L"../../Resources/Textures/Delivery/"))))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pManagement->Add_Component_Prototype(
	//	EResourceType::Static,
	//	L"Component_Mesh_Enemy1",
	//	CModelMesh::Create(m_pDevice, L"../../Resources/Models/enemy1.X", L"../../Resources/Textures/Enemy/"))))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
	//	return E_FAIL;
	//}
	//if (FAILED(m_pManagement->Add_Component_Prototype(
	//	EResourceType::Static,
	//	L"Component_Mesh_Enemy2",
	//	CModelMesh::Create(m_pDevice, L"../../Resources/Models/enemy2.X", L"../../Resources/Textures/Enemy/"))))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
	//	return E_FAIL;
	//}

	Ready_HUD_Resources();
	Ready_StageEffect();
#pragma endregion

	return S_OK;
}


HRESULT CLoading::Ready_LobbyResources()
{

	m_pManagement->Clear_NonStatic_Resources();

	CStreamHandler::Load_PassData_Resource(L"../../Resources/Data/Lobby.txt", FALSE);

#pragma region GameObjects
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Lobby_Model",
		CLobbyModel::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Lobby_Skybox");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Lobby_Skybox",
		CSkybox::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Lobby_Skybox");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LobbyCam",
		CLobbyCam::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_MainCam");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LobbyUI",
		CLobbyUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_MainCam");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_GatchaBox",
		CGatchaBox::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_GatchaBox");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Product",
		CProduct::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Ring");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_StatusBoard",
		CStatusBoard::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Ring");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Status",
		CStatus::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Ring");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		CExplosionSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_ExplosionSystem");
		return E_FAIL;
	}
	/* For.GameObject_FollowSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		CFollowSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_FollowSystem");
		return E_FAIL;
	}

	/* For.GameObject_EngineEffectSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_EngineEffectSystem",
		CEngineEffectSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_EngineEffectSystem");
		return E_FAIL;
	}

	/* For.GameObject_WingBoostSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_WingBoostSystem",
		CWingBoost_System::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_WingBoostSystem");
		return E_FAIL;
	}
	// 스크립트 UI
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LobbyScriptUI",
		CLobbyScriptUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_ScriptUI");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LobbyBackUI",
		CLobbyBackUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_UI");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LobbyCursor",
		CLobbyCursor::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_UI");
		return E_FAIL;
	}

#pragma endregion
	
#pragma region Components
	// Script_UI blackbar
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_ScriptUI_BlackBar",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Script/BlackBar.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_ScriptUI_BlackBar");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_ScriptUI_Script",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Script/Script.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_ScriptUI_BlackBar");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_GeoMesh_Ring",
		CGeoMesh_Torus::Create(m_pDevice, 0.1f, 5.f)))) //도넛의 두께와 지름
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_GeoMesh_Player_Lazer");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_VIBuffer_HexagonTex",
		CVIBuffer_HexagonTex::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_HexagonColor");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Plasma",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/plasma.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Plasma");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Fire",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/fire.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Fire");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Boost",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/boost.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Boost");

		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Smoke",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/smoke.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Smoke");
		return E_FAIL;
	}
#pragma endregion
	return S_OK;
}

HRESULT CLoading::Ready_StageEffect()
{
#pragma region GameObjects
	/* For.GameObject_ExplosionSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		CExplosionSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_ExplosionSystem");
		return E_FAIL;
	}

	/* For.GameObject_LaserSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LaserSystem",
		CLaserSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_LaserSystem");
		return E_FAIL;
	}

	/* For.GameObject_FollowSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		CFollowSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_FollowSystem");
		return E_FAIL;
	}

	/* For.GameObject_EngineEffectSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_EngineEffectSystem",
		CEngineEffectSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_EngineEffectSystem");
		return E_FAIL;
	}

	/* For.GameObject_WingBoostSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_WingBoostSystem",
		CWingBoost_System::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_WingBoostSystem");
		return E_FAIL;
	}

#pragma endregion

#pragma region Components
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Fire",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/fire.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Fire");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Plasma",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/plasma.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Plasma");
		return E_FAIL;
	}
	
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Smoke",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/smoke.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Smoke");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Orange",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/Gatling.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Orange");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Boost",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/boost.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Boost");

		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Glow",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/glow.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Boost");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Spark",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/spark.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Wind");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Ring_Pass",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/Ring_Pass.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Ring_Pass");
		return E_FAIL;
	}
	
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Particle_Yellow",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/Particle_Yellow.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Particle_Yellow");
		return E_FAIL;
	}

	/* For.Component_Texture_Effect_Warp */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Effect_Damage",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD_Effect/damage.dds", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Warp");
		return E_FAIL;
	}

	/* For.Component_Texture_Effect_Warp */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Effect_Boost",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD_Effect/boost.dds", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Warp");
		return E_FAIL;
	}

#pragma endregion
	return S_OK;
}

HRESULT CLoading::Ready_HUD_Resources()
{

	/* For.Component_Texture_Machinegun_HUD */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Machinegun_HUD",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Machinegun%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Machinegun");
		return E_FAIL;
	}

	/* For.Component_Texture_Missile_HUD */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Missile_HUD",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Missile%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Missile");
		return E_FAIL;
	}

	/* For.Component_Texture_Laser_HUD */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Laser_HUD",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Laser%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Laser_HUD");
		return E_FAIL;
	}

	/* For.Component_Texture_Overdrive_HUD */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Overdrive_HUD",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Overdrive%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Overdrive_HUD");
		return E_FAIL;
	}

	/* For.Component_Texture_Shield_Battery_HUD */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Shield_Battery_HUD",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Shield_Battery%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Shield_Battery_HUD");
		return E_FAIL;
	}

	/* For.Component_Texture_HUD_Boarder */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HUD_Boarder",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HUD_Boarder%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HUD_Boarder");
		return E_FAIL;
	}

	/* For.Component_Texture_HUD_Shield */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HUD_Shield",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HUD_Shield.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HUD_Shield");
		return E_FAIL;
	}

	/* For.Component_Texture_HUD_Hp */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HUD_Hp",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HUD_Hp.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HUD_Hp");
		return E_FAIL;
	}

	/* For.Component_Texture_HUD_Out_Bar */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HUD_Out_Bar",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HUD_Out_Bar.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HUD_Out_Bar");
		return E_FAIL;
	}

	/* For.Component_Texture_HUD_Out_Bar */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HP_Border",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HUD_Out_Bar.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HP_Border");
		return E_FAIL;
	}

	/* For.Component_Texture_HUD_In_Bar */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HUD_In_Bar",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HUD_In_Bar.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HUD_In_Bar");
		return E_FAIL;
	}

	/* For.Component_Texture_HUD_HP_Bar */ //HP_Bar!! 있는데 뭐지
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HP_Bar",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HP/HP_Bar%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HP_Bar");
		return E_FAIL;
	}

	Ready_ScriptUI_Resources();

	return S_OK;
}

HRESULT CLoading::Ready_ScriptUI_Resources()
{
	// 스크립트 UI
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_ScriptUI",
		CScriptUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_ScriptUI");
		return E_FAIL;
	}

	// Script_UI blackbar
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_ScriptUI_BlackBar",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Script/BlackBar.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_ScriptUI_BlackBar");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_ScriptUI_Script",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Script/Script.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_ScriptUI_BlackBar");
		return E_FAIL;
	}


	// 초상화
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Portrait",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Portrait/Portrait%d.png",3))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_ScriptUI_Portrait_Test");
		return E_FAIL;
	}

	// 미션/퀘스트 UI
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_MissionUI",
		CMissionUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_MissionUI");
		return E_FAIL;
	}

	// 미션 텍스처
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HUD_Mission",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Mission/HUD_Mission.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Mission_HUD");
		return E_FAIL;
	}

	// 미션 텍스처1
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HUD_Mission_Name",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Mission/HUD_Mission_Name.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HUD_Mission_Name");
		return E_FAIL;
	}

	// 미션 텍스처2
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_HUD_Mission_Deco",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Mission/HUD_Mission_Deco.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HUD_Mission_Deco");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_BackUI",
		CBackUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_UI");
		return E_FAIL;
	}

	/* For.Component_Texture_Stamina_Bar */ 
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Stamina_Bar",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HP/Stamina_Bar%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Stamina_Bar");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading::Ready_Stage1()
{
	// 과녁
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_TargetMonster",
		CTargetMonster::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_TargetMonster");
		return E_FAIL;
	}

	// 과녁매쉬
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_GeoMesh_Cylinder",
		CGeoMesh_Cylinder::Create(m_pDevice, 5.f, 5.f, 0.01f))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_GeoMesh_Player_Lazer");
		return E_FAIL;
	}




	// 고리
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Ring",
		CRing::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Ring");
		return E_FAIL;
	}

	// 고리매쉬
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_GeoMesh_Ring",
		CGeoMesh_Torus::Create(m_pDevice, 1.f, 10.f)))) //도넛의 두께와 지름
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_GeoMesh_Player_Lazer");
		return E_FAIL;
	}


	// 고리 내비게이션
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_TutorialUI",
		CTutorialUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_TutorialUI");
		return E_FAIL;
	}
	// UI 이미지
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Tutorial_Nevi",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/IMG_HUD_Marker_Turret_Peripheral.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Planet_Jupiter");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Meteor",
		CMeteor::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Meteor");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLoading::Ready_Map_Effect_Resources()
{

	return S_OK;
}

HRESULT CLoading::Ready_BossAndOthers()
{

#pragma region GameObjects
	// boss
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Boss_Monster",
		CBoss_Monster::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_LaserSystem");
		return E_FAIL;
	}

	// others
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Bullet_EnergyBall",
		CBullet_EnergyBall::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Bullet_EnergyBall");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Bullet_Laser",
		CBullet_Laser::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Bullet_Laser");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Bullet_EMP_Bomb",
		CBullet_EMP_Bomb::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Bullet_EMP_Bomb");
		return E_FAIL;
	}


	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Boss_Warmhole",
		CBoss_Warmhole::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Boss_Warmhole");
		return E_FAIL;
	}

	//if (FAILED(m_pManagement->Add_Component_Prototype(
	//	EResourceType::NonStatic,
	//	L"Component_Texture_Billboard_Warmhole",
	//	CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Billboard_Warmhole%d.png"))))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Billboard_Warmhole");
	//	return E_FAIL;
	//}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Boss_Spawn_Monster",
		CBoss_Spawn_Monster::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Boss_Spawn_Monster");
		return E_FAIL;
	}

#pragma endregion

#pragma region Textures

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_BossLaser_Trail",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/BossLaser_Trail.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_BossLaser_Trail");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_BossLaser",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/BossLaser.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_BossLaser");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_BossLaserAlert",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/BossLaserAlert.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_BossLaserAlert");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Bullet_Dead",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/Bullet_Dead.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Bullet_Dead");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Fire_Effet",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/Fire_Effet.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Fire_Effet");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Dust_Effet",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/Dust_Effect.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Dust_Effet");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Bullet_Trail_Puple",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/Bullet_Trail_Puple.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Bullet_Trail_Puple");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Bullet_EnergyBall",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Bullet/Boss_EnergyBall.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Bullet_EnergyBall");
		return E_FAIL;
	}

#pragma endregion

#pragma region Component/Mesh



#pragma endregion

	return S_OK;
}
