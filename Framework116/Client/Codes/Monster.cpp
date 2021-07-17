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
	, m_IsFight(other.m_IsFight)
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

	GAMEOBJECT_DESC* pDesc = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (pDesc = dynamic_cast<GAMEOBJECT_DESC*>(ptr))
		{
		}
		else
		{
			PRINT_LOG(L"Error", L"CMonster is nullptr");
			return E_FAIL;
		}
	}

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		pDesc->wstrMeshName,
		L"Com_Mesh",
		(CComponent**)&m_pModelMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc = pDesc->tTransformDesc;
	//TransformDesc.vPosition = _float3(300.f, 0.f, 300.f);	
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
	PASSDATA_COLLIDE tCollide;
	CStreamHandler::Load_PassData_Collide(L"enemy1", L"Component_Mesh_Enemy1", tCollide);
	m_Collides.reserve(tCollide.vecBoundingSphere.size());
	int i = 0;
	for (auto& bounds : tCollide.vecBoundingSphere) {
		if (FAILED(CGameObject::Add_Component(
			EResourceType::Static,
			L"Component_CollideSphere",
			L"Com_CollideSphere" + to_wstring(i++),
			nullptr,
			&bounds,
			true)))
		{
			PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
			return E_FAIL;
		}
	}

	// Init
	m_vCreatePosition = TransformDesc.vPosition;

	m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	if (nullptr == m_pTargetTransform)
	{
		PRINT_LOG(L"Error", L"m_pTargetTransform is nullptr");
		return E_FAIL;
	}

	STAT_INFO tStatus;
	tStatus.iMaxHp = 410;
	tStatus.iHp = tStatus.iMaxHp;
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

	//// Add Engine-Boost Effect
	//CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pLeftEngineEffect);
	//m_vLeftEngineOffset = { -1.4f, 0.9f, -1.7f };
	//CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pRightEngineEffect);
	//m_vRightEngineOffset = { 1.4f, 0.9f, -1.7f };

	//// Add Wing-Boost Effect
	//CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pLeftWingBoost);
	//m_vLeftWingOffset = { -6.2f, -1.5f, -4.f };
	//CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pRightWingBoost);
	//m_vRightWingOffset = { 6.2f, -1.5f, -4.f };

	Get_Delivery();

	return S_OK;
}

_uint CMonster::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsDead)
		return DEAD_OBJECT;

	m_IsBoost = true;
	Search_Target(fDeltaTime);
	if (!m_IsDead)
	{
		if (m_IsFight == true)
		{
			if (!m_bBattle)
				Movement(fDeltaTime);
			else
				Monster_Battle(fDeltaTime);
		}

		if (m_pHp_Bar != nullptr && m_pHP_Bar_Border != nullptr)
		{
			Set_Hp_Pos();
			Check_Degree();
		}

		m_pTransform->Update_Transform();
		for (auto& p : m_Collides)
		{
			if (p) p->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
		}
		//순서중요!
		Make_LockOn();
		//Update_Effect();
	}
	return NO_EVENT;
}

