#include "stdafx.h"
#include "..\Headers\Boss_Monster.h"
#include "Bullet_EnergyBall.h"
#include "HP_Bar.h"
#include "HP_Bar_Border.h"
#include "Pipeline.h"
#include "Collision.h"
#include "New_LockOn.h"

CBoss_Monster::CBoss_Monster(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CBoss_Monster::CBoss_Monster(const CBoss_Monster & other)
	: CGameObject(other)
	, m_eActionMode(other.m_eActionMode)
	, m_IsSpecialAction(other.m_IsSpecialAction)
	, m_fLaser_CoolTime(other.m_fLaser_CoolTime)
	, m_fEmpBomb_CoolTime(other.m_fEmpBomb_CoolTime)
	, m_fCannonLength(other.m_fCannonLength)
	, m_fLaser_Degree(other.m_fLaser_Degree)
	, m_IsLaserAlert(other.m_IsLaserAlert)
	, m_IsLaserTarget(other.m_IsLaserTarget)
	, m_IsFireEmp(other.m_IsFireEmp)
	, m_IsFight(other.m_IsFight)
{
}

HRESULT CBoss_Monster::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CBoss_Monster::Ready_GameObject(void * pArg/* = nullptr*/)
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
	wstring meshTag = L"Component_Mesh_Boss";
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		meshTag,
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_CubeTexture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc = pDesc->tTransformDesc;
	//TransformDesc.vPosition = _float3(500.f, 3.f, 50.f);
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(30.f);

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
	CStreamHandler::Load_PassData_Collide(L"Boss", meshTag, tCollide);
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

	m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pTargetTransform);
	if (nullptr == m_pTargetTransform)
	{
		PRINT_LOG(L"Error", L"m_pTargetTransform is nullptr");
		return E_FAIL;
	}

	// HP 세팅
	STAT_INFO tStatus;
	tStatus.iMaxHp = 21000;
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

	//m_fHp = 900.f;
	//m_fFullHp = m_fHp;

	return S_OK;
}

_uint CBoss_Monster::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	Check_NewPattern();
	Transform_Check();

	if (m_IsFight == true)
	{
		Move_AI(fDeltaTime);
		Attack_AI(fDeltaTime);
	}

	if (!m_IsHPBar)
		Add_Hp_Bar(fDeltaTime);

	if (m_pHp_Bar != nullptr && m_pHP_Bar_Border != nullptr)
	{
		Set_Hp_Pos();
		Check_Degree();
	}
	m_pTransform->Update_Transform();
	// 충돌박스 업데이트
	for (auto& collide : m_Collides)
		collide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);

	/* 얘가 문제 */
	Make_LockOn();

	return NO_EVENT;
}

_uint CBoss_Monster::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_pInfo->Get_Hp() <= 0)
	{
		_float3 vMyPos = m_pTransform->Get_State(EState::Position);
		_float3 vLook = m_pTransform->Get_State(EState::Look);
		D3DXVec3Normalize(&vLook, &vLook);
		_float3 vBack = vMyPos - vLook * 30.f;
		_float3 vFront = vMyPos - vLook * 30.f;

		CEffectHandler::Add_Layer_Effect_Explosion(vBack, 4.f);
		CEffectHandler::Add_Layer_Effect_Explosion(vFront, 4.f);

		m_IsDead = true;
		if (m_pHp_Bar)
			m_pHp_Bar->Set_IsDead(TRUE);
		if (m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_IsDead(TRUE);
		if (m_pLockOn)
			m_pLockOn->Set_IsDead(TRUE);
		m_pManagement->PlaySound(L"Boss_Dead1.ogg", CSoundMgr::SHIP_EXPLOSION);

		return DEAD_OBJECT;
	}
	if (nullptr == m_pHp_Bar)
	{
		m_IsCollide = false;
	}
	if (m_IsCollide) {
		// Bullet 데미지 만큼.
		_float fDamage = _float(m_pInfo->Get_HittedDamage());
		_float fMaxHp = _float(m_pInfo->Get_MaxHp());
		m_pHp_Bar->Set_ScaleX((-fDamage / fMaxHp) * m_fHpLength);
		m_IsCollide = false;
	}

	return _uint();
}

