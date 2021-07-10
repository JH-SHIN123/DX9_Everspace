#include "stdafx.h"
#include "..\Headers\Boss_Monster.h"
#include "Bullet_EnergyBall.h"
#include "HP_Bar.h"
#include "HP_Bar_Border.h"

CBoss_Monster::CBoss_Monster(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CBoss_Monster::CBoss_Monster(const CBoss_Monster & other)
	: CGameObject(other)
	, m_eActionMode(other.m_eActionMode)
	, m_IsSpecialAction(other.m_IsSpecialAction)
	, m_IsLeftFire(other.m_IsLeftFire)
	, m_fEnergyBall_CoolTime(other.m_fEnergyBall_CoolTime)
	, m_fLaser_CoolTime(other.m_fLaser_CoolTime)
	, m_fEmpBomb_CoolTime(other.m_fEmpBomb_CoolTime)
{
	//, m_fDetectionRange_Near(other.m_fDetectionRange_Near)
	//, m_fDetectionRange_Middle(other.m_fDetectionRange_Middle)
	//, m_fDetectionRange_Far(other.m_fDetectionRange_Far)
	
}

HRESULT CBoss_Monster::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CBoss_Monster::Ready_GameObject(void * pArg/* = nullptr*/)
{
	Add_InLayer_MyParts();

	CGameObject::Ready_GameObject(pArg);

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
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = _float3(500.f, 3.f, 50.f);
	TransformDesc.fSpeedPerSec = 2.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(10.f);

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

	//m_pGunTranform[0] = (CTransform*)m_pManagement->Get_Component(L"Layer_Boss_Monster_Has_A_EnergyBall_LEFT", L"Com_Transform");
	//Safe_AddRef(m_pGunTranform[0]);
	//m_pGunTranform[1] = (CTransform*)m_pManagement->Get_Component(L"Layer_Boss_Monster_Has_A_EnergyBall_RIGHT", L"Com_Transform");
	//Safe_AddRef(m_pGunTranform[1]);
	//if (nullptr == m_pGunTranform[0] || nullptr == m_pGunTranform[1])
	//{
	//	PRINT_LOG(L"Error", L"m_pGunTranform is nullptr");
	//	return E_FAIL;
	//}

	// HP 세팅
	m_fHp = 900.f;
	m_fFullHp = m_fHp;


	return S_OK;
}

_uint CBoss_Monster::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	Move_AI(fDeltaTime);
	//Attack_AI(fDeltaTime);

	//Spawn_Monster(fDeltaTime);

	//Movement(fDeltaTime);

	if(!m_IsHPBar)
		Add_Hp_Bar(fDeltaTime);

	m_pTransform->Update_Transform();
	// 충돌박스 업데이트
	for (auto& collide : m_Collides)
		collide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);

	return NO_EVENT;
}

_uint CBoss_Monster::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_fHp <= 0.f)
	{
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);
		m_IsDead = true;
		m_pHp_Bar->Set_IsDead(TRUE);
		m_pHP_Bar_Border->Set_IsDead(TRUE);
		m_pManagement->PlaySound(L"Ship_Explosion.ogg", CSoundMgr::SHIP_EXPLOSION);
		return DEAD_OBJECT;
	}
	if (m_IsCollide) {
		// Bullet 데미지 만큼.
 		m_pHp_Bar->Set_ScaleX(-100.f / m_fFullHp * m_fHpLength);
		m_fHp -= 100.f;
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


	return _uint();
}

_uint CBoss_Monster::Move_Middle(_float fDeltaTime)
{



	return _uint();
}

_uint CBoss_Monster::Move_Far(_float fDeltaTime)
{
	RotateMy_Y(fDeltaTime);
	RotateMy_X(fDeltaTime);

	m_pTransform->Go_Straight(fDeltaTime);


	return _uint();
}

_uint CBoss_Monster::Fire_Triger(_float fDeltaTime)
{
	m_fEnergyBall_CoolTime += fDeltaTime;

	if (m_fEnergyBall_CoolTime >= 2.f)
	{
		m_fEnergyBall_CoolTime = 0.f;

		TRANSFORM_DESC* pArg = new TRANSFORM_DESC;
		_float3 vPos = m_pTransform->Get_State(EState::Position);
		_float3 vUp = m_pTransform->Get_State(EState::Up);
		_float3 vLook = m_pTransform->Get_State(EState::Look);

		_float3 vRight, vLeft;

		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Normalize(&vUp, &vUp);

		D3DXVec3Cross(&vRight, &vUp, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		vPos += vLook * 8.f;

		if (m_IsLeftFire == true)
			vPos -= vRight * 8.f;

		if (m_IsLeftFire == false)
			vPos += vRight * 8.f;

		m_IsLeftFire = !m_IsLeftFire;

		pArg->vPosition = vPos;
		pArg->vRotate = m_pTransform->Get_TransformDesc().vRotate;

		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Bullet_EnergyBall",
			L"Layer_Bullet_EnergyBall", pArg)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Bullet_EnergyBall In Layer");
			return E_FAIL;
		}
	}

	return _uint();
}

