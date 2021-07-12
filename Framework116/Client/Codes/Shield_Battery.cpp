#include "stdafx.h"
#include "..\Headers\Shield_Battery.h"
#include "MaterialHandler.h"
#include "Player.h"

CShield_Battery::CShield_Battery(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CShield_Battery::CShield_Battery(const CShield_Battery & other)
	: CGameObject(other)
{
}


HRESULT CShield_Battery::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CShield_Battery::Ready_GameObject(void* pArg)
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

	//// For.Com_Texture
	//if (FAILED(CGameObject::Add_Component(
	//	EResourceType::NonStatic,
	//	L"Component_Texture_Earth",
	//	L"Com_Texture",
	//	(CComponent**)&m_pTexture)))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
	//	return E_FAIL;
	//}

	// For.Com_Transform
	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player",L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);

	TRANSFORM_DESC TransformDesc;
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
	BoundingSphere.fRadius = 0.f;
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

	return S_OK;
}

_uint CShield_Battery::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	Movement(fDeltaTime);

	if (m_fRadian >= 0.105f)
	{
		m_IsAdd = false;
	}

	if (m_IsAdd)
		m_fRadian += 0.002f;


	if (!m_IsAdd)
	{
		m_fRadian -= 0.0005f;
		m_fLifeTime += fDeltaTime;

		if (m_fLifeTime > 3.6f)
		{
			// IsShield를 False로!
			((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsShield(false);
			return DEAD_OBJECT;
		}
	}


	m_pCollide->Resize_Shpere(m_fRadian);
	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return _uint();
}

_uint CShield_Battery::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CShield_Battery::Render_GameObject()
{
	CGameObject::Render_GameObject();

	//m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	//m_pTexture->Set_Texture();
	//m_pModelMesh->Render_Mesh();
	//m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CShield_Battery::Movement(_float fDeltaTime)
{

	m_vPos = m_pPlayerTransform->Get_State(EState::Position);
	m_pTransform->Set_Position(m_vPos);

	return _uint();
}


CShield_Battery* CShield_Battery::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CShield_Battery* pInstance = new CShield_Battery(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShield_Battery::Clone(void* pArg)
{
	CShield_Battery* pClone = new CShield_Battery(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CShield_Battery::Free()
{
	Safe_Release(m_pModelMesh);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
