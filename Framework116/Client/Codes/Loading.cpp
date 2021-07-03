
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
#include "Crosshair.h"
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

	/* For.GameObject_ExplosionSystem */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		CExplosionSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_ExplosionSystem");
		return E_FAIL;
	}

	/* For.GameObject_Particle_Laser */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_LaserSystem",
		CLaserSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_LaserSystem");
		return E_FAIL;
	}


	/* 임시 Boss_Monster 입니다 */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Boss_Monster",
		CBoss_Monster::Create(m_pDevice, nullptr))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_LaserSystem");
		return E_FAIL;
	}

	/*  HUD Crosshair 입니다 */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Crosshair",
		CCrosshair::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Crosshair");
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
		CTexture::Create(m_pDevice, ETextureType::Cube, L"../../Resources/Textures/Skybox%d.dds",1))))
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

	Ready_HUD_Resources();

#pragma endregion

	//CStreamHandler::Load_PassData_Object(L"../../Data/PrototypeData/TestSaveFile.object");



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

	return S_OK;
}
