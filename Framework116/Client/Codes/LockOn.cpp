#include "stdafx.h"
#include "..\Headers\LockOn.h"
#include "Collision.h"
#include "Pipeline.h"

CLockOn::CLockOn(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CLockOn::CLockOn(const CLockOn & other)
	: CGameObject(other)
{
}

HRESULT CLockOn::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLockOn::Ready_GameObject(void * pArg/* = nullptr*/)
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
		EResourceType::NonStatic,
		L"Component_Texture_LockOn",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	/*m_pTargetCollide = (CCollideSphere*)pArg;*/
	/*m_ray = (RAY*)pArg;*/

	wstring* m_pTargetLayerTag = (wstring*)pArg;
	m_wstrLayerTag = *m_pTargetLayerTag;
	int i = 0;
	return S_OK;
}

_uint CLockOn::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);	
	


	return m_pTransform->Update_Transform();
}

_uint CLockOn::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);
	/*IsBillboarding();*/
	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::AlphaUI, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CLockOn::Render_GameObject()
{
	m_pTargetCollide = (CCollideSphere*)m_pManagement->Get_Component(m_wstrLayerTag, L"Com_CollideSphere");
	
	_float4x4 matView;
	//D3DXMatrixIdentity(&matView);
	//
	//matView._11 = 50.f;
	//matView._22 = 50.f;
	//matView._41 = 
	//matView._42 = 

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	//////////////////////////////////////////////////////



	return _uint();
}

_uint CLockOn::Movement(_float fDeltaTime)
{
	return _uint();
}

_uint CLockOn::IsBillboarding()
{
	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);

	_float4x4 matBill;
	D3DXMatrixIdentity(&matBill);

	

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);
	_float4x4 matWorld = m_pTransform->Get_TransformDesc().matWorld;
	m_pTransform->Set_WorldMatrix(matBill * matWorld);

	return _uint();
}

CLockOn * CLockOn::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLockOn* pInstance = new CLockOn(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create LockOn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLockOn::Clone(void * pArg/* = nullptr*/)
{
	CLockOn* pClone = new CLockOn(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone LockOn");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLockOn::Free()
{
	Safe_Release(m_pTargetCollide);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
