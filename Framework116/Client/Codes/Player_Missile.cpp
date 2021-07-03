#include "stdafx.h"
#include "..\Headers\Player_Missile.h"
#include "Player.h"
#include "Pipeline.h"
#include "MainCam.h"

CPlayer_Missile::CPlayer_Missile(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData)
	: CGameObject(pDevice)
{
	m_pPassData = pData;
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
	TransformDesc.fSpeedPerSec = 200.f;
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


	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);

	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
		return E_FAIL;
	}

	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_float3 vPlayerRight = m_pPlayerTransform->Get_State(EState::Right);

	m_vMuzzlePos = vPlayerPos;
	m_pTransform->Set_Position(m_vMuzzlePos);

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




	return S_OK;
}

_uint CPlayer_Missile::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);
	Movement(fDeltaTime);
	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().vPosition);

	m_fLifeTime += fDeltaTime;

	if (m_fLifeTime >= 3.f)
		return DEAD_OBJECT;
	
	return NO_EVENT;
}

_uint CPlayer_Missile::LateUpdate_GameObject(_float fDeltaTime)
{
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
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CPlayer_Missile::Movement(_float fDeltaTime)
{
	_float4x4 matWorld;
	matWorld = m_pPlayerTransform->Get_TransformDesc().matWorld;

	matWorld._31 = m_vPlayerLook.x;
	matWorld._32 = m_vPlayerLook.y;
	matWorld._33 = m_vPlayerLook.z;

	if (m_IsFirst)
	{
		_float3 vPlayerRotate = m_pPlayerTransform->Get_TransformDesc().vRotate;
		m_pTransform->Set_Rotate(vPlayerRotate);
		m_IsFirst = false;
	}
	m_pTransform->Set_WorldMatrix(matWorld);

	m_pTransform->Go_Straight(fDeltaTime);
	




	return _uint();
}

_uint CPlayer_Missile::Searching_Target(_float fDeltaTime)
{
	return _uint();
}

CPlayer_Missile * CPlayer_Missile::Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData /*= nullptr*/)
{
	CPlayer_Missile* pInstance = new CPlayer_Missile(pDevice, pData);
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
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
