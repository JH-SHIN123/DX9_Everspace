
#include "stdafx.h"
#include "..\Headers\Loading.h"

#pragma region Scenes
#include"Lobby.h"
#include "Stage.h"
#include "Stage2.h"
#include "Stage3.h"
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
#include "FollowSystem.h"
#include "EngineEffectSystem.h"
#include "New_LockOn.h"
#include "Planet.h"
#include "LoopExplosionSystem.h"
#include "Drone.h"
#include "WingBoost_System.h"
#include"Product.h"
#include"StatusBoard.h"
#include"Status.h"
#include"VIBuffer_HexagonTex.h"
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
#include "BrokenPlane.h"

// 3Stage필요
#include "Sniper.h"
#include "Sniper_Bullet.h"
#include "LockOnAlert.h"
#include "Delivery.h"
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
	m_pManagement->StopSound(CSoundMgr::BGM);

	// 클론들 비우기
	m_pManagement->Clear_NonStatic_Resources();

	// 씬 진입전 로딩쓰레드 생성
	m_hLoadingThread = (HANDLE)_beginthreadex(0, 0, ThreadMain, this, 0, 0);
	if (nullptr == m_hLoadingThread)
	{
		PRINT_LOG(L"Error", L"Failed To _beginthreadex");
		return E_FAIL;
	}
	InitializeCriticalSection(&m_CriticalSection);

	return S_OK;
}

_uint CLoading::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);
	m_pManagement->PlaySound(L"Loading_Ambience.ogg", CSoundMgr::BGM);

	// 로딩 끝
	if (m_IsFinished)
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
			if (m_pLoadingUI) m_pLoadingUI->Set_IsDead(true);
			Safe_Release(m_pLoadingUI);
			if (m_pLoadingUIIcon) m_pLoadingUIIcon->Set_IsDead(true);
			Safe_Release(m_pLoadingUIIcon);
			
			CScene* pNextScene = nullptr;
			switch (m_eNextSceneID)
			{
			case ESceneType::Lobby:
				pNextScene = CLobby::Create(m_pDevice);
				break;
			case ESceneType::Stage:
				pNextScene = CStage::Create(m_pDevice);
				break;
			case ESceneType::Stage2:
				pNextScene = CStage2::Create(m_pDevice);
				break;
			case ESceneType::Stage3:
				pNextScene = CStage3::Create(m_pDevice);
				break;
			}

			if (FAILED(m_pManagement->Setup_CurrentScene((_uint)m_eNextSceneID, pNextScene)))
			{
				PRINT_LOG(L"Error", L"Failed To Setup Next Scene");
				return UPDATE_ERROR;
			}

			return CHANGE_SCENE;
			m_bLeaveScene = false;
		}
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
	
	m_pManagement->StopAll();


	CScene::Free(); // 2.부모 리소스 정리

}

unsigned CLoading::ThreadMain(void * pArg)
{
	CLoading* pLoading = (CLoading*)pArg;
	EnterCriticalSection(&pLoading->m_CriticalSection);

	HRESULT hr = 0;
	switch (pLoading->m_eNextSceneID)
	{
	case ESceneType::Lobby:
		hr = pLoading->Ready_LobbyResources();
		break;
	case ESceneType::Stage:
		hr = pLoading->Ready_StageResources();
		break;
	case ESceneType::Stage2:
		hr = pLoading->Ready_Stage2Resources();
		break;
	case ESceneType::Stage3:
		hr = pLoading->Ready_Stage3Resources();
		break;
	default:
		break;
	}

	if (FAILED(hr))
	{
		CManagement::Get_Instance()->Clear_NonStatic_Resources();

		LeaveCriticalSection(&pLoading->m_CriticalSection);
		return LOADING_ERROR;
	}

	pLoading->m_IsFinished = true;
	LeaveCriticalSection(&pLoading->m_CriticalSection);

	return NO_EVENT;
}

