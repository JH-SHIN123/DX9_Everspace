#include "stdafx.h"
#include "..\Headers\Player_Bullet.h"
#include "Player.h"
#include "Pipeline.h"
#include "MainCam.h"
#include "ExplosionSystem.h"

CPlayer_Bullet::CPlayer_Bullet(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData)
	: CGameObject(pDevice)
{
	m_pPassData = pData;
}

CPlayer_Bullet::CPlayer_Bullet(const CPlayer_Bullet & other)
	: CGameObject(other)
{
}

HRESULT CPlayer_Bullet::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPlayer_Bullet::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_CubeTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 800.f;
	TransformDesc.vScale = { 0.5f, 0.5f, 0.5f };

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
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_CollideSphere",
		L"Com_CollideSphere",
		(CComponent**)&m_pCollide,
		nullptr,
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

	// TEST중!!
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_float3 vPlayerRight = m_pPlayerTransform->Get_State(EState::Right);

	
	if ((_bool)pArg == true)
		m_vMuzzlePos = vPlayerPos - (vPlayerRight * 5.f);
	else
		m_vMuzzlePos = vPlayerPos + (vPlayerRight * 5.f);

	m_pTransform->Set_Position(m_vMuzzlePos);

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	/* 뷰포트 -> 투영스페이스 */
	_float3 vMouse = _float3(0.f, 0.f, 0.f);
	vMouse.x = ptMouse.x / (WINCX * 0.5f) - 1.f;
	vMouse.y = 1.f - ptMouse.y / (WINCY * 0.5f);

	/* 투영스페이스 -> 뷰스페이스 */
	_float4x4 matProj;
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMouse, &vMouse, &matProj);

	/* 뷰스페이스 상에서 광선의 출발점과 방향을 구해준다. */
	_float3 vRayPos = _float3(0.f, 0.f, 0.f);
	_float3 vRayDir = vMouse - vRayPos;

	/* 뷰스페이스 -> 월드스페이스 */
	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);
	D3DXVec3TransformCoord(&vRayPos, &vRayPos, &matView);
	D3DXVec3TransformNormal(&vRayDir, &vRayDir, &matView);
	D3DXVec3Normalize(&vRayDir, &vRayDir);


	m_vPlayerLook = vRayDir;
	D3DXVec3Normalize(&m_vPlayerLook, &m_vPlayerLook);

	// Material Setting
	m_tMaterial.Diffuse.r = m_tMaterial.Ambient.r = m_tMaterial.Specular.r = m_tMaterial.Emissive.r = 1.f;
	m_tMaterial.Diffuse.g = m_tMaterial.Ambient.g = m_tMaterial.Specular.g = m_tMaterial.Emissive.g = 1.f;
	m_tMaterial.Diffuse.b = m_tMaterial.Ambient.b = m_tMaterial.Specular.b = m_tMaterial.Emissive.b = 1.f;
	m_tMaterial.Diffuse.a = m_tMaterial.Ambient.a = m_tMaterial.Specular.a = m_tMaterial.Emissive.a = 1.f;


	return S_OK;
}

_uint CPlayer_Bullet::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsCollide) {
		m_IsDead = true;

		//if (m_pBulletParticle)
		//	m_pBulletParticle->Set_IsDead(true);

		return DEAD_OBJECT;
	}

	Movement(fDeltaTime);
	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);

	m_fLifeTime += fDeltaTime;

	if (m_fLifeTime >= 2.f) {
		m_IsDead = true;

		//if (m_pBulletParticle)
		//	m_pBulletParticle->Set_IsDead(true);

		return DEAD_OBJECT;
	}
	
	return NO_EVENT;
}

_uint CPlayer_Bullet::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CPlayer_Bullet::Render_GameObject()
{
	CGameObject::Render_GameObject();

	//m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pDevice->SetMaterial(&m_tMaterial);
	m_pVIBuffer->Render_VIBuffer();
	//m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CPlayer_Bullet::Movement(_float fDeltaTime)
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

CPlayer_Bullet * CPlayer_Bullet::Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData /*= nullptr*/)
{
	CPlayer_Bullet* pInstance = new CPlayer_Bullet(pDevice, pData);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Bullet::Clone(void * pArg/* = nullptr*/)
{
	CPlayer_Bullet* pClone = new CPlayer_Bullet(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player_Bullet");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlayer_Bullet::Free()
{
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);
	//
	//if (m_pBulletParticle)
	//	m_pBulletParticle->Set_IsDead(true);

	CGameObject::Free();
}
