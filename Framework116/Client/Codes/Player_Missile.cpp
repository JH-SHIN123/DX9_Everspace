#include "stdafx.h"
#include "..\Headers\Player_Missile.h"
#include "Player.h"
#include "Pipeline.h"
#include "MainCam.h"
#include "Collision.h"

CPlayer_Missile::CPlayer_Missile(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CPlayer_Missile::CPlayer_Missile(const CPlayer_Missile & other)
	: CGameObject(other)
{
}

HRESULT CPlayer_Missile::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPlayer_Missile::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_GeoMesh_Player_Missile",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Texture_Player_Bullet",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 60.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vScale = { 0.1f, 0.1f, 0.1f };

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
	BoundingSphere.fRadius = 15.f;

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


	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);

	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
		return E_FAIL;
	}

	// 유도할 타겟.
	//if (m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster")->size() != 0)
	//{
	//	m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Boss_Monster", L"Com_Transform");
	//	Safe_AddRef(m_pTargetTransform);
	//	if (nullptr == m_pTargetTransform)
	//	{
	//		PRINT_LOG(L"Error", L"m_pTargetTransform is nullptr");
	//		return E_FAIL;
	//	}
	//}


	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_float3 vPlayerRight = m_pPlayerTransform->Get_State(EState::Right);
	_float3 vPlayerRotate = m_pPlayerTransform->Get_TransformDesc().vRotate;

	m_vMuzzlePos = vPlayerPos;
	m_pTransform->Set_Position(m_vMuzzlePos);
	m_pTransform->Set_Rotate(vPlayerRotate);

	RAY ray;
	CPipeline::CreatePickingRay(ray, g_hWnd, WINCX, WINCY, m_pDevice);
	m_vPlayerLook = ray.vDirection;

	/* 뷰스페이스 -> 월드스페이스 */
	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);
	D3DXVec3TransformCoord(&ray.vPos, &ray.vPos, &matView);
	D3DXVec3TransformNormal(&ray.vDirection, &ray.vDirection, &matView);
	D3DXVec3Normalize(&ray.vDirection, &ray.vDirection);

	m_vPlayerLook = ray.vDirection;
	D3DXVec3Normalize(&m_vPlayerLook, &m_vPlayerLook);


	// Add Effect
	CEffectHandler::Add_Layer_Effect_Missile_Head(this, &m_pHeadParticle);
	CEffectHandler::Add_Layer_Effect_Missile_Smoke(this, &m_pBulletParticle);

	vDir = (_float3*)pArg;
	

	STAT_INFO tStatus;
	tStatus.iAtk = 170;

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Status_Info",
		L"Com_StatInfo",
		(CComponent**)&m_pInfo,
		&tStatus)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}


	return S_OK;
}

_uint CPlayer_Missile::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);
	// 발사되자 마자 유도안되게.
	m_fBeforeHoming += fDeltaTime;

	if (m_fBeforeHoming < 1.f)
		Movement(fDeltaTime);
	else
	{
		Search_Shortest_Target(fDeltaTime);
		if (m_pTargetTransform != nullptr)
		{
			m_fAddSpeed += 0.75f;
			m_fRotateSpeed += D3DXToRadian(15.f);
			m_pTransform->Set_SpeedPerSec(m_fAddSpeed);
			m_pTransform->Set_RotatePerSec(m_fRotateSpeed);
			Homing(fDeltaTime);
		}
		else if (m_pTargetTransform == nullptr)
			Movement(fDeltaTime);
	}
	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	
	m_fLifeTime += fDeltaTime;
	
	return NO_EVENT;
}