HRESULT CLoading::Ready_LobbyResources()
{
	m_pManagement->Clear_NonStatic_Resources();
	Ready_LoadingResources();
	
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
	/* For.Component_Texture_Skybox */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Skybox_Stage3",
		CTexture::Create(m_pDevice, ETextureType::Cube, L"../../Resources/Textures/Skybox_Stage3.dds", 1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Skybox_Stage3");
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

#pragma region GatchaEffect
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Effect_Party",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/party.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_ScriptUI_BlackBar");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CLoading::Ready_StageResources()
{
	m_pManagement->Clear_NonStatic_Resources();
	Ready_LoadingResources();

	// Stage 공통 로드 리소스
	Load_HUD_Resources();
	Load_ScriptUI_Resources();
	Load_StageEffect_Resources();

	Load_Stage1_Prop_Resources();

	return S_OK;
}

HRESULT CLoading::Ready_Stage2Resources()
{
	m_pManagement->Clear_NonStatic_Resources();
	Ready_LoadingResources();

	Load_HUD_Resources();
	Load_ScriptUI_Resources();
	Load_StageEffect_Resources();

	Load_Stage2_Prop_Resources();
	Load_Stage3_Prop_Resources();

	return S_OK;
}

HRESULT CLoading::Ready_Stage3Resources()
{
	m_pManagement->Clear_NonStatic_Resources();
	Ready_LoadingResources();

	Load_HUD_Resources();
	Load_ScriptUI_Resources();
	Load_StageEffect_Resources();

	Load_Stage3_Prop_Resources();

	return S_OK;
}

HRESULT CLoading::Load_Stage1_Prop_Resources()
{
#pragma region Map

#pragma region GameObjects
	/* For.GameObject_Skybox */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Skybox",
		CSkybox::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Skybox");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Planet",
		CPlanet::Create(m_pDevice, EPlanetType::Basic))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Planet");
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

	/* For.GameObject_Planet */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Asteroid",
		CAsteroid::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Asteroid");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Ring",
		CRing::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Ring");
		return E_FAIL;
	}
#pragma endregion

#pragma region Components
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

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Planet",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/planet.X", L""))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Planet_Basic",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Planet/Basic.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Earth");
		return E_FAIL;
	}

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

	// 고리매쉬
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_GeoMesh_Ring",
		CGeoMesh_Torus::Create(m_pDevice, 1.f, 10.f)))) //도넛의 두께와 지름
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_GeoMesh_Player_Lazer");
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
#pragma endregion

#pragma endregion

#pragma region Drone
#pragma region GameObjects
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
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Drone",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/drone.X", L"../../Resources/Textures/Enemy/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}
#pragma endregion

#pragma endregion

	return S_OK;
}

HRESULT CLoading::Load_Stage2_Prop_Resources()
{

#pragma region Map
#pragma region GameObject
	/* For.GameObject_Skybox */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Skybox",
		CSkybox::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Skybox");
		return E_FAIL;
	}

	/* For.GameObject_Planet_Gas */
	// 맵정보 잘못들어감 (실제로 가스행성임)
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Planet_Ice",
		CPlanet::Create(m_pDevice, EPlanetType::Gas))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Planet");
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

	/* For.GameObject_Planet */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Asteroid",
		CAsteroid::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Asteroid");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Broken_Plane",
		CBrokenPlane::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Broken_Plane");
		return E_FAIL;
	}

#pragma endregion

#pragma region Components
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

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Planet",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/planet.X", L""))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	/* For.Component_Texture_Planet */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Planet_Gas",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Planet/Gas.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Earth");
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
		L"Component_Mesh_Broken_Plane",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/broken.X", L"../../Resources/Textures/Broken/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}
#pragma endregion

#pragma endregion

#pragma region Monster
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
#pragma endregion

#pragma region Component
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Enemy1",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/enemy1.X", L"../../Resources/Textures/Enemy/"))))
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

#pragma endregion
#pragma endregion

	return S_OK;
}

HRESULT CLoading::Load_Stage3_Prop_Resources()
{
#pragma region Map

#pragma region GameObjects
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Skybox",
		CSkybox::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Skybox");
		return E_FAIL;
	}


	/* For.GameObject_Planet_Ice */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Planet_Ice",
		CPlanet::Create(m_pDevice, EPlanetType::Ice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Planet");
		return E_FAIL;
	}

	/* For.GameObject_Planet */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Asteroid",
		CAsteroid::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Asteroid");
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

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Delivery",
		CDelivery::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_NaviArrow");
		return E_FAIL;
	}
#pragma endregion

#pragma region Components
	/* For.Component_Texture_Planet */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Planet_Ice",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Planet/Ice.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Earth");
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
		L"Component_Mesh_Planet",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/planet.X", L""))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Delivery",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/delivery.X", L"../../Resources/Textures/Delivery/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

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
		L"Component_Mesh_Ice_Generic_001",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/Asteroid/ice_generic_001.X", L"../../Resources/Textures/Asteroid/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Ice_Generic_002",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/Asteroid/ice_generic_002.X", L"../../Resources/Textures/Asteroid/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}
