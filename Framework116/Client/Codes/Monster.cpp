#include "stdafx.h"
#include "..\Headers\Monster.h"
#include "HP_Bar.h"
#include "HP_Bar_Border.h"
#include "Player.h"
#include "EngineEffectSystem.h"
#include "WingBoost_System.h"
#include "Pipeline.h"
#include "Collision.h"
#include "New_LockOn.h"

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

	m_fHp = 1100.f;
	m_fFullHp = m_fHp;

	// Add Engine-Boost Effect
	CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pLeftEngineEffect);
	m_vLeftEngineOffset = { -1.4f, 0.9f, -1.7f };
	CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pRightEngineEffect);
	m_vRightEngineOffset = { 1.4f, 0.9f, -1.7f };

	// Add Wing-Boost Effect
	CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pLeftWingBoost);
	m_vLeftWingOffset = { -6.2f, -1.5f, -4.f };
	CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pRightWingBoost);
	m_vRightWingOffset = { 6.2f, -1.5f, -4.f };
	return S_OK;
}

_uint CMonster::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	
	m_IsBoost = true;
	Search_Target(fDeltaTime);
	if (!m_IsDead)
	{
		if (!m_bBattle)
			Movement(fDeltaTime);
		else
			Monster_Battle(fDeltaTime);

		if (m_pHp_Bar != nullptr && m_pHP_Bar_Border != nullptr)
		{
			Set_Hp_Pos();
			Check_Degree();
		}

		m_pTransform->Update_Transform();
		//m_pTransform->Update_Transform_Quaternion();
		m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
		//순서중요!
		Make_LockOn();
		Update_Effect();
	}
	return NO_EVENT;
}

