#include "pch.h"
#include "..\Headers\Axis.h"

CAxis::CAxis(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CAxis::CAxis(const CAxis & other)
	: CGameObject(other)
{
}

HRESULT CAxis::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CAxis::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_Axis",
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Component_Mesh_Axis");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vScale = { 0.5f,0.5f,0.5f };

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

	return S_OK;
}

_uint CAxis::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	

	return m_pTransform->Update_Transform();
}

_uint CAxis::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CAxis::Render_GameObject()
{
	CGameObject::Render_GameObject();
	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	return _uint();
}

_uint CAxis::Movement(_float fDeltaTime)
{
	return _uint();
}

CAxis * CAxis::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CAxis* pInstance = new CAxis(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAxis::Clone(void * pArg/* = nullptr*/)
{
	CAxis* pClone = new CAxis(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CAxis::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);

	CGameObject::Free();
}
