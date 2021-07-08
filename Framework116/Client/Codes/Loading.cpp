
#include "stdafx.h"
#include "..\Headers\Loading.h"

#pragma region Scenes
#include "Stage.h"
#pragma endregion

#pragma region GameObjects
#include "Terrain.h"
#include "Monster.h"
#include "Grass.h"
#include "Skybox.h"
#include "ExplosionSystem.h"
#include "LaserSystem.h"
#include "Boss_Monster.h"
#include "Bullet_EnergyBall.h"
#include "Bullet_Laser.h"
#include "Bullet_EMP_Bomb.h"
#include "Crosshair.h"
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
#include "WingBoost_System.h"
#include "HP_Bar.h"
#include "Asteroid.h"
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

	InitializeCriticalSection(&m_CriticalSection);

	return S_OK;
}

_uint CLoading::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);

	if (m_IsFinished)
	{
		//

		CScene* pNextScene = nullptr;
		switch (m_eNextSceneID)
		{
		case ESceneType::Stage:
			pNextScene = CStage::Create(m_pDevice);
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
	/* For.GameObject_Terrain */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Terrain",
		CTerrain::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Terrain");
		return E_FAIL;
	}

	/* For.GameObject_Monster */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Monster",
		CMonster::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Monster");
		return E_FAIL;
	}

	/* For.GameObject_Grass */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Grass",
		CGrass::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Grass");
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


	/*  HUD Crosshair 입니다 */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Crosshair",
		CCrosshair::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Crosshair");
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

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Asteroid",
		CAsteroid::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Asteroid");
		return E_FAIL;
	}

#pragma endregion

#pragma region Components
	/* For.Component_VIBuffer_TerrainColor */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_VIBuffer_TerrainColor",
		CVIBuffer_TerrainColor::Create(m_pDevice, 129, 129))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_TerrainColor");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_TerrainTexture */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_VIBuffer_TerrainTexture",
		CVIBuffer_TerrainTexture::Create(m_pDevice, 129, 129,
			1.f, L"../../Resources/Textures/Terrain/Height.bmp"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_TerrainTexture");
		return E_FAIL;
	}

	/* For.Component_Texture_Terrain */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Terrain",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Terrain/Terrain%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Terrain");
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

	/* For.Component_Texture_Grass */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Grass",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/BillboardGrass%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Grass");
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

	//For.Component_Stage_Texture
	CStreamHandler::Load_PassData_Resource(L"../../Resources/Data/Stage.txt", FALSE);


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

	//if (FAILED(m_pManagement->Add_Component_Prototype(
	//	EResourceType::Static,
	//	L"Component_Mesh_Monster",
	//	CModelMesh::Create(m_pDevice, L"../../Resources/Models/monster.X", L"../../Resources/Textures/Monster/"))))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_Monster");
	//	return E_FAIL;
	//}

	Ready_HUD_Resources();
	Ready_StageEffect();
#pragma endregion

	//CStreamHandler::Load_PassData_Object(L"../../Data/PrototypeData/TestSaveFile.object");


	Ready_Stage1();

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

	// 테스트용 이미지
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Ring",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Player/Player1.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_HUD_In_Bar");
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
	return S_OK;
}

HRESULT CLoading::Ready_Map_Effect_Resources()
{

	return S_OK;
}

HRESULT CLoading::Ready_BossAndOthers()
{
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

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Billboard_Warmhole",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Textures/Billboard_Warmhole%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Billboard_Warmhole");
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

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Boss_Spawn_Monster",
		CBoss_Spawn_Monster::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Boss_Spawn_Monster");
		return E_FAIL;
	}

	return S_OK;
}