_uint CMonster::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_fHp <= 0.f)
	{
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);
		m_IsDead = true;
		if (m_pHp_Bar)
			m_pHp_Bar->Set_IsDead(TRUE);
		if (m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_IsDead(TRUE);
		if (m_pLockOn)
			m_pLockOn->Set_IsDead(TRUE);
		m_pManagement->PlaySound(L"Ship_Explosion.ogg", CSoundMgr::SHIP_EXPLOSION);
		((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Someone_Try_To_Kill_Me(false);
		return DEAD_OBJECT;
	}
	if (nullptr == m_pHp_Bar)
	{
		m_IsCollide = false;
	}
	if (m_IsCollide) {
		// Bullet 데미지 만큼.
		CEffectHandler::Add_Layer_Effect_Bullet_Explosion(m_pTransform->Get_State(EState::Position));
		m_pHp_Bar->Set_ScaleX(-100.f / m_fFullHp * m_fHpLength);
		m_fHp -= 100.f;
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
	if (fDist <= 300.f && fDist != 0.f)
	{
		Add_Hp_Bar(fDeltaTime);
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

	_float3 vLook = m_pTransform->Get_State(EState::Look);

	
	if (m_fPatternTime > 0.f && m_fPatternTime <= 7.f)
	{
		if (fDist > 200.f)
		{
			m_pTransform->Go_Dir(vDir, fDeltaTime * 4.f);
		}
		RotateToPlayer(fDeltaTime * 3.f);
		m_pTransform->Go_Side(fDeltaTime * 2.f);
		m_pTransform->RotateZ(fDeltaTime * 1.f);
		
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

_uint CMonster::Add_Hp_Bar(_float fDeltaTime)
{
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	_float3 vPlayerPos = m_pTargetTransform->Get_State(EState::Position);

	_float3 vDir = vMonsterPos - vPlayerPos;
	_float fDist = D3DXVec3Length(&vDir);


	if (m_IsHPBar == false)
	{
		//////////////////3d좌표를 2d좌표로////////////////////////////
		D3DVIEWPORT9 vp2;
		m_pDevice->GetViewport(&vp2);
		_float4x4 TestView2, TestProj2;
		m_pDevice->GetTransform(D3DTS_VIEW, &TestView2);
		m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj2);
		_float4x4 matCombine2 = TestView2 * TestProj2;
		D3DXVec3TransformCoord(&vMonsterPos, &vMonsterPos, &matCombine2);
		vMonsterPos.x += 1.f;
		vMonsterPos.y += 1.f;

		vMonsterPos.x = (vp2.Width * (vMonsterPos.x)) / 2.f + vp2.X;
		vMonsterPos.y = (vp2.Height * (2.f - vMonsterPos.y) / 2.f + vp2.Y);

		_float3 ptBoss;
		ptBoss.x = -1.f * (WINCX / 2) + vMonsterPos.x;
		ptBoss.y = 1.f * (WINCY / 2) + vMonsterPos.y;
		ptBoss.z = 0.f;
		//////////////////////////////////////////////////////////////////
		// 감지범위에 들어오게 되면 HP_Bar 생성!

		CGameObject* pGameObject = nullptr;
		UI_DESC HUD_Hp_Bar;
		HUD_Hp_Bar.tTransformDesc.vPosition = { ptBoss.x - 64.f, ptBoss.y - 50.f, 0.f };
		HUD_Hp_Bar.tTransformDesc.vScale = { m_fHp * (m_fHpLength / m_fFullHp), 8.f, 0.f };
		HUD_Hp_Bar.wstrTexturePrototypeTag = L"Component_Texture_HP_Bar";
		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_HP_Bar",
			L"Layer_HP_Bar",
			&HUD_Hp_Bar, &pGameObject)))
		{
			PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
			return E_FAIL;
		}

		CGameObject* pGameObjectBorder = nullptr;
		UI_DESC HUD_Hp_Bar_Border;
		HUD_Hp_Bar_Border.tTransformDesc.vPosition = { ptBoss.x - 64.f, ptBoss.y - 50.f, 0.f };
		HUD_Hp_Bar_Border.tTransformDesc.vScale = { m_fHp * (m_fHpLength / m_fFullHp) + 2.5f, 12.f, 0.f };
		HUD_Hp_Bar_Border.wstrTexturePrototypeTag = L"Component_Texture_HP_Border";
		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_HP_Bar_Border",
			L"Layer_HP_Bar_Border",
			&HUD_Hp_Bar_Border, &pGameObjectBorder)))
		{
			PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
			return E_FAIL;
		}
		m_IsHPBar = true;

		m_pHP_Bar_Border = static_cast<CHP_Bar_Border*>(pGameObjectBorder);
		m_pHP_Bar_Border->Who_Make_Me(m_pHP_Bar_Border->MAKER_MONSTER);

		m_pHp_Bar = static_cast<CHP_Bar*>(pGameObject);
		m_pHp_Bar->Who_Make_Me(m_pHp_Bar->MAKER_MONSTER);

	}
	return S_OK;
}

