#include "stdafx.h"
#include "BrokenPlane.h"

CBrokenPlane::CBrokenPlane(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CBrokenPlane::CBrokenPlane(const CBrokenPlane& other)
	: CGameObject(other)
{
}

HRESULT CBrokenPlane::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CBrokenPlane::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);

	GAMEOBJECT_DESC* pDesc = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (pDesc = dynamic_cast<GAMEOBJECT_DESC*>(ptr))
		{}
		else
		{
			PRINT_LOG(L"Error", L"CBrokenPlane is nullptr");
			return E_FAIL;
		}
	}

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		pDesc->wstrMeshName,
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc = pDesc->tTransformDesc;
	TransformDesc.vRotate = { 0.f, 60.f, 0.f };
	TransformDesc.fSpeedPerSec = 4.f;

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

	BOUNDING_SPHERE BoundingSphere;
	BoundingSphere.fRadius = 90.f;

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

	return S_OK;
}

_uint CBrokenPlane::Update_GameObject(_float fDeltaTime)
{
	if (m_IsDead) 
		return DEAD_OBJECT;

	if (m_bCreateEffect)
	{
		if (m_fCreateEffectTime >= 5.f)
		{
			CEffectHandler::Add_Layer_Effect_BrokenPlane_Smoke(&m_pSmokeEffect);
			m_fCreateEffectTime = 0.f;
			m_bCreateEffect = false;
		}
		else m_fCreateEffectTime += fDeltaTime;
	}

	CGameObject::Update_GameObject(fDeltaTime);
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	if(m_pCollide) m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);

	if (m_pSmokeEffect)
	{
		CTransform* pTransform = (CTransform*)m_pSmokeEffect->Get_Component(L"Com_Transform");
		if(pTransform) pTransform->Set_Position(m_pTransform->Get_State(EState::Position));
	}

	return _uint();
}

_uint CBrokenPlane::LateUpdate_GameObject(_float fDeltaTime)
{
	if (m_IsDead)
		return DEAD_OBJECT;

	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CBrokenPlane::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();

#ifdef _DEBUG
	if (m_pCollide)
		m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CBrokenPlane::Movement(_float fDeltaTime)
{
	// 위아래로 둥둥 떠다니게
	float fMoveSpeed = m_pTransform->Get_TransformDesc().fSpeedPerSec;
	if(m_fMoveUpDeltaT <= 2.f)
	{
		m_pTransform->Move({ 0.f,fDeltaTime * fMoveSpeed ,0.f });
		m_fMoveUpDeltaT += fDeltaTime;
	}
	else if (m_fMoveUpDeltaT <= 4.f) {
		m_pTransform->Move({ 0.f,-fDeltaTime * fMoveSpeed,0.f });
		m_fMoveUpDeltaT += fDeltaTime;
	}
	else {
		m_fMoveUpDeltaT = 0.f;
	}

	return _uint();
}

CBrokenPlane* CBrokenPlane::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CBrokenPlane* pInstance = new CBrokenPlane(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrokenPlane::Clone(void* pArg)
{
	CBrokenPlane* pClone = new CBrokenPlane(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CBrokenPlane::Free()
{
	if (m_pSmokeEffect)
	{
		m_pSmokeEffect->Set_IsDead(true);
		m_pSmokeEffect = nullptr;
	}

	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);
	
	CGameObject::Free();
}