_uint CBoss_Monster::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();

#ifdef _DEBUG // Render Collide
	for (auto& collide : m_Collides)
		collide->Render_Collide();
#endif

	return _uint();
}

void CBoss_Monster::Set_BossFight(_bool bFight)
{
	m_IsFight = bFight;
}

_uint CBoss_Monster::Movement(_float fDeltaTime)
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

	_float3 vMyRight, vMyLeft;
	D3DXVec3Cross(&vMyRight, &vMyUp, &vMyLook);
	D3DXVec3Cross(&vMyLeft, &vMyLook, &vMyUp);

	D3DXVec3Normalize(&vMyRight, &vMyRight);
	D3DXVec3Normalize(&vMyLeft, &vMyLeft);

	_float fRight = D3DXVec3Dot(&vTargetDir, &vMyRight);
	_float fLeft = D3DXVec3Dot(&vTargetDir, &vMyLeft);

	if (fRight < fLeft)
		m_pTransform->RotateY(-fDeltaTime);

	else
		m_pTransform->RotateY(fDeltaTime);


	m_pTransform->Go_Straight(fDeltaTime);

	return _uint();
}

_uint CBoss_Monster::Move_Near(_float fDeltaTime)
{
	m_pTransform->Go_Straight(fDeltaTime);

	return _uint();
}

_uint CBoss_Monster::Move_Middle(_float fDeltaTime)
{
	RotateMy_Y(fDeltaTime);


	return _uint();
}

_uint CBoss_Monster::Move_Far(_float fDeltaTime)
{
	RotateMy_Y(fDeltaTime);
	RotateMy_X(fDeltaTime);

	m_pTransform->Go_Straight(fDeltaTime);


	return _uint();
}

_uint CBoss_Monster::EnergyBallCannon_Target_Search(_float fDeltaTime)
{
	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	_float3 vMyPos = m_vMyPos;
	_float3 vUp = m_pTransform->Get_State(EState::Up);
	_float3 vLook = m_pTransform->Get_State(EState::Look);
	_float3 vRight;

	D3DXVec3Normalize(&vLook, &vLook);
	D3DXVec3Normalize(&vUp, &vUp);

	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	vMyPos += vLook * 6.f;

	m_vRight_EnergyBallCannon_Position = vMyPos - (vRight * 70.f);
	m_vLeft_EnergyBallCannon_Position = vMyPos + (vRight * 70.f);

	_float fRightCannonToTarget_Length = D3DXVec3Length(&(vTargetPos - m_vRight_EnergyBallCannon_Position));
	_float fLeftCannonToTarget_Length = D3DXVec3Length(&(vTargetPos - m_vLeft_EnergyBallCannon_Position));
	_float fOtherCannon = fabs(fRightCannonToTarget_Length - fLeftCannonToTarget_Length);

	// 일단 짧은놈은 발사, 오른쪽 기준
	if (fRightCannonToTarget_Length < fLeftCannonToTarget_Length)
	{
		Right_EnergyBall(fDeltaTime);

		if (fOtherCannon <= m_fCannonLength)
		{
			Left_EnergyBall(fDeltaTime);
		}
	}
	else
	{
		Left_EnergyBall(fDeltaTime);

		if (fOtherCannon <= m_fCannonLength)
		{
			Right_EnergyBall(fDeltaTime);
		}
	}

	return _uint();
}

_uint CBoss_Monster::Left_EnergyBall(_float fDeltaTime)
{
	m_fLeftCannonCoolTime += fDeltaTime;

	if (m_fLeftCannonCoolTime >= 2.f)
	{
		m_fLeftCannonCoolTime = 0.f;

		TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

		pArg->vPosition = m_vLeft_EnergyBallCannon_Position;
		pArg->vRotate = m_pTransform->Get_TransformDesc().vRotate;
		CEffectHandler::Add_Layer_Effect_Boss_FireBullet(pArg->vPosition, 1.f);

		m_pManagement->PlaySound(L"Boss_Energy_Ball.ogg", CSoundMgr::BOSS_ATTACK_1);

		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Bullet_EnergyBall",
			L"Layer_Bullet_EnergyBall", pArg)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Bullet_EnergyBall In Layer");
			return E_FAIL;
		}
	}

	return S_OK;
}

