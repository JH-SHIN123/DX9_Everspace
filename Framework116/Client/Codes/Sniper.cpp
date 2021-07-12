#include "stdafx.h"
#include "..\Headers\Sniper.h"

CSniper::CSniper(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CSniper::CSniper(const CSniper & other)
	: CGameObject(other)
{
}

HRESULT CSniper::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CSniper::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_Monster",
		L"Com_Mesh",
		(CComponent**)&m_pModelMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = _float3(0.5f, 0.f, 0.5f);	
	TransformDesc.vScale = _float3(2.f, 2.f, 2.f);

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
	BoundingSphere.fRadius = 1.f;

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

	// Init
	m_eNextState = State::Research;
	m_vCreatePosition = TransformDesc.vPosition;
	m_vResearchRange = { 50.f,50.f,50.f };

	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);

	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"Player Transform is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

_uint CSniper::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	
	Movement(fDeltaTime);
	StateCheck();

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CSniper::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_IsCollide) {
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);
		m_IsCollide = false;
	}

	return _uint();
}

_uint CSniper::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pModelMesh->Render_Mesh(); 
	// Test

#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CSniper::Movement(_float fDeltaTime)
{
	if (m_eCurState = State::Research) {
		Researching(fDeltaTime);
	}
	

	return _uint();
}

_uint CSniper::Researching(_float fDeltaTime)
{
	// if 범위보다 벗어났다. -> Create Pos로 돌아가기


	return _uint();
}

void CSniper::StateCheck()
{
	if (m_eCurState != m_eNextState) {
		switch (m_eNextState)
		{
		case State::Research:
			break;
		case State::Warning:
			break;
		case State::Attack:
			break;
		case State::Die:
			break;
		}
		m_eCurState = m_eNextState;
	}
}

CSniper * CSniper::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CSniper* pInstance = new CSniper(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CSniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSniper::Clone(void * pArg/* = nullptr*/)
{
	CSniper* pClone = new CSniper(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CSniper");
		Safe_Release(pClone);
	}

	return pClone;
}

void CSniper::Free()
{
	Safe_Release(m_pModelMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