_uint CPlayer_Missile::LateUpdate_GameObject(_float fDeltaTime)
{
	if (m_IsCollide)
	{
		//m_IsDead = true;

		if (m_pBulletParticle) {
			m_pBulletParticle->Set_IsDead(true);
			m_pBulletParticle = nullptr;
		}

		if (m_pHeadParticle) {
			m_pHeadParticle->Set_IsDead(true);
			m_pHeadParticle = nullptr;
		}
		CEffectHandler::Add_Layer_Effect_Missile_Explosion(m_pTransform->Get_State(EState::Position));
		m_pManagement->StopSound(CSoundMgr::PLAYER_WEAPON);
		m_pManagement->PlaySound(L"Missile_Explosion.ogg", CSoundMgr::PLAYER_MISSILE_EXPLOSION);
		return DEAD_OBJECT;
	}

	if (m_fLifeTime >= 4.f)
	{
		//m_IsDead = true;

		if (m_pBulletParticle) {
			m_pBulletParticle->Set_IsDead(true);
			m_pBulletParticle = nullptr;
		}

		if (m_pHeadParticle) {
			m_pHeadParticle->Set_IsDead(true);
			m_pHeadParticle = nullptr;
		}
		CEffectHandler::Add_Layer_Effect_Missile_Explosion(m_pTransform->Get_State(EState::Position));
		return DEAD_OBJECT;
	}

	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CPlayer_Missile::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_Mesh();
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CPlayer_Missile::Movement(_float fDeltaTime)
{
	m_pTransform->Go_Dir(*vDir, fDeltaTime);
	
	_float3 vLook = m_pPlayerTransform->Get_State(EState::Look);
	m_pTransform->Go_Dir(vLook, fDeltaTime);
	return _uint();
}

_uint CPlayer_Missile::Homing(_float fDeltaTime)
{
	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	
	_float3 vMyPos = m_pTransform->Get_State(EState::Position);

	_float3 vTargetDir = vTargetPos - vMyPos;
	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float3 vMyLook = m_pTransform->Get_State(EState::Look);
	_float3 vMyUp = m_pTransform->Get_State(EState::Up);
	D3DXVec3Normalize(&vMyLook, &vMyLook);

	_float fCeta = D3DXVec3Dot(&vTargetDir, &vMyLook);
	_float fRadianMax = D3DXToRadian(95.f);
	_float fRadianMin = D3DXToRadian(85.f);

	_float3 vMyRight, vMyLeft, vMissileUp, vMissileDown;
	D3DXVec3Cross(&vMyRight, &vMyUp, &vMyLook);
	D3DXVec3Cross(&vMyLeft, &vMyLook, &vMyUp);
	D3DXVec3Cross(&vMissileUp, &vMyRight, &vMyLook);
	D3DXVec3Cross(&vMissileDown, &vMyLook, &vMyRight);

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

	m_pTransform->Go_Straight(fDeltaTime);
	return _uint();
}

_uint CPlayer_Missile::Searching_Target(_float fDeltaTime)
{

	return _uint();
}

_uint CPlayer_Missile::Search_Shortest_Target(_float fDeltaTime)
{
	const list<class CGameObject*>* m_listCheckBoss = m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster");
	const list<class CGameObject*>* m_listCheckDrone = m_pManagement->Get_GameObjectList(L"Layer_Drone");
	const list<class CGameObject*>* m_listCheckSniper = m_pManagement->Get_GameObjectList(L"Layer_Sniper");
	const list<class CGameObject*>* m_listCheckNormalMonster = m_pManagement->Get_GameObjectList(L"Layer_Monster");

	//if (nullptr == m_listCheckBoss || nullptr == m_listCheckDrone || nullptr == m_listCheckSniper || nullptr == m_listCheckNormalMonster)
	//{
	//	PRINT_LOG(L"Error", L"One of list is nullptr");
	//	return NO_EVENT;
	//}

	if (nullptr != m_listCheckBoss)
	{
		for (auto& pObj : *m_listCheckBoss)
		{
			_float3 vTargetPos = pObj->Get_Collides()->front()->Get_BoundingSphere().Get_Position();
			_float3 vMissilePos = m_pTransform->Get_State(EState::Position);
			_float3 vDir = vTargetPos - vMissilePos;

			_float Test = D3DXVec3Length(&vDir);
			if (Test > 1.f)
				m_fDistToBoss = D3DXVec3Length(&vDir);
			else
				m_fDistToBoss = 9999.f;
		}
		if (m_listCheckBoss->size() == 0)
			m_fDistToBoss = 9999.f;
	}
	else
	{
		m_fDistToBoss = 9999.f;
	}
	if (nullptr != m_listCheckDrone)
	{
		for (auto& pObj : *m_listCheckDrone)
		{
			_float3 vTargetPos = pObj->Get_Collides()->front()->Get_BoundingSphere().Get_Position();
			_float3 vMissilePos = m_pTransform->Get_State(EState::Position);
			_float3 vDir = vTargetPos - vMissilePos;

			_float Test = D3DXVec3Length(&vDir);
			if (Test > 1.f)
				m_fDistToDrone = D3DXVec3Length(&vDir);
			else
				m_fDistToDrone = 9999.f;
			if (Test > 0.f)
				m_fDistToDrone = D3DXVec3Length(&vDir);
		}
		if (m_listCheckDrone->size() == 0)
			m_fDistToDrone = 9999.f;
	}
	else
	{
		m_fDistToDrone = 9999.f;
	}
	if (nullptr != m_listCheckSniper)
	{
		for (auto& pObj : *m_listCheckSniper)
		{
			_float3 vTargetPos = pObj->Get_Collides()->front()->Get_BoundingSphere().Get_Position();
			_float3 vMissilePos = m_pTransform->Get_State(EState::Position);
			_float3 vDir = vTargetPos - vMissilePos;

			_float Test = D3DXVec3Length(&vDir);

			if (Test > 1.f)
				m_fDistToSniper = D3DXVec3Length(&vDir);
			else
				m_fDistToSniper = 9999.f;
		}
		if (m_listCheckSniper->size() == 0)
			m_fDistToSniper = 9999.f;
	}
	else
	{
		m_fDistToSniper = 9999.f;
	}
	if (nullptr != m_listCheckNormalMonster)
	{
		for (auto& pObj : *m_listCheckNormalMonster)
		{
			_float3 vTargetPos = pObj->Get_Collides()->front()->Get_BoundingSphere().Get_Position();
			_float3 vMissilePos = m_pTransform->Get_State(EState::Position);
			_float3 vDir = vTargetPos - vMissilePos;

			_float Test = D3DXVec3Length(&vDir);

			if (Test > 1.f)
				m_fDistToNormalMonster = D3DXVec3Length(&vDir);
			else
				m_fDistToNormalMonster = 9999.f;
		}
		if (m_listCheckNormalMonster->size() == 0)
			m_fDistToNormalMonster = 9999.f;
	}
	else
	{
		m_fDistToNormalMonster = 9999.f;
	}




	if (m_fDistToBoss < m_fDistToDrone && m_fDistToBoss < m_fDistToSniper && m_fDistToBoss < m_fDistToNormalMonster)
	{
		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Boss_Monster", L"Com_Transform");
	}
	else if (m_fDistToDrone < m_fDistToBoss && m_fDistToDrone < m_fDistToSniper && m_fDistToDrone < m_fDistToNormalMonster)
	{
		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Drone", L"Com_Transform");
	}
	else if (m_fDistToSniper < m_fDistToBoss && m_fDistToSniper < m_fDistToDrone && m_fDistToSniper < m_fDistToNormalMonster)
	{
		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Sniper", L"Com_Transform");
	}
	else if (m_fDistToNormalMonster < m_fDistToBoss && m_fDistToNormalMonster < m_fDistToDrone && m_fDistToNormalMonster < m_fDistToSniper)
	{
		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Monster", L"Com_Transform");
	}

	return _uint();
}

CPlayer_Missile * CPlayer_Missile::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CPlayer_Missile* pInstance = new CPlayer_Missile(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Missile::Clone(void * pArg/* = nullptr*/)
{
	CPlayer_Missile* pClone = new CPlayer_Missile(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player_Bullet");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlayer_Missile::Free()
{
	if (m_IsClone)
		CEffectHandler::Add_Layer_Effect_Missile_Explosion(m_pTransform->Get_State(EState::Position));

	//Safe_Release(m_pTargetTransform);
	Safe_Release(m_pInfo);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	if (m_pBulletParticle) {
		m_pBulletParticle->Set_IsDead(true);
		m_pBulletParticle = nullptr;
	}

	if (m_pHeadParticle) {
		m_pHeadParticle->Set_IsDead(true);
		m_pHeadParticle = nullptr;
	}


	CGameObject::Free();
}
