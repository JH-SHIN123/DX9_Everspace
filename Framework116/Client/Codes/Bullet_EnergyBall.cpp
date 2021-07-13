#include "stdafx.h"
#include "..\Headers\Bullet_EnergyBall.h"
#include "EffectHandler.h"

CBullet_EnergyBall::CBullet_EnergyBall(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData)
	: CGameObject(pDevice)
{
}

CBullet_EnergyBall::CBullet_EnergyBall(const CBullet_EnergyBall & other)
	: CGameObject(other)
	, m_fTrackingTime(other.m_fTrackingTime)
	, m_IsTracking(other.m_IsTracking)
	, m_fLiveTime(other.m_fLiveTime)
{
}

HRESULT CBullet_EnergyBall::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();//앙기모띠

	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_GeoMesh_Cylinder_EnergyBall",
		CGeoMesh_Sphere::Create(m_pDevice, 0.8f))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_GeoMesh_Player_Lazer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBullet_EnergyBall::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_GeoMesh_Cylinder_EnergyBall",
		L"Com_CubeTexture",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_CubeTexture");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Bullet_EnergyBall",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform

	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = ((TRANSFORM_DESC*)pArg)->vPosition;//_float3(10.f, 3.f, 20.f);
	TransformDesc.vRotate = ((TRANSFORM_DESC*)pArg)->vRotate;
	TransformDesc.fSpeedPerSec = 40.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vScale = { 2.f, 2.f, 2.f };

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

	CEffectHandler::Add_Layer_Effect_BossBullet_EnergyBall_Trail(this,(CGameObject**)&m_pEffect);
	//m_vEffectOffset = { 0.f,0.f,0.f };


	return S_OK;
}

_uint CBullet_EnergyBall::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CBullet_EnergyBall::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_IsDead == true)
	{
		if (m_pEffect)
		{
			m_pEffect->Set_IsDead(true);
			m_pEffect = nullptr;
		}

		CEffectHandler::Add_Layer_Effect_BossBullet_EnergyBall_Dead(m_pTransform->Get_State(EState::Position), 1.f);

		return DEAD_OBJECT;
	}

	m_fLiveTime -= fDeltaTime;
	if (m_fLiveTime <= 0.f || m_IsCollide == true)
		m_IsDead = true;

	//BillBoard();

	return _uint();
}

_uint CBullet_EnergyBall::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pMesh->Render_Mesh();

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CBullet_EnergyBall::Movement(_float fDeltaTime)
{
	m_pTransform->Go_Straight(fDeltaTime);

	m_fLiveTime -= fDeltaTime;

	if (m_fLiveTime <= 0.f)
	{
		return DEAD_OBJECT;
	}

	if (m_IsTracking == false)
	{
		m_fTrackingTime -= fDeltaTime;

		if (m_fTrackingTime <= 0.f)
			m_IsTracking = true;

		return NO_EVENT;
	}
	
	_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
	_float3 vMyPos = m_pTransform->Get_State(EState::Position);

	_float3 vTargetDir = vTargetPos - vMyPos;
	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float3 vMyLook = m_pTransform->Get_State(EState::Look);
	_float3 vMyUp = m_pTransform->Get_State(EState::Up);
	D3DXVec3Normalize(&vMyLook, &vMyLook);
	D3DXVec3Normalize(&vMyUp, &vMyUp);

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
	{
		if(fCeta < fRadianMax)
			m_pTransform->RotateY(-fDeltaTime);
	}
	else
	{
		if (fCeta < fRadianMax)
			m_pTransform->RotateY(fDeltaTime);
	}

	_float3 vMyDown;
	D3DXVec3Cross(&vMyUp, &vMyLook, &vMyRight);
	D3DXVec3Cross(&vMyDown, &vMyRight, &vMyLook);

	_float fUp = D3DXVec3Dot(&vTargetDir, &vMyUp);
	_float fDown = D3DXVec3Dot(&vTargetDir, &vMyLeft);

	if (fUp < fDown)
		m_pTransform->RotateX(fDeltaTime);

	else
		m_pTransform->RotateX(-fDeltaTime);




	return _uint();
}

_uint CBullet_EnergyBall::Fire_Triger(_float fDeltaTime)
{
	return _uint();
}

_uint CBullet_EnergyBall::BillBoard()
{
	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	ZeroMemory(&matView._41, sizeof(_float3));
	D3DXMatrixInverse(&matView, 0, &matView);

	_float3 vBillPos = m_pTransform->Get_State(EState::Position);

	_float fScale[3];
	fScale[0] = m_pTransform->Get_State(EState::Right).x;
	fScale[1] = m_pTransform->Get_State(EState::Up).y;
	fScale[2] = m_pTransform->Get_State(EState::Look).z;

	memcpy(&matView._41, &vBillPos, sizeof(_float3));

	for (_uint i = 0; i < 3; ++i)
	{
		for (_uint j = 0; j < 4; ++j)
			matView(i, j) *= fScale[i];
	}

	m_pTransform->Set_WorldMatrix(matView);

	return _uint();
}

CBullet_EnergyBall * CBullet_EnergyBall::Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData /*= nullptr*/)
{
	CBullet_EnergyBall* pInstance = new CBullet_EnergyBall(pDevice, pData);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Boss_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBullet_EnergyBall::Clone(void * pArg/* = nullptr*/)
{
	CBullet_EnergyBall* pClone = new CBullet_EnergyBall(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Boss_Monster");
		Safe_Release(pClone);
	}

	return pClone;
}

void CBullet_EnergyBall::Free()
{
	Safe_Release(m_pTargetTransform);

	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	if (m_pEffect) 
	{
		m_pEffect->Set_IsDead(true);
		m_pEffect = nullptr;
	}


	CGameObject::Free();
}
