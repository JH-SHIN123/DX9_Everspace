#include "stdafx.h"
#include "..\Headers\Bullet_Laser.h"
#include "MaterialHandler.h"
#include "EffectHandler.h"

CBullet_Laser::CBullet_Laser(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	_float4 vMaterial = { 1.f,1.f,1.f,1.f };
	CMaterialHandler::Set_RGBA(vMaterial, &m_tMaterial);
}

CBullet_Laser::CBullet_Laser(const CBullet_Laser & other)
	: CGameObject(other)
	, m_fLiveTime(other.m_fLiveTime)
	, m_IsTracking(other.m_IsTracking)
	, m_tMaterial(other.m_tMaterial)
{
}

HRESULT CBullet_Laser::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();


	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::NonStatic,
		L"Component_VIBuffer_RectTexture_BossLaser",
		CVIBuffer_RectTexture::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Texture_Bullet_EnergyBall");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBullet_Laser::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_VIBuffer_RectTexture_BossLaser",
		L"Com_RectTexture",
		(CComponent**)&m_pRectTexure)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Cylinder");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_BossLaser",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform

	TRANSFORM_DESC TransformDesc = *((TRANSFORM_DESC*)pArg);
	//TransformDesc.vPosition = ((TRANSFORM_DESC*)pArg)->vPosition;
	//TransformDesc.fRotatePerSec = D3DXToRadian(180.f);
	TransformDesc.fSpeedPerSec = 300.f;
	TransformDesc.vScale = { 3.f, 3.f, 3.f };

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

	CEffectHandler::Add_Layer_Effect_BossBullet_Laser_Trail(this, (CGameObject**)&m_pEffect);

	STAT_INFO tStatus;
	tStatus.iAtk = 40;
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

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;

	BillBoard();

	m_fLiveTime -= fDeltaTime;
	if (m_fLiveTime <= 0.f)
		m_IsDead = true;

	if (m_IsDead == true)
	{
		if (m_pEffect)
		{
			m_pEffect->Set_IsDead(true);
			m_pEffect = nullptr;
		}

		return DEAD_OBJECT;
	}

	return _uint();
}

_uint CBullet_Laser::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);

	m_pDevice->SetMaterial(&m_tMaterial);
	m_pTexture->Set_Texture(0);
	m_pRectTexure->Render_VIBuffer();

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CBullet_Laser::Movement(_float fDeltaTime)
{
	m_fLiveTime -= fDeltaTime;

	if (m_fLiveTime <= 0.f)
	{
		if (m_pEffect)
		{
			m_pEffect->Set_IsDead(true);
			m_pEffect = nullptr;
		}


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

	m_pTransform->Go_Dir(m_vMoveDir, fDeltaTime);



	return _uint();
}

_uint CBullet_Laser::BillBoard()
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

CBullet_Laser * CBullet_Laser::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CBullet_Laser* pInstance = new CBullet_Laser(pDevice);
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
	Safe_Release(m_pInfo);
	Safe_Release(m_pRectTexure);
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