_uint CBoss_Monster::Right_EnergyBall(_float fDeltaTime)
{
	m_fRightCannonCoolTime += fDeltaTime;

	if (m_fRightCannonCoolTime >= 2.f)
	{
		m_fRightCannonCoolTime = 0.f;

		TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

		pArg->vPosition = m_vRight_EnergyBallCannon_Position;
		pArg->vRotate = m_pTransform->Get_TransformDesc().vRotate;
		CEffectHandler::Add_Layer_Effect_Boss_FireBullet(pArg->vPosition, 1.f);

		m_pManagement->PlaySound(L"Boss_Energy_Ball.ogg", CSoundMgr::BOSS_ATTACK_1);

		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Bullet_EnergyBall",
			L"Layer_Bullet_EnergyBall", pArg)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Bullet_EnergyBall In Layer");
			return E_FAIL;
		}
	}

	return S_OK;
}

_uint CBoss_Monster::Fire_Laser(_float fDeltaTime)
{
	m_fLaser_CoolTime += fDeltaTime;

	// 3초의 사이클
	if (m_fLaser_CoolTime >= 3.f)
	{
		m_fLaser_CoolTime = 0.f;
		m_IsLaserAlert = true;
	}

	// 경고 이펙트 / 조준
	if (m_fLaser_CoolTime >= 2.2f)
	{
		if (m_IsLaserAlert == true)
		{
			m_IsLaserAlert = false;
			m_IsLaserTarget = false;
			m_IsLaserAttack = false;

			_float3 vMyPos = m_vMyPos;
			_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);

			_float3 vDir = vTargetPos - vMyPos;
			D3DXVec3Normalize(&vDir, &vDir);

			_float3 vUp = m_pTransform->Get_State(EState::Up);
			_float3 vLook = m_pTransform->Get_State(EState::Look);
			D3DXVec3Normalize(&vUp, &vUp);
			D3DXVec3Normalize(&vLook, &vLook);

			// Look 방향으로 1이 정면
			_float fTheta_Z = D3DXVec3Dot(&vDir, &vLook);
			_float fLaser_Radian_Z = D3DXToRadian(m_fLaser_Degree);
			_float fRadian_Min_Z = 1.f - fLaser_Radian_Z;
			_float fRadian_Max_Z = fLaser_Radian_Z + 1.f;

			// 위 아래로 한번 더 걸러야 함
			// 기준이 Z축이니 Y와의 내적은 0이 나옴(법선 일때)
			_float fTheta_Y = D3DXVec3Dot(&vDir, &vUp);
			_float fLaser_Radian_Y = D3DXToRadian(m_fLaser_Degree);
			_float fRadian_Max_Y = 1.f - fLaser_Radian_Y;
			_float fRadian_Min_Y = fLaser_Radian_Y - 1.f;

			if (0.15f >= fTheta_Y &&
				fTheta_Y >= -0.45f)
			{
				if (fRadian_Min_Z <= fTheta_Z &&
					fTheta_Z <= fRadian_Max_Z)
				{
					m_IsLaserAttack = true;
					m_IsLaserTarget = true;

				}
			}


			if (m_IsLaserAttack == true &&
				m_IsLaserTarget == true)
			{
				m_IsLaserTarget = false;
				m_iLaserCount = 0;

				_float3 vMyPos = m_vMyPos;
				_float3 vUp = m_pTransform->Get_State(EState::Up);
				_float3 vLook = m_pTransform->Get_State(EState::Look);
				D3DXVec3Normalize(&vUp, &vUp);
				D3DXVec3Normalize(&vLook, &vLook);

				vMyPos -= vUp * 20.f;
				m_vLaserCannon_Position = vMyPos + (vLook * 125.f);
				_float3 vEffectPos = m_vLaserCannon_Position + (vLook * 3.f);

				CEffectHandler::Add_Layer_Effect_BossBullet_Laser_Alert(vEffectPos, 1.f);
			}
		}
	}

	// 레이저 발사 1개
	if (m_fLaser_CoolTime >= 2.7f)
	{
		if (m_IsLaserAttack == true)
		{
			if (m_iLaserCount == 0)
			{
				//m_IsLaserAttack = false;
				//m_fLaser_CoolTime = 0.f;
				TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

				_float3 vMyPos = m_vMyPos;
				_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
				_float3 vRight = m_pTransform->Get_State(EState::Right);
				_float3 vUp = m_pTransform->Get_State(EState::Up);
				_float3 vLook = m_pTransform->Get_State(EState::Look);

				_float3 vDir = vTargetPos - vMyPos;
				D3DXVec3Normalize(&vDir, &vDir);

				//_float3 vTheta	=	{ 1.f, 1.f, 1.f };
				//_float2 vThetaXY_Z = { 1.f, 0.f };
				//_float2 vDirXY = { vDir.x, vDir.y };
				//_float2 vThetaYZ_X = { 1.f, 0.f };
				//_float2 vDirYZ = { vDir.y, vDir.z };
				//_float2 vThetaZX_Y = { 1.f, 0.f };
				//_float2 vDirZX = { vDir.z, vDir.x };
				////vTheta.z = D3DXVec2Dot(&vThetaXY_Z, &vDirXY);
				////vTheta.x = D3DXVec2Dot(&vThetaYZ_X, &vDirYZ);
				////vTheta.y = D3DXVec2Dot(&vThetaZX_Y, &vDirZX);
				////vTheta.x = (D3DXVec3Dot(&vDir, &vRight));
				//vTheta.y = (D3DXVec3Dot(&vDir, &vUp));
				////vTheta.z = (D3DXVec3Dot(&vDir, &vLook));
				//pArg->vRotate = vTheta;

				//if (m_iLaserCount < 1)
				//{
				vMyPos -= vUp * 20.f;
				m_vLaserCannon_Position = vMyPos + (vLook * 125.f);
				//}

				pArg->vPosition = m_vLaserCannon_Position;

				++m_iLaserCount;


				if (FAILED(m_pManagement->Add_GameObject_InLayer(
					EResourceType::NonStatic,
					L"GameObject_Bullet_Laser",
					L"Layer_Bullet_Laser", pArg)))
				{
					PRINT_LOG(L"Error", L"Failed To Add Bullet_Laser In Layer");
					return E_FAIL;
				}
				_float3 vEffectPos = m_vLaserCannon_Position + (vLook * 5.f);
				m_pManagement->PlaySound(L"Boss_Laser.ogg", CSoundMgr::BOSS_ATTACK_2);
				CEffectHandler::Add_Layer_Effect_Boss_FireBullet(vEffectPos, 1.f);
			}
		}
	}

	return _uint();
}