_uint CMonster::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_pInfo->Get_Hp() <= 0.f || m_IsDead == true)
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
		_int iRand = rand() % 2;
		if (iRand == 0)
		{
			m_pManagement->PlaySound(L"Dialog_Kill.ogg", CSoundMgr::DIALOGUE1);
		}
		return DEAD_OBJECT;
	}
	if (nullptr == m_pHp_Bar)
	{
		m_IsCollide = false;
	}
	if (m_IsCollide) {
		// Bullet 데미지 만큼.
		CEffectHandler::Add_Layer_Effect_Bullet_Explosion(m_pTransform->Get_State(EState::Position));
		_float fDamage = _float(m_pInfo->Get_HittedDamage());
		_float fMaxHp = _float(m_pInfo->Get_MaxHp());
		m_pHp_Bar->Set_ScaleX((-fDamage / fMaxHp) * m_fHpLength);
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


	//거리 표시
	//if (true)
	//{
	//	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	//	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	//	_float3 vDir = vMonsterPos - vTargetPos;
	//	_int iDist = (_int)D3DXVec3Length(&vDir);
	//	if (iDist < 400.f)
	//	{
	//		D3DVIEWPORT9 vp2;
	//		m_pDevice->GetViewport(&vp2);
	//		_float4x4 TestView2, TestProj2;
	//		m_pDevice->GetTransform(D3DTS_VIEW, &TestView2);
	//		m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj2);
	//		_float4x4 matCombine2 = TestView2 * TestProj2;
	//		D3DXVec3TransformCoord(&vMonsterPos, &vMonsterPos, &matCombine2);
	//		vMonsterPos.x += 1.f;
	//		vMonsterPos.y += 1.f;

	//		vMonsterPos.x = (vp2.Width * (vMonsterPos.x)) / 2.f + vp2.X;
	//		vMonsterPos.y = (vp2.Height * (2.f - vMonsterPos.y) / 2.f + vp2.Y);

	//		_float3 ptBoss;
	//		ptBoss.x = vMonsterPos.x;
	//		ptBoss.y = vMonsterPos.y;
	//		ptBoss.z = 0.f;

	//		wstring str = to_wstring(iDist);
	//		wstring str2 = L"M";
	//		wstring combine = str + str2;
	//		RECT tUIBounds;
	//		GetClientRect(g_hWnd, &tUIBounds);
	//		tUIBounds.top += (_int)-ptBoss.y + (_int)(WINCY / 2);
	//		tUIBounds.left += (_int)ptBoss.x - (_int)(WINCX / 2);
	//		tUIBounds.right += (_int)ptBoss.x - (_int)(WINCX / 2) + 20;
	//		tUIBounds.bottom += (_int)-ptBoss.y + (_int)(WINCY / 2) + 20;
	//		m_pManagement->Get_Font()->DrawText(NULL
	//			, combine.c_str(), -1
	//			, &tUIBounds, DT_CENTER, D3DXCOLOR(100, 100, 100, 255));
	//	}
	//}
#ifdef _DEBUG // Render Collide
	for (auto& p : m_Collides)
		if (p) p->Render_Collide();
#endif

	return _uint();
}

void CMonster::Set_IsFight(_bool bFight)
{
	m_IsFight = bFight;
}

