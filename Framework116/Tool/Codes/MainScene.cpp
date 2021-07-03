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

	if (FAILED(Add_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Add_Layer_Cam(L"Layer_Cam")))
		return E_FAIL;
	
	if (FAILED(Add_Layer_Axis(L"Layer_Axis")))
		return E_FAIL;

		// 우주에서 태양광을 표현하기 위해선
	// 포인트라이트 혹은 스포트라이트가 더 어울릴듯
	LIGHT_DESC lightDesc;
	lightDesc.eLightType = ELightType::Directional;
	lightDesc.vLightDir = { 1.0f, -0.0f, 0.25f };
	lightDesc.tLightColor = D3DCOLOR_XRGB(255, 255, 255);
	lightDesc.iLightIndex = 0;
	if (FAILED(Add_Layer_Light(L"Layer_DirectionalLight", &lightDesc)))
		return E_FAIL;

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

HRESULT CMainScene::Add_Layer_Cam(const wstring& LayerTag)
{
	CAMERA_DESC CameraDesc;
	CameraDesc.fFovY = D3DXToRadian(90.f);
	CameraDesc.fAspect = (_float)MAINVIEW_WINCX / MAINVIEW_WINCY;
	CameraDesc.fNear = 1.f;
	CameraDesc.fFar = 1000.f;

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

HRESULT CMainScene::Add_Layer_Axis(const wstring& LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_Axis",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Axis In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMainScene::Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_DirectionalLight",
		LayerTag,
		(void*)pLightDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Directional Light In Layer");
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
	CScene::Free(); // 2.부모 리소스 정리	
}