_uint CBoss_Monster::Fire_EMP(_float fDeltaTime)
{
	m_fEmpBomb_CoolTime += fDeltaTime;

	if (m_fEmpBomb_CoolTime >= 7.f)
	{
		m_fEmpBomb_CoolTime = 0.f;

		TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

		_float3 vUp = m_pTransform->Get_State(EState::Up);
		_float3 vLook = m_pTransform->Get_State(EState::Look);
		D3DXVec3Normalize(&vUp, &vUp);
		D3DXVec3Normalize(&vLook, &vLook);

		pArg->vPosition = m_pTransform->Get_State(EState::Position) + (vUp * -25.f) + (vLook * 40.f);
		pArg->vRotate = m_pTransform->Get_TransformDesc().vRotate;

		m_vEmpBomb_Position;

		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Bullet_EMP_Bomb",
			L"Layer_Bullet_EMP_Bomb", pArg)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Bullet_EMP_Bomb In Layer");
			return E_FAIL;
		}

		m_pManagement->PlaySound(L"Boss_EMP_Bomb.ogg", CSoundMgr::BOSS_ATTACK_3);
		CEffectHandler::Add_Layer_Effect_Boss_FireBullet(pArg->vPosition, 1.f);

	}



	return _uint();
}

_uint CBoss_Monster::Spawn_Monster(_float fDeltaTime)
{
	m_fSpawnCoolTime += fDeltaTime;

	if (m_fSpawnCoolTime >= 2.f)
	{
		m_fSpawnCoolTime = 0.f;

		TRANSFORM_DESC* pArg = new TRANSFORM_DESC;
		_float3 vMyPos = m_vMyPos;
		_float3 vUp = m_pTransform->Get_State(EState::Up);
		_float3 vLook = m_pTransform->Get_State(EState::Look);

		_float3 vRight, vLeft;

		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Normalize(&vUp, &vUp);

		D3DXVec3Cross(&vRight, &vUp, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		vMyPos -= vLook * 8.f;

		pArg->vPosition = vMyPos;
		pArg->vRotate = m_pTransform->Get_TransformDesc().vRotate;


		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Boss_Spawn_Monster",
			L"Layer_Boss_Spawn_Monster", pArg)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Boss_Spawn_Monster In Layer");
			return E_FAIL;
		}
	}

	return _uint();
}

