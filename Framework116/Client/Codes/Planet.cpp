#include "stdafx.h"
#include "..\Headers\Planet.h"
#include "MaterialHandler.h"

CPlanet::CPlanet(LPDIRECT3DDEVICE9 pDevice, const EPlanetType ePlanentType)
	: CGameObject(pDevice)
	, m_ePlanetType(ePlanentType)
{
}

CPlanet::CPlanet(const CPlanet & other)
	: CGameObject(other),
	m_ePlanetType(other.m_ePlanetType)
{
}


HRESULT CPlanet::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPlanet::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);

	GAMEOBJECT_DESC* pDesc = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (pDesc = dynamic_cast<GAMEOBJECT_DESC*>(ptr))
		{
		}
		else
		{
			PRINT_LOG(L"Error", L"ASEROID_DESC is nullptr");
			return E_FAIL;
		}
	}

	wstring texTag = L"";
	switch (m_ePlanetType)
	{
	case EPlanetType::Basic:
		texTag = L"Component_Texture_Planet_Basic";
		break;
	case EPlanetType::Ice:
		texTag = L"Component_Texture_Planet_Ice";
		break;
	case EPlanetType::Gas:
		texTag = L"Component_Texture_Planet_Gas";
		break;
	}

	// For.Com_Geo_Sphere
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_Planet",
		L"Com_GeoMesh",
		(CComponent**)&m_pModelMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Component_GeoMesh_Sphere");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		texTag,
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc = pDesc->tTransformDesc;
	TransformDesc.fRotatePerSec = D3DXToRadian(20.f);

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		&TransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	// For.Com_Collide
	BOUNDING_SPHERE BoundingSphere;
	BoundingSphere.fRadius = 100.f;
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_CollideSphere",
		L"Com_CollideSphere",
		(CComponent**)&m_pCollide,
		&BoundingSphere,
		true)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Collide");
		return E_FAIL;
	}

	// Point Light 추가
	LIGHT_DESC lightDesc;
	lightDesc.eLightType = ELightType::PointLight;
	lightDesc.tLightColor = D3DCOLOR_XRGB(255, 255, 255);
	lightDesc.tTransformDesc.vPosition = TransformDesc.vPosition;
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_Light",
		L"Layer_Light",
		(void*)&lightDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Light In Layer");
		return E_FAIL;
	}

	// 스크립트 관련
	m_IsMoving = true;

	return S_OK;
}

_uint CPlanet::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);
	
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return _uint();
}

_uint CPlanet::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CPlanet::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture();
	m_pModelMesh->Render_Mesh();
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
#endif

	m_pDevice->SetRenderState(D3DRS_LIGHTING, true);
	m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	return _uint();
}

void CPlanet::Set_IsMove(_bool IsMove)
{
	m_IsMoving = IsMove;
}

_uint CPlanet::Movement(_float fDeltaTime)
{
	if (m_IsMoving == false)
		return 0;

	if (m_pTransform) {
		m_pTransform->RotateY(fDeltaTime);
	}


	return _uint();
}


CPlanet* CPlanet::Create(LPDIRECT3DDEVICE9 pDevice, const EPlanetType ePlanentType)
{
	CPlanet* pInstance = new CPlanet(pDevice, ePlanentType);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlanet::Clone(void* pArg)
{
	CPlanet* pClone = new CPlanet(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlanet::Free()
{
	Safe_Release(m_pModelMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
