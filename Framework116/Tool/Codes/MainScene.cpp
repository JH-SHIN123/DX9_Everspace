#include "pch.h"
#include "MainScene.h"
#include "Camera.h"

CMainScene::CMainScene(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CMainScene::Ready_Scene()
{
	CScene::Ready_Scene();

	//::SetWindowText(g_hWnd, L"MainScene");

	//if (FAILED(Add_Layer_Player(L"Layer_Player")))
	//	return E_FAIL;

	return S_OK;
}

_uint CMainScene::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);

	return _uint();
}

_uint CMainScene::LateUpdate_Scene(_float fDeltaTime)
{
	CScene::LateUpdate_Scene(fDeltaTime);

	return _uint();
}

HRESULT CMainScene::Add_Layer_Player(const wstring& LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_Player",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Player In Layer");
		return E_FAIL;
	}

	return S_OK;
}

CMainScene* CMainScene::Create(LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice)
	{
		PRINT_LOG(L"Error", L"Device is nullptr");
		return nullptr;
	}

	CMainScene* pInstance = new CMainScene(pDevice);
	if (FAILED(pInstance->Ready_Scene()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Stage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainScene::Free()
{
	CScene::Free(); // 2.何葛 府家胶 沥府	
}
