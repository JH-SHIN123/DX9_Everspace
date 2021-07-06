#include "stdafx.h"
#include "..\Headers\AlertArrow.h"
#include "Collision.h"
#include "Pipeline.h"

CAlertArrow::CAlertArrow(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CAlertArrow::CAlertArrow(const CAlertArrow & other)
	: CGameObject(other)
{
}

HRESULT CAlertArrow::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CAlertArrow::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Texture_AlertArrow",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC Transform;
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	m_pTargetMonster = (CGameObject*)pArg;
	Safe_AddRef(m_pTargetMonster);
	if (nullptr == m_pTargetMonster)
	{
		PRINT_LOG(L"Error", L"m_pTargetMonster is nullptr");
	}

	return S_OK;
}

_uint CAlertArrow::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	
	Movement(fDeltaTime);	
	return m_pTransform->Update_Transform();
}

_uint CAlertArrow::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);
	
	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CAlertArrow::Render_GameObject()
{
	CGameObject::Render_GameObject();

	return _uint();
}

_uint CAlertArrow::Movement(_float fDeltaTime)
{
	// 타겟 몬스터의 포스 받아옴.
	// 이제 화살표가 타겟 몬스터의 포스쪽을 가리켜야 하는디..
	// 월드상에서 화살표의 Look벡터가 몬스터 포스를 가르킨다 하믄..
	// 직교투영 했을때.. 
	_float3 vTargetPos = m_pTargetMonster->Get_Collides()->front()->Get_BoundingSphere().Get_Position();
	if (nullptr == vTargetPos)
	{
		PRINT_LOG(L"Error", L"vTargetPos is nullptr");
		return _uint();
	}
	
	_float4x4 matWorld = m_pTransform->Get_TransformDesc().matWorld;

	matWorld._11 = 10.f;
	matWorld._22 = 10.f;
	matWorld._33 = 10.f;

	matWorld._41 = 30.f;
	matWorld._42 = 30.f;
	matWorld._43 = 0.f;

	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);

	_float4x4 matBill;
	D3DXMatrixIdentity(&matBill);

	matBill = matView;
	matBill._41 = 0.f;
	matBill._42 = 0.f;
	matBill._43 = 0.f;

	D3DXMatrixInverse(&matBill, 0, &matBill);
	_float4x4 realmatWorld;
	realmatWorld = matBill * matWorld;
	m_pTransform->Set_WorldMatrix(realmatWorld);

	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	//////////////////////////////////////////////////////


	return _uint();
}

CAlertArrow * CAlertArrow::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CAlertArrow* pInstance = new CAlertArrow(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create AlertArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAlertArrow::Clone(void * pArg/* = nullptr*/)
{
	CAlertArrow* pClone = new CAlertArrow(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone AlertArrow");
		Safe_Release(pClone);
	}

	return pClone;
}

void CAlertArrow::Free()
{
	Safe_Release(m_pTargetMonster);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