#pragma endregion


#pragma endregion

#pragma region Boss
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
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Boss_Spawn_Monster",
		CBoss_Spawn_Monster::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Boss_Spawn_Monster");
		return E_FAIL;
	}

#pragma region Bullets
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
#pragma endregion

#pragma endregion

#pragma region Components
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Boss",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/boss.X", L"../../Resources/Textures/Boss/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_Boss");
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

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_BossEMP_Explosion",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/BossEMP_Explosion.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_BossEMP_Explosion");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_BossEMP_2",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/BossEMP_2.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_BossEMP_2");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_BossEMP",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/BossEMP.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_BossEMP");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_BossLaser_Fire",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/BossLaser_Fire.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_BossLaser_Fire");
		return E_FAIL;
	}

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

	///////////////////////////////////////////////////////////////
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
#pragma endregion

#pragma region Sniper
#pragma region GameObjects
	/* For.GameObject_Sniper */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Sniper",
		CSniper::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Sniper");
		return E_FAIL;
	}
	// For Sniper Bullet
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Sniper_Bullet",
		CSniper_Bullet::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Sniper_Bullet");
		return E_FAIL;
	}
#pragma endregion

#pragma region Components
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Enemy2",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/enemy2.X", L"../../Resources/Textures/Enemy/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Sniper_Bullet",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Bullet/Sniper_Bullet.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Sniper_Bullet");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Sniper_Bullet_Trail",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Effect/Sniper_Bullet_Trail.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Sniper_Bullet_Trail");
		return E_FAIL;
	}
#pragma endregion
#pragma endregion

#pragma region Monster
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
#pragma endregion

#pragma region Component
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Enemy1",
		CModelMesh::Create(m_pDevice, L"../../Resources/Models/enemy1.X", L"../../Resources/Textures/Enemy/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}
#pragma endregion
#pragma endregion


	return S_OK;
}

HRESULT CLoading::Load_HUD_Resources()
{
#pragma region GameObjects
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
		CNew_LockOn::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_LockOn");
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

	/* For.GameObject_LockOnAlert */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LockOnAlert",
		CLockOnAlert::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_LockOnAlert");
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

	/* For.GameObject_Stamina_Bar */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Stamina_Bar",
		CStamina_Bar::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Stamina_Bar");
		return E_FAIL;
	}
#pragma endregion

#pragma region Components
	/* For.Component_Texture_Stamina_Bar */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Stamina_Bar",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HP/Stamina_Bar%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Stamina_Bar");
		return E_FAIL;
	}
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

	/* For.Component_Texture_LockOnAlert */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_LockOnAlert",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/LockOnAlert.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_LockOnAlert");
		return E_FAIL;
	}

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


	/* For.Component_Texture_Head_Up_Display */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Head_Up_Display",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HeadUpDisplay.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Head_Up_Display");
		return E_FAIL;
	}

	/* For.Component_Texture_Head_Up_Display2 */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Head_Up_Display2",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/HeadUpDisplay2.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Head_Up_Display2");
		return E_FAIL;
	}

#pragma endregion

	return S_OK;
}

HRESULT CLoading::Load_ScriptUI_Resources()
{
#pragma region GameObjects
	// 스크립트 UI
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_ScriptUI",
		CScriptUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_ScriptUI");
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
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_BackUI",
		CBackUI::Create(m_pDevice))))
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
	// 초상화
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Portrait",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/HUD/Portrait/Portrait%d.png", 5))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_ScriptUI_Portrait_Test");
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
#pragma endregion

	return S_OK;
}

HRESULT CLoading::Load_StageEffect_Resources()
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

	/* For.GameObject_ExplosionSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LoopExplosionSystem",
		CLoopExplosionSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_LoopExplosionSystem");
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


HRESULT CLoading::Ready_LoadingResources()
{
	UI_DESC uiDesc;
	uiDesc.tTransformDesc.vScale = { WINCX * 1.2f, WINCY, 0.f };
	uiDesc.wstrTexturePrototypeTag = L"Component_Texture_Loading";
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_LoadingUI",
		L"Layer_LoadingUI",
		(void*)&uiDesc,
		&m_pLoadingUI)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_LoadingIcon",
		L"Layer_LoadingUI",
		nullptr,
		&m_pLoadingUIIcon)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}


	return S_OK;
}