_uint CBoss_Monster::Spawn_Wormhole(_float fDeltaTime)
{
	return _uint();
}

_uint CBoss_Monster::Transform_Check()
{
	m_vMyPos = m_pTransform->Get_State(EState::Position);

	return _uint();
}

_uint CBoss_Monster::Move_AI(_float fDeltaTime)
{
	_float fDis = fabs(D3DXVec3Length(&m_pTargetTransform->Get_State(EState::Position))
		- D3DXVec3Length(&m_pTransform->Get_State(EState::Position)));

	//
	if (fDis < BOSSRANGE_NEAR)
		m_eActionMode = Near;

	else if (BOSSRANGE_NEAR < fDis && fDis < BOSSRANGE_MIDDLE)
		m_eActionMode = Middle;

	else if (BOSSRANGE_MIDDLE < fDis && fDis < BOSSRANGE_FAR)
		m_eActionMode = Far;

	else
		m_eActionMode = End;

	if (m_IsSpecialAction == true)
		m_eActionMode = SpecialAction;


	switch (m_eActionMode)
	{
	case CBoss_Monster::Near:
		Move_Near(fDeltaTime);
		break;
	case CBoss_Monster::Middle:
		Move_Middle(fDeltaTime);
		break;
	case CBoss_Monster::Far:
		Move_Far(fDeltaTime);
		break;
	case CBoss_Monster::SpecialAction:
		break;
	default:
		break;
	}

	return _uint();
}

_uint CBoss_Monster::Attack_AI(_float fDeltaTime)
{
	_float fDis = fabs(D3DXVec3Length(&m_pTargetTransform->Get_State(EState::Position))
		- D3DXVec3Length(&m_pTransform->Get_State(EState::Position)));

	if (fDis < BOSSRANGE_FAR)
	{
		EnergyBallCannon_Target_Search(fDeltaTime);
		Fire_Laser(fDeltaTime);
		
		if (m_IsFireEmp == true)
			Fire_EMP(fDeltaTime);
	}

	return _uint();
}

_uint CBoss_Monster::Check_NewPattern()
{
	_float fNowHp = _float(m_pInfo->Get_Hp()) / _float(m_pInfo->Get_MaxHp());

	if (fNowHp <= 0.7f)
	{
		m_IsFireEmp = true;
	}

	return _uint();
}

void CBoss_Monster::RotateMy_X(_float fDeltaTime)
{
	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	_float3 vMyPos = m_pTransform->Get_State(EState::Position);

	_float3 vTargetDir = vTargetPos - vMyPos;

	_float3 vMyLook = m_pTransform->Get_State(EState::Look);
	_float3 vMyRight = m_pTransform->Get_State(EState::Right);

	_float3 vMyUp, vMyDown;
	D3DXVec3Cross(&vMyUp, &vMyLook, &vMyRight);
	D3DXVec3Cross(&vMyDown, &vMyRight, &vMyLook);

	D3DXVec3Normalize(&vMyUp, &vMyUp);
	D3DXVec3Normalize(&vMyDown, &vMyDown);
	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float fUpScala = D3DXVec3Dot(&vTargetDir, &vMyUp);
	_float fDownScala = D3DXVec3Dot(&vTargetDir, &vMyDown);

	_float fTheta = D3DXVec3Dot(&vTargetDir, &vMyLook);
	_float fRadianMax = D3DXToRadian(95.f);
	_float fRadianMin = D3DXToRadian(85.f);

	_int i = 0;

	if (fTheta < 0.01f && fTheta > -0.99f)
		return;

	if (fUpScala < fDownScala)
		m_pTransform->RotateX(fDeltaTime);

	else
		m_pTransform->RotateX(-fDeltaTime);
}