_uint CBoss_Monster::Fire_Laser(_float fDeltaTime)
{
	m_fLaser_CoolTime += fDeltaTime;

	// 3초의 사이클
	if (m_fLaser_CoolTime >= 3.f)
		m_fLaser_CoolTime = 0.f;

	// 2초가 넘어가면 발사
	// 2초의 쿨타임으로 1초동안 발사
	if (m_fLaser_CoolTime >= 2.5f)
	{
		TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

		_float3 vUp = m_pTransform->Get_State(EState::Up);
		D3DXVec3Normalize(&vUp, &vUp);

		pArg->vPosition = m_pTransform->Get_State(EState::Position) + (vUp * 2.f);

		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Bullet_Laser",
			L"Layer_Bullet_Laser", pArg)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Bullet_Laser In Layer");
			return E_FAIL;
		}

	}

	return _uint();
}

_uint CBoss_Monster::Fire_EMP(_float fDeltaTime)
{
	m_fEmpBomb_CoolTime += fDeltaTime;



	if (m_fEmpBomb_CoolTime >= 5.f)
	{
		m_fEmpBomb_CoolTime = 0.f;

		TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

		_float3 vUp = m_pTransform->Get_State(EState::Up);
		D3DXVec3Normalize(&vUp, &vUp);

		pArg->vPosition = m_pTransform->Get_State(EState::Position) + (vUp * 2.f);

		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Bullet_EMP_Bomb",
			L"Layer_Bullet_EMP_Bomb", pArg)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Bullet_EMP_Bomb In Layer");
			return E_FAIL;
		}
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
		_float3 vPos = m_pTransform->Get_State(EState::Position);
		_float3 vUp = m_pTransform->Get_State(EState::Up);
		_float3 vLook = m_pTransform->Get_State(EState::Look);

		_float3 vRight, vLeft;

		D3DXVec3Normalize(&vLook, &vLook);
		D3DXVec3Normalize(&vUp, &vUp);

		D3DXVec3Cross(&vRight, &vUp, &vLook);
		D3DXVec3Normalize(&vRight, &vRight);

		vPos -= vLook * 8.f;

		pArg->vPosition = vPos;
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
		return UPDATE_ERROR;
		break;
	}



	return _uint();
}

_uint CBoss_Monster::Attack_AI(_float fDeltaTime)
{
	switch (m_eActionMode)
	{
	case CBoss_Monster::Near:
		Fire_Triger(fDeltaTime);
		break;
	case CBoss_Monster::Middle:
		Fire_Laser(fDeltaTime);
		Fire_Triger(fDeltaTime);
		//Spawn_Monster(fDeltaTime);
		break;
	case CBoss_Monster::Far:
		Fire_EMP(fDeltaTime);
		Fire_Laser(fDeltaTime);
		break;
	case CBoss_Monster::SpecialAction:
		break;
	default:
		return UPDATE_ERROR;
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

	_float fUpScala = D3DXVec3Dot(&vTargetDir, &vMyUp);
	_float fDownScala = D3DXVec3Dot(&vTargetDir, &vMyDown);

	_float fCeta = D3DXVec3Dot(&vTargetDir, &vMyLook);
	_float fRadianMax = D3DXToRadian(95.f);
	_float fRadianMin = D3DXToRadian(85.f);


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

	//if (fCeta < fRadianMin)
	//{
		if (fRight < fLeft)
			m_pTransform->RotateY(-fDeltaTime);

		else
			m_pTransform->RotateY(fDeltaTime);
	//}
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

	if (fDist < 300.f)
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
			m_pHP_Bar_Border->Who_Make_Me(m_pHP_Bar_Border->MAKER_BOSS_MONSTER);

			m_pHp_Bar = static_cast<CHP_Bar*>(pGameObject);
			m_pHp_Bar->Who_Make_Me(m_pHp_Bar->MAKER_BOSS_MONSTER);
		}

	}
	return _uint();
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
	Safe_Release(m_pHP_Bar_Border);
	Safe_Release(m_pHp_Bar);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);

	CGameObject::Free();
}

HRESULT CBoss_Monster::Add_InLayer_MyParts()
{



	return S_OK;
}
