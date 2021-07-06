#include "stdafx.h"
#include "..\Headers\Boss_Monster.h"
#include "Bullet_EnergyBall.h"

CBoss_Monster::CBoss_Monster(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData)
	: CGameObject(pDevice)
{
	m_pPassData = pData;
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
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_CubeTexture",
		L"Com_CubeTexture",
		(CComponent**)&m_pCube)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_CubeTexture");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_TestCube",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = _float3(100.f, 3.f, 50.f);
	TransformDesc.fSpeedPerSec = 2.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(10.f);
	TransformDesc.vScale = { 10.f,10.f,30.f };


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



	return S_OK;
}

_uint CBoss_Monster::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	Move_AI(fDeltaTime);
	//Attack_AI(fDeltaTime);

	//Spawn_Monster(fDeltaTime);

	//Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CBoss_Monster::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_IsCollide)
	{
		m_IsCollide = false;
	}

	return _uint();
}

_uint CBoss_Monster::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pCube->Render_VIBuffer();

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
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

CBoss_Monster * CBoss_Monster::Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData /*= nullptr*/)
{
	CBoss_Monster* pInstance = new CBoss_Monster(pDevice, pData);
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
	Safe_Release(m_pTargetTransform);

	Safe_Release(m_pCube);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}

HRESULT CBoss_Monster::Add_InLayer_MyParts()
{



	return S_OK;
}