void CBoss_Monster::RotateMy_Y(_float fDeltaTime)
{
	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	_float3 vMyPos = m_pTransform->Get_State(EState::Position);

	_float3 vTargetDir = vTargetPos - vMyPos;
	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float3 vMyLook = m_pTransform->Get_State(EState::Look);
	_float3 vMyUp = m_pTransform->Get_State(EState::Up);
	D3DXVec3Normalize(&vMyLook, &vMyLook);

	_float fTheta = D3DXVec3Dot(&vTargetDir, &vMyLook);
	_float fRadianMax = D3DXToRadian(95.f);
	_float fRadianMin = D3DXToRadian(85.f);

	_float3 vMyRight, vMyLeft;
	D3DXVec3Cross(&vMyRight, &vMyUp, &vMyLook);
	D3DXVec3Cross(&vMyLeft, &vMyLook, &vMyUp);

	D3DXVec3Normalize(&vMyRight, &vMyRight);
	D3DXVec3Normalize(&vMyLeft, &vMyLeft);

	_float fRight = D3DXVec3Dot(&vTargetDir, &vMyRight);
	_float fLeft = D3DXVec3Dot(&vTargetDir, &vMyLeft);

	if (fTheta > 0.99f && fTheta < 1.01f)
		return;

	if (fRight < fLeft)
		m_pTransform->RotateY(-fDeltaTime);

	else
		m_pTransform->RotateY(fDeltaTime);
}

void CBoss_Monster::RotateMy_Z(_float fDeltaTime)
{
}

_uint CBoss_Monster::Add_Hp_Bar(_float fDeltaTime)
{
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	_float3 vPlayerPos = m_pTargetTransform->Get_State(EState::Position);

	_float3 vDir = vMonsterPos - vPlayerPos;
	_float fDist = D3DXVec3Length(&vDir);

	if (fDist < 800.f && fDist != 0.f)
	{
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
			m_pHP_Bar_Border->Who_Make_Me(m_pHP_Bar_Border->MAKER_BOSS_MONSTER);

			m_pHp_Bar = static_cast<CHP_Bar*>(pGameObject);
			m_pHp_Bar->Who_Make_Me(m_pHp_Bar->MAKER_BOSS_MONSTER);

			//Safe_Release(pGameObject);
			//Safe_Release(pGameObjectBorder);
		}

	}
	return _uint();
}

void CBoss_Monster::Set_Hp_Pos()
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
	if (m_pHp_Bar)
		m_pHp_Bar->Set_Pos(vPosition);
	if (m_pHP_Bar_Border)
		m_pHP_Bar_Border->Set_Pos(vPosition);
	if (m_pLockOn)
		m_pLockOn->Set_Pos(vLockOnPos);

}

CBoss_Monster* CBoss_Monster::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CBoss_Monster* pInstance = new CBoss_Monster(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Boss_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss_Monster::Clone(void * pArg/* = nullptr*/)
{
	CBoss_Monster* pClone = new CBoss_Monster(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Boss_Monster");
		Safe_Release(pClone);
	}

	return pClone;
}

void CBoss_Monster::Free()
{
	//if(m_pLockOn)
	//	m_pLockOn->Set_IsDead(TRUE);
	//Safe_Release(m_pLockOn);

	Safe_Release(m_pInfo);
	Safe_Release(m_pHP_Bar_Border);
	Safe_Release(m_pHp_Bar);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);


	CGameObject::Free();
}

_uint CBoss_Monster::Check_Degree()
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

	_float fDegree = D3DXToDegree(fCeta); //

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

_uint CBoss_Monster::Make_LockOn()
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

	if (CCollision::IntersectRayToSphere(ray, m_Collides.front()->Get_BoundingSphere()))
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
			m_pLockOn->Who_Make_Me(m_pLockOn->MAKER_BOSS_MONSTER);
			//Safe_Release(pLockOn);
		}
	}
	return S_OK;
}
