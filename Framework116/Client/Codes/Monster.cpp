#include "stdafx.h"
#include "..\Headers\Monster.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CMonster::CMonster(const CMonster & other)
	: CGameObject(other)
{
}

HRESULT CMonster::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CMonster::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_Enemy1",
		L"Com_Mesh",
		(CComponent**)&m_pModelMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = _float3(300.f, 0.f, 300.f);	
	TransformDesc.vScale = _float3(5.f, 5.f, 5.f);
	TransformDesc.fSpeedPerSec = 60.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(45.f);

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
	BoundingSphere.fRadius = 6.f;

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
	m_vCreatePosition = TransformDesc.vPosition;

	m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	if (nullptr == m_pTargetTransform)
	{
		PRINT_LOG(L"Error", L"m_pTargetTransform is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMonster::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	
	Search_Target(fDeltaTime);

	if (!m_bBattle)
		Movement(fDeltaTime);
	else
		Monster_Battle(fDeltaTime);
		
		

	m_pTransform->Update_Transform();
	//m_pTransform->Update_Transform_Quaternion();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CMonster::LateUpdate_GameObject(_float fDeltaTime)
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

_uint CMonster::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pModelMesh->Render_Mesh(); 
	// Test

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CMonster::Movement(_float fDeltaTime)
{
	// 그냥 와리가리하면서 정찰
	m_fMoveDist += fDeltaTime;

	if (m_fMoveDist > 0.f && m_fMoveDist < 6.f)
	{
		m_pTransform->Go_Straight(fDeltaTime);
	}
	else if (m_fMoveDist >= 6.f && m_fMoveDist < 10.f)
	{
		m_pTransform->RotateY(fDeltaTime);
		
	}
	else if (m_fMoveDist >= 10.f)
	{
		m_fMoveDist = 0.f;
	}
	
	return _uint();
}

_uint CMonster::Search_Target(_float fDeltaTime)
{
	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);

	_float3 vDir = vTargetPos - vMonsterPos;
	_float fDist = D3DXVec3Length(&vDir);

		// 배틀상태 On
	if (fDist <= 200.f && fDist != 0.f)
	{
		m_bBattle = true;
	}
	return _uint();
}

_uint CMonster::Monster_Battle(_float fDeltaTime)
{
	// 배틀상태가 On일때 뭐할랭
	// 플레이어 공전하면서 공격?
	m_fPatternTime += fDeltaTime;

	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);

	_float3 vDir = vTargetPos - vMonsterPos;
	_float fDist = D3DXVec3Length(&vDir);
	D3DXVec3Normalize(&vDir, &vDir);


	
	if (m_fPatternTime > 0.f && m_fPatternTime <= 7.f)
	{
		if (fDist > 200.f)
		{
			m_pTransform->Go_Dir(vDir, fDeltaTime * 4.f);
		}
		RotateToPlayer(fDeltaTime * 3.f);
		m_pTransform->Go_Side(fDeltaTime * 2.f);
		m_bAttack = true;
	}
	else if (m_fPatternTime > 7.f && m_fPatternTime <= 8.f)
	{
		m_pTransform->Go_Up(-fDeltaTime);
		m_bAttack = false;
	}
	else if (m_fPatternTime > 8.f && m_fPatternTime <= 10.f)
	{
		// 플레이어 안바라보고 그 각도에서 회전이 안먹어요 하면서 직진
		if (fDist < 500.f)
		{
			m_pTransform->Go_Straight(fDeltaTime * 10.f);
		}
		m_bAttack = false;
	}
	else if (m_fPatternTime > 10.f)
	{
		// 다시 플레이어를 바라보렴
		RotateToPlayer(fDeltaTime * 3.f);
		m_fPatternTime = 0.f;
	}
	

	m_fAttackDelay += fDeltaTime;

	if (m_fAttackDelay > 1.f && m_bAttack == true)
	{
		TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

		pArg->vPosition = m_pTransform->Get_State(EState::Position);
		pArg->vRotate = m_pTransform->Get_TransformDesc().vRotate;

		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Sniper_Bullet",
			L"Layer_Sniper_Bullet", pArg)))
		{
			PRINT_LOG(L"Error", L"Failed To Add GameObject_Sniper_Bullet In Layer");
			return E_FAIL;
		}
		m_fAttackDelay = 0.f;
	}


	return _uint();
}

_uint CMonster::Shoot_Bullet(_float fDeltaTime)
{
	TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

	pArg->vPosition = m_pTransform->Get_State(EState::Position);
	pArg->vRotate = m_pTransform->Get_TransformDesc().vRotate;

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Sniper_Bullet",
		L"Layer_Sniper_Bullet", pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Sniper_Bullet In Layer");
		return E_FAIL;
	}
	m_fAttackDelay = 0.f;
	return _uint();
}

_bool CMonster::RotateToPlayer(_float fDeltaTime)
{
	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	_float3 vSniperPos = m_pTransform->Get_State(EState::Position);

	_float3 vTargetDir = vTargetPos - vSniperPos;
	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float3 vSniperLook = m_pTransform->Get_State(EState::Look);
	_float3 vSniperUp = m_pTransform->Get_State(EState::Up);
	D3DXVec3Normalize(&vSniperLook, &vSniperLook);

	_float fCeta = D3DXVec3Dot(&vTargetDir, &vSniperLook);
	_float fRadianMax = D3DXToRadian(95.f);
	_float fRadianMin = D3DXToRadian(85.f);

	_float3 vMyRight, vMyLeft, vMissileUp, vMissileDown;
	D3DXVec3Cross(&vMyRight, &vSniperUp, &vSniperLook);
	D3DXVec3Cross(&vMyLeft, &vSniperLook, &vSniperUp);
	D3DXVec3Cross(&vMissileUp, &vMyRight, &vSniperLook);
	D3DXVec3Cross(&vMissileDown, &vSniperLook, &vMyRight);

	D3DXVec3Normalize(&vMyRight, &vMyRight);
	D3DXVec3Normalize(&vMyLeft, &vMyLeft);
	D3DXVec3Normalize(&vMissileUp, &vMissileUp);
	D3DXVec3Normalize(&vMissileDown, &vMissileDown);

	_float fRight = D3DXVec3Dot(&vTargetDir, &vMyRight);
	_float fLeft = D3DXVec3Dot(&vTargetDir, &vMyLeft);
	_float fUp = D3DXVec3Dot(&vTargetDir, &vMissileUp);
	_float fDown = D3DXVec3Dot(&vTargetDir, &vMissileDown);


	if (fRight < fLeft)
		m_pTransform->RotateY(-fDeltaTime);
	else
		m_pTransform->RotateY(fDeltaTime);

	if (fUp < fDown)
		m_pTransform->RotateX(-fDeltaTime);
	else
		m_pTransform->RotateX(fDeltaTime);

	if (fabs(fRight - fLeft) < 10.f || fabs(fUp - fDown) < 10.f)
	{
		m_IsLookingPlayer = true;
	}
	else
		m_IsLookingPlayer = false;

	return m_IsLookingPlayer;
}


CMonster * CMonster::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CMonster* pInstance = new CMonster(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonster::Clone(void * pArg/* = nullptr*/)
{
	CMonster* pClone = new CMonster(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMonster::Free()
{
	Safe_Release(m_pModelMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
