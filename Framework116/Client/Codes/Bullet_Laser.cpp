#include "stdafx.h"
#include "..\Headers\Bullet_Laser.h"

CBullet_Laser::CBullet_Laser(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData)
	: CGameObject(pDevice)
{
	m_pPassData = pData;
}

CBullet_Laser::CBullet_Laser(const CBullet_Laser & other)
	: CGameObject(other)
	, m_fLiveTime(other.m_fLiveTime)
	, m_IsTracking(other.m_IsTracking)
{
}

HRESULT CBullet_Laser::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CBullet_Laser::Ready_GameObject(void * pArg/* = nullptr*/)
{
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
	TransformDesc.vPosition = ((TRANSFORM_DESC*)pArg)->vPosition;
	TransformDesc.fSpeedPerSec = 220.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(180.f);
	TransformDesc.vScale = { 1.f, 1.f, 1.f };

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
	// asdasd

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

	//m_pParentTransform = (CTransform*)m_pManagement->Get_Component(L"GameObject_Boss_Monster", L"Com_Transform");
	//Safe_AddRef(m_pParentTransform);
	//if (nullptr == m_pParentTransform)
	//{
	//	PRINT_LOG(L"Error", L"m_pParentTransform is nullptr");
	//	return E_FAIL;
	//}


	return S_OK;
}

_uint CBullet_Laser::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);

	return NO_EVENT;
}

_uint CBullet_Laser::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CBullet_Laser::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pCube->Render_VIBuffer();

#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CBullet_Laser::Movement(_float fDeltaTime)
{
	m_fLiveTime -= fDeltaTime;

	if (m_fLiveTime <= 0.f)
	{
		return DEAD_OBJECT;
	}

	if (m_IsTracking == false)
	{
		m_pTransform->Update_Transform();

		_float3 vTargetPos = m_pTargetTransform->Get_State(EState::Position);
		_float3 vMyPos = m_pTransform->Get_State(EState::Position);
		m_vMoveDir = vTargetPos - vMyPos;
		D3DXVec3Normalize(&m_vMoveDir, &m_vMoveDir);
		//m_pTransform->Set_Rotate(m_vMoveDir);
		m_IsTracking = true;
	}

	m_pTransform->Go_Dir(m_vMoveDir,fDeltaTime);



	return _uint();
}

_uint CBullet_Laser::Fire_Triger(_float fDeltaTime)
{
	return _uint();
}

CBullet_Laser * CBullet_Laser::Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData /*= nullptr*/)
{
	CBullet_Laser* pInstance = new CBullet_Laser(pDevice, pData);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Boss_Monster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBullet_Laser::Clone(void * pArg/* = nullptr*/)
{
	CBullet_Laser* pClone = new CBullet_Laser(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Boss_Monster");
		Safe_Release(pClone);
	}

	return pClone;
}

void CBullet_Laser::Free()
{
	Safe_Release(m_pTargetTransform);

	Safe_Release(m_pCube);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
