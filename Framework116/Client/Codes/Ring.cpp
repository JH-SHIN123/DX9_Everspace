#include "stdafx.h"
#include "..\Headers\Ring.h"

#include "Player.h"

CRing::CRing(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	ZeroMemory(&m_tMaterial, sizeof(D3DMATERIAL9));
}

CRing::CRing(const CRing & other)
	: CGameObject(other)
	, m_bHitRing(other.m_bHitRing)
	, m_tMaterial(other.m_tMaterial)
{

	m_tMaterial.Diffuse.r = m_tMaterial.Ambient.r = m_tMaterial.Specular.r = m_tMaterial.Emissive.r = 0.9f;
	m_tMaterial.Diffuse.g = m_tMaterial.Ambient.g = m_tMaterial.Specular.g = m_tMaterial.Emissive.g = 0.2f;
	m_tMaterial.Diffuse.b = m_tMaterial.Ambient.b = m_tMaterial.Specular.b = m_tMaterial.Emissive.b = 0.5f;
	m_tMaterial.Diffuse.a = m_tMaterial.Ambient.a = m_tMaterial.Specular.a = m_tMaterial.Emissive.a = 0.f;
	m_tMaterial.Power = 1000.f;

}

HRESULT CRing::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CRing::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_GeoMesh_Ring",
		L"Com_GeoMesh",
		(CComponent**)&m_pGeoMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Ring",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	//TransformDesc.vPosition = ((TRANSFORM_DESC*)pArg)->vPosition;
	//TransformDesc.vRotate = ((TRANSFORM_DESC*)pArg)->vRotate;
	TransformDesc.vPosition = { 0.f,3.f,-30.f };

	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(80.f);
	TransformDesc.vScale = { 1.f, 1.f, 1.f };

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

	//m_pTerrainBuffer = (CVIBuffer_TerrainTexture*)m_pManagement->Get_Component(L"Layer_Terrain", L"Com_VIBuffer");
	//Safe_AddRef(m_pTerrainBuffer);
	//if (nullptr == m_pTerrainBuffer)
	//{
	//	PRINT_LOG(L"Error", L"m_pTerrainBuffer is nullptr");
	//	return E_FAIL;
	//}

	// For.Com_Collide
	BOUNDING_SPHERE BoundingSphere;
	BoundingSphere.fRadius = 8.f;

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_CollideSphere",
		L"Com_CollideSphere",
		(CComponent**)&m_pCollide,
		&BoundingSphere,
		true)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	// For Target's Collide
	//m_TargetCollide = (COLLIDES*)m_pManagement->Get_GameObject(L"Layer_Player")->Get_Collides();

	return S_OK;
}

_uint CRing::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	//Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CRing::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	CollideCheck();
	//if (CollideCheck())
		//return DEAD_OBJECT;

	return _uint();
}

_uint CRing::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pDevice->SetMaterial(&m_tMaterial);
	m_pGeoMesh->Render_Mesh();
	// Test

#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CRing::Movement(_float fDeltaTime)
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

_bool CRing::CollideCheck()
{
	if (m_IsCollide == true)
	{
		
		
	}

	return false;
}

CRing * CRing::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CRing* pInstance = new CRing(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRing::Clone(void * pArg/* = nullptr*/)
{
	CRing* pClone = new CRing(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CRing::Free()
{
	//for (auto& Collide : m_TargetCollide)
	//{
	//	Safe_Release(Collide);
	//}
	//m_TargetCollide.clear();

	Safe_Release(m_pGeoMesh);
	//Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
