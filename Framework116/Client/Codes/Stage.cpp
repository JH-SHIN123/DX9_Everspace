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

	UI_DESC uiDesc;
	uiDesc.tTransformDesc.vScale = { 150.f, 150.f,0.f };
	uiDesc.wstrTexturePrototypeTag = L"Component_Texture_Grass";
	if (FAILED(Add_Layer_UI(L"Layer_UI", &uiDesc)))
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

	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Grass";
	pSystemDesc.iNumParticles = 500;
	pSystemDesc.tResetAttribute.fParticleSize = 0.9f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 50.f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.f;
	if(FAILED(Add_Layer_ExplosionSystem(L"Layer_ExplosionSystem", &pSystemDesc)))
		return E_FAIL;

	pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Grass";
	pSystemDesc.tResetAttribute.fParticleSize = 0.9f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 100.f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;
	if (FAILED(Add_Layer_LaserSystem(L"Layer_LaserSystem", &pSystemDesc)))
		return E_FAIL;

	return S_OK;
}

_uint CStage::Update_Scene(_float fDeltaTime)
{
	CScene::Update_Scene(fDeltaTime);

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		PARTICLESYSTEM_DESC pSystemDesc;
		pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Grass";
		pSystemDesc.iNumParticles = 500;
		pSystemDesc.tResetAttribute.fParticleSize = 0.9f;
		pSystemDesc.tResetAttribute.fParticleSpeed = 50.f;
		pSystemDesc.tResetAttribute.fLifeTime = 2.f;
		if (FAILED(Add_Layer_ExplosionSystem(L"Layer_Particle_Explosion", &pSystemDesc)))
			return E_FAIL;

	}

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

HRESULT CStage::Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc)
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

HRESULT CStage::Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc)
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

HRESULT CStage::Add_Layer_ExplosionSystem(const wstring& LayerTag, const PARTICLESYSTEM_DESC* pParticleSystemDesc)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		LayerTag,
		(void*)pParticleSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage::Add_Layer_LaserSystem(const wstring& LayerTag, const PARTICLESYSTEM_DESC* pParticleSystemDesc)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_LaserSystem",
		LayerTag,
		(void*)pParticleSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
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
