#include "..\Headers\Scene_Manager.h"
#include "Scene.h"
#include "Management.h"

USING(Engine)
IMPLEMENT_SINGLETON(CScene_Manager)

CScene_Manager::CScene_Manager()
{
}

HRESULT CScene_Manager::Setup_CurrentScene(_uint iNewSceneType, CScene * pNewScene)
{
	if (nullptr == pNewScene)
	{
		PRINT_LOG(L"Error", L"New Scene is nullptr");
		return E_FAIL;
	}

	if (iNewSceneType != m_iCurrentSceneType)
	{	
		Safe_Release(m_pCurrentScene);

		m_pCurrentScene = pNewScene;
		m_iCurrentSceneType = iNewSceneType;
	}

	return S_OK;
}

_uint CScene_Manager::Update_Scene(_float fDeltaTime)
{
	if (nullptr == m_pCurrentScene)
	{
		PRINT_LOG(L"Error", L"m_pCurrentScene is nullptr");
		return UPDATE_ERROR;
	}

	return m_pCurrentScene->Update_Scene(fDeltaTime);
}

_uint CScene_Manager::LateUpdate_Scene(_float fDeltaTime)
{
	if (nullptr == m_pCurrentScene)
	{
		PRINT_LOG(L"Error", L"m_pCurrentScene is nullptr");
		return UPDATE_ERROR;
	}

	return m_pCurrentScene->LateUpdate_Scene(fDeltaTime);
}

void CScene_Manager::Free()
{
	Safe_Release(m_pCurrentScene);
}

