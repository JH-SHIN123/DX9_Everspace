#include "stdafx.h"
#include "..\Headers\Player_Lazer.h"
#include "Player.h"

CPlayer_Lazer::CPlayer_Lazer(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData)
	: CGameObject(pDevice)
{
	m_pPassData = pData;
}

CPlayer_Lazer::CPlayer_Lazer(const CPlayer_Lazer & other)
	: CGameObject(other)
{
}

HRESULT CPlayer_Lazer::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPlayer_Lazer::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_GeoMesh_Player_Lazer",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	//if (FAILED(CGameObject::Add_Component(
	//	EResourceType::Static,
	//	L"Component_Texture_Player_Lazer",
	//	L"Com_Texture",
	//	(CComponent**)&m_pTexture)))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
	//	return E_FAIL;
	//}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 0.f;
	TransformDesc.vScale = { 0.3f, 0.3f, 81.f };

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

	if ((_bool)pArg == true)
		m_IsLeft = true;
	else
		m_IsLeft = false;

	return S_OK;
}

_uint CPlayer_Lazer::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);
	Movement(fDeltaTime);
	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().vPosition);

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		return NO_EVENT;
	}
	else
		return DEAD_OBJECT;
	
	return NO_EVENT;
}

_uint CPlayer_Lazer::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CPlayer_Lazer::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	//m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_Mesh();
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CPlayer_Lazer::Movement(_float fDeltaTime)
{
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_float3 vPlayerRight = m_pPlayerTransform->Get_State(EState::Right);

	if (m_IsLeft)
		m_vMuzzlePos = vPlayerPos - (vPlayerRight * 5.f) + m_pPlayerTransform->Get_State(EState::Look) * 400.f - m_pPlayerTransform->Get_State(EState::Up) * 30.f;
	else
		m_vMuzzlePos = vPlayerPos + (vPlayerRight * 5.f) + m_pPlayerTransform->Get_State(EState::Look) * 400.f - m_pPlayerTransform->Get_State(EState::Up) * 30.f;

	m_pTransform->Set_Position(m_vMuzzlePos);

	m_vPlayerLook = m_pPlayerTransform->Get_State(EState::Look);
	D3DXVec3Normalize(&m_vPlayerLook, &m_vPlayerLook);

	_float4x4 matWorld;
	matWorld = m_pPlayerTransform->Get_TransformDesc().matWorld;


	matWorld._31 = m_vPlayerLook.x;
	matWorld._32 = m_vPlayerLook.y;
	matWorld._33 = m_vPlayerLook.z;


	_float3 vPlayerRotate = m_pPlayerTransform->Get_TransformDesc().vRotate;
	m_pTransform->Set_Rotate(vPlayerRotate);

	m_pTransform->Set_WorldMatrix(matWorld);

	return _uint();
}

CPlayer_Lazer * CPlayer_Lazer::Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData /*= nullptr*/)
{
	CPlayer_Lazer* pInstance = new CPlayer_Lazer(pDevice, pData);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player_Bullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Lazer::Clone(void * pArg/* = nullptr*/)
{
	CPlayer_Lazer* pClone = new CPlayer_Lazer(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player_Bullet");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlayer_Lazer::Free()
{
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	//Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
