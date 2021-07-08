#include "stdafx.h"
#include "..\Headers\Planet.h"
#include "MaterialHandler.h"

CPlanet::CPlanet(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CPlanet::CPlanet(const CPlanet & other)
	: CGameObject(other)
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
		L"Component_Texture_Earth",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = _float3(200.f, 0.f, 200.f);
	TransformDesc.vScale = _float3(1.f, 1.f, 1.f);
	if (pArg != nullptr)
	{
		TransformDesc.vPosition = ((TRANSFORM_DESC*)pArg)->vPosition;
		TransformDesc.vRotate = ((TRANSFORM_DESC*)pArg)->vRotate;
	}
	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(80.f);

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
	BoundingSphere.fRadius = 100000.f;
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

	return S_OK;
}

_uint CPlanet::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

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

	m_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture();
	m_pModelMesh->Render_Mesh();

#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	m_pDevice->SetRenderState(D3DRS_LIGHTING, true);


	return _uint();
}

_uint CPlanet::Movement(_float fDeltaTime)
{
	//_float3 vOutPos = m_pTransform->Get_State(EState::Position);
	//if (true == m_pTerrainBuffer->Is_OnTerrain(&vOutPos))
	//{
	//	vOutPos.y += 0.5f;
	//	m_pTransform->Set_Position(vOutPos);
	//}	


	m_pTransform->Go_Straight(fDeltaTime);


	return _uint();
}


CPlanet* CPlanet::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CPlanet* pInstance = new CPlanet(pDevice);
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