void CMonster::Set_Hp_Pos()
{
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	_float3 vPlayerPos = m_pTargetTransform->Get_State(EState::Position);

	_float3 vDir = vMonsterPos - vPlayerPos;
	_float fDist = D3DXVec3Length(&vDir);

	// 너무 멀면 렌더하지마
	if (fDist > 600.f)
	{
		if (m_pHp_Bar)
			m_pHp_Bar->Set_Is_Far(true);
		if (m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_Is_Far(true);
	}
	else
	{
		if (m_pHp_Bar)
			m_pHp_Bar->Set_Is_Far(false);
		if (m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_Is_Far(false);
	}

	D3DVIEWPORT9 vp2;
	m_pDevice->GetViewport(&vp2);
	_float4x4 TestView2, TestProj2;
	m_pDevice->GetTransform(D3DTS_VIEW, &TestView2);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj2);
	_float4x4 matCombine2 = TestView2 * TestProj2;
	D3DXVec3TransformCoord(&vMonsterPos, &vMonsterPos, &matCombine2);
	vMonsterPos.x += 1.f;
	vMonsterPos.y += 1.f;

	vMonsterPos.x = (vp2.Width * (vMonsterPos.x)) / 2.f + vp2.X;
	vMonsterPos.y = (vp2.Height * (2.f - vMonsterPos.y) / 2.f + vp2.Y);

	_float3 ptBoss;
	ptBoss.x = vMonsterPos.x;
	ptBoss.y = vMonsterPos.y;
	ptBoss.z = 0.f;
	//////////////////////////////////////////////////////////////////

	_float3 vPosition = { ptBoss.x - (WINCX / 2.f) - 30.f, -ptBoss.y + (WINCY / 2.f) + 30.f, 0.f };
	_float3 vLockOnPos = { ptBoss.x - (WINCX / 2.f), -ptBoss.y + (WINCY / 2.f), 0.f };
	//_float3 vPosition = { 0.f, 0.f, 0.f };

	if(m_pHp_Bar)
		m_pHp_Bar->Set_Pos(vPosition);
	if(m_pHP_Bar_Border)
		m_pHP_Bar_Border->Set_Pos(vPosition);
	if(m_pLockOn)
		m_pLockOn->Set_Pos(vLockOnPos);
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
	Safe_Release(m_pHp_Bar);
	Safe_Release(m_pHP_Bar_Border);
	Safe_Release(m_pLockOn);
	Safe_Release(m_pModelMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}

_uint CMonster::Check_Degree()
{
	_float3 vPlayerLook = m_pTargetTransform->Get_State(EState::Look);
	_float3 v1 = vPlayerLook;
	_float3 v2 = m_pTransform->Get_State(EState::Position) - m_pTargetTransform->Get_State(EState::Position);
	_float fCeta;
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
	_float v1v2 = D3DXVec3Dot(&v1, &v2);
	_float v1Length = D3DXVec3Length(&v1);
	_float v2Length = D3DXVec3Length(&v2);
	fCeta = acosf(v1v2 / (v1Length * v2Length));

	_float fDegree = D3DXToDegree(fCeta);

	if (v2.x < 0)
	{
		if (fDegree > 90.f)
		{
			if(m_pHp_Bar)
			m_pHp_Bar->Set_IsBack(true);
			if(m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_IsBack(true);
			if(m_pLockOn)
			m_pLockOn->Set_IsBack(true);
		}
		else if (fDegree <= 90.f)
		{
			if (m_pHp_Bar)
				m_pHp_Bar->Set_IsBack(false);
			if (m_pHP_Bar_Border)
				m_pHP_Bar_Border->Set_IsBack(false);
			if (m_pLockOn)
				m_pLockOn->Set_IsBack(false);
		}
	}
	else
	{
		if (fDegree > 90.f)
		{
			if (m_pHp_Bar)
				m_pHp_Bar->Set_IsBack(true);
			if (m_pHP_Bar_Border)
				m_pHP_Bar_Border->Set_IsBack(true);
			if (m_pLockOn)
				m_pLockOn->Set_IsBack(true);
		}
		else if (fDegree <= 90.f)
		{
			if (m_pHp_Bar)
				m_pHp_Bar->Set_IsBack(false);
			if (m_pHP_Bar_Border)
				m_pHP_Bar_Border->Set_IsBack(false);
			if (m_pLockOn)
				m_pLockOn->Set_IsBack(false);
		}
	}
	return _uint();
}

_uint CMonster::Make_LockOn()
{
	RAY ray;
	CPipeline::CreatePickingRay(ray, g_hWnd, WINCX, WINCY, m_pDevice);
	_float3 m_vLockOn;
	m_vLockOn = ray.vDirection;

	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);
	D3DXVec3TransformCoord(&ray.vPos, &ray.vPos, &matView);
	D3DXVec3TransformNormal(&ray.vDirection, &ray.vDirection, &matView);
	D3DXVec3Normalize(&ray.vDirection, &ray.vDirection);

	m_vLockOn = ray.vDirection;
	D3DXVec3Normalize(&m_vLockOn, &m_vLockOn);

	// True면? - LockOn HUD 생성
	
	if (CCollision::IntersectRayToSphere(ray, m_pCollide->Get_BoundingSphere()))
	{
		if (m_pManagement->Get_GameObjectList(L"Layer_NewLockOn") != nullptr
			&& m_pManagement->Get_GameObjectList(L"Layer_NewLockOn")->size() == 1)
		{
			m_pManagement->Get_GameObjectList(L"Layer_NewLockOn")->front()->Set_IsDead(true);
			m_IsLockOn = false;
		}
		else if (m_pManagement->Get_GameObjectList(L"Layer_NewLockOn") != nullptr
			&& m_pManagement->Get_GameObjectList(L"Layer_NewLockOn")->size() == 0)
		{
			m_IsLockOn = false;
		}

		if (!m_IsLockOn)
		{
			CGameObject* pLockOn = nullptr;
			UI_DESC HUD_Lock_On;
			HUD_Lock_On.tTransformDesc.vPosition = { 11110.f, 0.f, 0.f };
			HUD_Lock_On.tTransformDesc.vScale = { 50.f, 50.f, 0.f };
			HUD_Lock_On.wstrTexturePrototypeTag = L"Component_Texture_LockOn";
			if (FAILED(m_pManagement->Add_GameObject_InLayer(
				EResourceType::NonStatic,
				L"GameObject_LockOn",
				L"Layer_NewLockOn",
				&HUD_Lock_On, &pLockOn)))
			{
				PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
				return E_FAIL;
			}
			m_IsLockOn = true;

			m_pLockOn = static_cast<CNew_LockOn*>(pLockOn);
			m_pLockOn->Who_Make_Me(m_pLockOn->MAKER_MONSTER);
		}
	}
	return S_OK;
}

void CMonster::Update_Effect()
{
	// Engine-Boost Effect
	if (m_pLeftEngineEffect) {
		_float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
		vEnginePos += m_pTransform->Get_State(EState::Right) * m_vLeftEngineOffset.x;
		vEnginePos += m_pTransform->Get_State(EState::Up) * m_vLeftEngineOffset.y;
		vEnginePos += m_pTransform->Get_State(EState::Look) * m_vLeftEngineOffset.z;
		m_pLeftEngineEffect->Set_EngineOffset(vEnginePos);
		m_pLeftEngineEffect->Set_IsBoost(m_IsBoost);
	}
	if (m_pRightEngineEffect) {
		_float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
		vEnginePos += m_pTransform->Get_State(EState::Right) * m_vRightEngineOffset.x;
		vEnginePos += m_pTransform->Get_State(EState::Up) * m_vRightEngineOffset.y;
		vEnginePos += m_pTransform->Get_State(EState::Look) * m_vRightEngineOffset.z;
		m_pRightEngineEffect->Set_EngineOffset(vEnginePos);
		m_pRightEngineEffect->Set_IsBoost(m_IsBoost);
	}

	// Wing-Boost Effect

	if (m_pLeftWingBoost) {
		_float3 vWingPos = m_pTransform->Get_TransformDesc().vPosition;
		vWingPos += m_pTransform->Get_State(EState::Right) * m_vLeftWingOffset.x;
		vWingPos += m_pTransform->Get_State(EState::Up) * m_vLeftWingOffset.y;
		vWingPos += m_pTransform->Get_State(EState::Look) * m_vLeftWingOffset.z;
		m_pLeftWingBoost->Set_WingOffset(vWingPos);
		m_pLeftWingBoost->Set_IsBoost(m_IsBoost);
	}
	if (m_pRightWingBoost) {
		_float3 vWingPos = m_pTransform->Get_TransformDesc().vPosition;
		vWingPos += m_pTransform->Get_State(EState::Right) * m_vRightWingOffset.x;
		vWingPos += m_pTransform->Get_State(EState::Up) * m_vRightWingOffset.y;
		vWingPos += m_pTransform->Get_State(EState::Look) * m_vRightWingOffset.z;
		m_pRightWingBoost->Set_WingOffset(vWingPos);
		m_pRightWingBoost->Set_IsBoost(m_IsBoost);
	}
	
}
