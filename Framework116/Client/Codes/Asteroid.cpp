#include "stdafx.h"
#include "Asteroid.h"

CAsteroid::CAsteroid(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CAsteroid::CAsteroid(const CAsteroid& other)
	: CGameObject(other)
{
}

HRESULT CAsteroid::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CAsteroid::Ready_GameObject(void* pArg)
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

	// For.Com_Mesh Component_Mesh_Rock_Generic_001
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
	float randRotateSpeed = rand() % 5 + 10.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(randRotateSpeed);
	TransformDesc.fSpeedPerSec = 4.f;

	// 운석 데코레이트 (충돌X)
	if (pDesc->wstrMeshName == L"Component_Mesh_Rock_Cloud") {
		m_bDecorate = true;
	}

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

	if (false == m_bDecorate) {
		// For.Com_Collide
		BOUNDING_SPHERE BoundingSphere;
		if (pDesc->tTransformDesc.vScale.x >= 20.f)
			BoundingSphere.fRadius = 4.f;
		else if (pDesc->tTransformDesc.vScale.x <= 2.f)
			BoundingSphere.fRadius = 4.5f;
		else
			BoundingSphere.fRadius = 5.f;

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

		m_vRandomRotateDir.x = (float)(rand() % 2);
		m_vRandomRotateDir.y = (float)(rand() % 2);
		m_vRandomRotateDir.z = (float)(rand() % 2);

	}

	return S_OK;
}

_uint CAsteroid::Update_GameObject(_float fDeltaTime)
{
	if (m_IsDead) return DEAD_OBJECT;

	CGameObject::Update_GameObject(fDeltaTime);
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	if (false == m_bDecorate)
		m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CAsteroid::LateUpdate_GameObject(_float fDeltaTime)
{
	if (m_IsDead) return DEAD_OBJECT;

	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CAsteroid::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();
	// Test

#ifdef _DEBUG // Render Collide
	//if(m_pCollide) m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CAsteroid::Movement(_float fDeltaTime)
{
	//if (m_vRandomRotateDir.x) {
	//	m_pTransform->RotateX(fDeltaTime);
	//}
	//if (m_vRandomRotateDir.y) {
	//	m_pTransform->RotateY(fDeltaTime);
	//}
	//if (m_vRandomRotateDir.z) {
	//	m_pTransform->RotateZ(fDeltaTime);
	//}

	//float fMoveSpeed = m_pTransform->Get_TransformDesc().fSpeedPerSec;
	//if(m_fMoveUpDeltaT <= 2.f)
	//{
	//	m_pTransform->Move({ 0.f,fDeltaTime * fMoveSpeed ,0.f });
	//	m_fMoveUpDeltaT += fDeltaTime;
	//}
	//else if (m_fMoveUpDeltaT <= 4.f) {
	//	m_pTransform->Move({ 0.f,-fDeltaTime * fMoveSpeed,0.f });
	//	m_fMoveUpDeltaT += fDeltaTime;
	//}
	//else {
	//	m_fMoveUpDeltaT = 0.f;
	//}

	m_pTransform->RotateZ(fDeltaTime);
	m_pTransform->RotateX(fDeltaTime);
	m_pTransform->RotateY(fDeltaTime);

	return _uint();
}

CAsteroid* CAsteroid::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CAsteroid* pInstance = new CAsteroid(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAsteroid::Clone(void* pArg)
{
	CAsteroid* pClone = new CAsteroid(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CAsteroid::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
