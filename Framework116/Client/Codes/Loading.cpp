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
#include "Explosion.h"
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

	/* For.GameObject_Particle_Explosion */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::NonStatic,
		L"GameObject_Particle_Explosion",
		CExplosionSystem::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Particle_Explosion");
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
		CVIBuffer_TerrainTexture::Create(m_pDevice, 129, 129))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_TerrainTexture");
		return E_FAIL;
	}

	/* For.Component_Texture_Terrain */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Terrain",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../Resources/Terrain/Terrain%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Terrain");
		return E_FAIL;
	}

	/* For.Component_Texture_Monster */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Monster",
		CTexture::Create(m_pDevice, ETextureType::Cube, L"../Resources/Monster%d.dds", 2))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Monster");
		return E_FAIL;
	}

	/* For.Component_Texture_Grass */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Grass",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../Resources/BillboardGrass%d.png"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Grass");
		return E_FAIL;
	}

	/* For.Component_Texture_Skybox */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_Texture_Skybox",
		CTexture::Create(m_pDevice, ETextureType::Normal, L"../../Resources/Skybox%d.dds",1))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Skybox");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}