void CMonster::Get_Delivery()
{
	if (m_pManagement->Get_GameObject(L"Layer_Delivery") != nullptr)
	{
		m_pDeliveryTransform = (CTransform*)(m_pManagement->Get_Component(L"Layer_Delivery", L"Com_Transform"));
		if (m_pDeliveryTransform == nullptr)
		{
			PRINT_LOG(L"Error", L"m_pDeliveryTransform is  nullptr");
			return;
		}
		Safe_AddRef(m_pDeliveryTransform);
	}
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

	_float3 vDeliveryPos;
	_float3 vDir_D;
	_float fDist_D = 12345.f;
	_float fDist_Player_To_Deli = 0.f;

	if (m_pDeliveryTransform != nullptr)
	{
		vDeliveryPos	= m_pDeliveryTransform->Get_State(EState::Position);
		vDir_D			= vDeliveryPos - vMonsterPos;
		fDist_D			= D3DXVec3Length(&vDir_D);
		fDist_Player_To_Deli = D3DXVec3Length(&(vTargetPos - vDeliveryPos));
	}

	_float3 vDir = vTargetPos - vMonsterPos;
	_float fDist = D3DXVec3Length(&vDir);


	// 플레이어와 배틀상태 On

	m_eAttackTarget = EAttackTarget::End;

	if (fDist <= 300.f && fDist != 0.f)
	{
		Add_Hp_Bar(fDeltaTime);
		m_eAttackTarget = EAttackTarget::Player;
		m_bBattle = true;
	}

	// 호이차
	if (fDist_D <= 300.f && fDist_D != 0.f)
	{
		m_bBattle = true;
		Add_Hp_Bar(fDeltaTime);
		m_eAttackTarget = EAttackTarget::Delivery;

		// 범위 안에 있는데 플레이어가 있네
		if(fDist_Player_To_Deli <= DIST_PLAYER_TO_DELIVERY)
			m_eAttackTarget = EAttackTarget::Player;
	}


	/*
	+ 호위차 조건 추가
	플레이어를 때리는 경우
	- 인식 범위 내에 있다

	
	호위차 때리는 경우
	- 인식 범위 내에 있으며 플레이어가 주변에 없다

	*/


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


	_float3 vDeliveryPos;
	_float3 vDir_Delivery_To_Player;
	_float fDist_Delivery_To_Player = 0.f;
	_float3 vDir_Delivery;
	_float fDist_Delivery;

	if (m_pDeliveryTransform != nullptr)
	{
		// 호위, 플레이어 거리 체크
		vDeliveryPos			= m_pDeliveryTransform->Get_State(EState::Position);
		vDir_Delivery_To_Player = vDeliveryPos - vTargetPos;
		fDist_Delivery_To_Player = D3DXVec3Length(&vDir_Delivery_To_Player);

		// 나, 플레이어 거리 체크
		vDir_Delivery			= vDeliveryPos - vMonsterPos;
		fDist_Delivery			= D3DXVec3Length(&vDir_Delivery);
		D3DXVec3Normalize(&vDir_Delivery, &vDir_Delivery);
	}

	_float3 vLook = m_pTransform->Get_State(EState::Look);

	// 플레이어와 호위차의 거리가 작다면
	// 플레이어 공격
	// 아니면 호위차 공격
	_uint iStage = m_pManagement->Get_Current_Scene_Type();
	if (iStage == (_uint)ESceneType::Stage2)
	{
		m_eAttackTarget = EAttackTarget::Player;
	}
	if (m_eAttackTarget == EAttackTarget::Player)
	{
		// 플레이어가 호위차 주변에 잘 있으니 플레이어 공격
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
	}
	else if (m_eAttackTarget == EAttackTarget::Delivery)
	{
		// 플레이어가 딴짓거리 해서 호위 안할때 
		if (m_fPatternTime > 0.f && m_fPatternTime <= 7.f)
		{
			if (fDist_Delivery > 200.f)
			{
				m_pTransform->Go_Dir(vDir_Delivery, fDeltaTime * 4.f);
			}
			RotateToDelivery(fDeltaTime * 3.f);
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
			// 호위차 안바라보고 그 각도에서 회전이 안먹어요 하면서 직진
			if (fDist_Delivery < 500.f)
			{
				m_pTransform->Go_Straight(fDeltaTime * 10.f);
			}
			m_bAttack = false;
		}
		else if (m_fPatternTime > 10.f)
		{
			// 다시 호위차를 바라보렴
			RotateToDelivery(fDeltaTime * 3.f);
			m_fPatternTime = 0.f;
		}
	}


	m_fAttackDelay += fDeltaTime;

	if (m_fAttackDelay > 1.f && m_bAttack == true) // 여기서 공격 
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

_bool CMonster::RotateToDelivery(_float fDeltaTime)
{
	if (m_pDeliveryTransform == nullptr)
		return false;

	_float3 vTargetPos = m_pDeliveryTransform->Get_State(EState::Position);
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

	_bool IsLookingDeli = false;


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
		IsLookingDeli = true;
	}
	else
		IsLookingDeli = false;

	return IsLookingDeli;
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
		HUD_Hp_Bar.tTransformDesc.vScale = { m_pInfo->Get_Hp() * (m_fHpLength / m_pInfo->Get_MaxHp()), 8.f, 0.f };
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
		HUD_Hp_Bar_Border.tTransformDesc.vScale = { m_pInfo->Get_Hp() * (m_fHpLength / m_pInfo->Get_MaxHp()) + 2.5f, 12.f, 0.f };
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

		//Safe_Release(pGameObjectBorder);
		//Safe_Release(pGameObject);

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

	if (m_pHp_Bar)
		m_pHp_Bar->Set_Pos(vPosition);
	if (m_pHP_Bar_Border)
		m_pHP_Bar_Border->Set_Pos(vPosition);
	if (m_pLockOn)
	{
		if (m_pLockOn->Get_IsDead() == false)
		{
			if (GetAsyncKeyState(L'R') & 0x8000)
			{
				POINT pt = { (LONG)ptBoss.x, (LONG)ptBoss.y };
				//ScreenToClient(g_hWnd, &pt);
				SetCursorPos((_int)pt.x + 8, (_int)pt.y + 13);
			}
			m_pLockOn->Set_Pos(vLockOnPos);
		}
	}

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
	Safe_Release(m_pInfo);
	Safe_Release(m_pHp_Bar);
	Safe_Release(m_pHP_Bar_Border);
	Safe_Release(m_pLockOn);
	Safe_Release(m_pModelMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pDeliveryTransform);

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
	for (auto& p : m_Collides)
	{
		if (p == nullptr) continue;
		if (CCollision::IntersectRayToSphere(ray, p->Get_BoundingSphere()))
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
				m_pManagement->PlaySound(L"Lock_On.ogg", CSoundMgr::LOCKON);
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

				//Safe_Release(pLockOn);
			}
		}

	}

	return S_OK;
}
//
//void CMonster::Update_Effect()
//{
//	// Engine-Boost Effect
//	if (m_pLeftEngineEffect) {
//		_float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
//		vEnginePos += m_pTransform->Get_State(EState::Right) * m_vLeftEngineOffset.x;
//		vEnginePos += m_pTransform->Get_State(EState::Up) * m_vLeftEngineOffset.y;
//		vEnginePos += m_pTransform->Get_State(EState::Look) * m_vLeftEngineOffset.z;
//		m_pLeftEngineEffect->Set_EngineOffset(vEnginePos);
//		m_pLeftEngineEffect->Set_IsBoost(m_IsBoost);
//	}
//	if (m_pRightEngineEffect) {
//		_float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
//		vEnginePos += m_pTransform->Get_State(EState::Right) * m_vRightEngineOffset.x;
//		vEnginePos += m_pTransform->Get_State(EState::Up) * m_vRightEngineOffset.y;
//		vEnginePos += m_pTransform->Get_State(EState::Look) * m_vRightEngineOffset.z;
//		m_pRightEngineEffect->Set_EngineOffset(vEnginePos);
//		m_pRightEngineEffect->Set_IsBoost(m_IsBoost);
//	}
//
//	// Wing-Boost Effect
//
//	if (m_pLeftWingBoost) {
//		_float3 vWingPos = m_pTransform->Get_TransformDesc().vPosition;
//		vWingPos += m_pTransform->Get_State(EState::Right) * m_vLeftWingOffset.x;
//		vWingPos += m_pTransform->Get_State(EState::Up) * m_vLeftWingOffset.y;
//		vWingPos += m_pTransform->Get_State(EState::Look) * m_vLeftWingOffset.z;
//		m_pLeftWingBoost->Set_WingOffset(vWingPos);
//		m_pLeftWingBoost->Set_IsBoost(m_IsBoost);
//	}
//	if (m_pRightWingBoost) {
//		_float3 vWingPos = m_pTransform->Get_TransformDesc().vPosition;
//		vWingPos += m_pTransform->Get_State(EState::Right) * m_vRightWingOffset.x;
//		vWingPos += m_pTransform->Get_State(EState::Up) * m_vRightWingOffset.y;
//		vWingPos += m_pTransform->Get_State(EState::Look) * m_vRightWingOffset.z;
//		m_pRightWingBoost->Set_WingOffset(vWingPos);
//		m_pRightWingBoost->Set_IsBoost(m_IsBoost);
//	}
//	
//}
