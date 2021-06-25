#include "stdafx.h"
#include "..\Headers\Stage.h"
#include "Camera.h"

CStage::CStage(LPDIRECT3DDEVICE9 pDevice)
	: CScene(pDevice)
{
}

HRESULT CStage::Ready_Scene()
{
	CScene::Ready_Scene();

	::SetWindowText(g_hWnd, L"Stage");

	if (FAILED(Add_Layer_Terrain(L"Layer_Terrain")))
		return E_FAIL;

	if (FAILED(Add_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Add_Layer_Cam(L"Layer_Cam")))
		return E_FAIL;

	if (FAILED(Add_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	if (FAILED(Add_Layer_Skybox(L"Layer_Skybox")))
		return E_FAIL;

	TRANSFORM_DESC uiTransformDesc;
	uiTransformDesc.vScale = { 150.f, 150.f,0.f };
	if (FAILED(Add_Layer_UI(L"Layer_UI", uiTransformDesc, L"Component_Texture_Grass")))
		return E_FAIL;

	if (FAILED(Add_Layer_DirectionalLight(L"Layer_DirectionalLight", { 1.0f, -0.0f, 0.25f }, D3DCOLOR_XRGB(255, 255, 255))))
		return E_FAIL;

	return S_OK;
}

_uint CStage::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);

	return _uint();
}

_uint CStage::LateUpdate_Scene(_float fDeltaTime)
{
	CScene::LateUpdate_Scene(fDeltaTime);

	return _uint();
}

HRESULT CStage::Add_Layer_Player(const wstring & LayerTag)
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

HRESULT CStage::Add_Layer_Terrain(const wstring & LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Terrain",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Player In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage::Add_Layer_Cam(const wstring & LayerTag)
{
	CAMERA_DESC CameraDesc;
	CameraDesc.fFovY = D3DXToRadian(90.f);
	CameraDesc.fAspect = (_float)WINCX / WINCY;
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

HRESULT CStage::Add_Layer_Monster(const wstring & LayerTag)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Monster",
		LayerTag)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Monster In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage::Add_Layer_Grass(const wstring & LayerTag)
{
	// MT19937 난수 엔진
	mt19937 engine((unsigned int)time(NULL));                

	// 생성 범위
	uniform_real_distribution<_float> distribution(0.f, 128.f);
	auto generator = bind(distribution, engine);

	TRANSFORM_DESC TransformDesc;
	for (_uint i = 0; i < 500; ++i)
	{		
		TransformDesc.vPosition.x = generator();
		TransformDesc.vPosition.z = generator();

		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Grass",
			LayerTag, 
			&TransformDesc)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Grass In Layer");
			return E_FAIL;
		}
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

HRESULT CStage::Add_Layer_UI(const wstring& LayerTag, const TRANSFORM_DESC& tTransformDesc, const wstring& wstrTexturePrototypeTag)
{
	UI_DESC uiDesc;
	uiDesc.tTransformDesc = tTransformDesc;
	uiDesc.wstrTexturePrototypeTag = wstrTexturePrototypeTag;

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_UI",
		LayerTag,
		&uiDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage::Add_Layer_DirectionalLight(const wstring& LayerTag, const _float3 vDir, const D3DXCOLOR tColor)
{
	LIGHT_DESC lightDesc;
	lightDesc.eLightType = ELightType::Directional;
	lightDesc.vLightDir = vDir;
	lightDesc.tLightColor = tColor;

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_DirectionalLight",
		LayerTag,
		&lightDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Directional Light In Layer");
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
