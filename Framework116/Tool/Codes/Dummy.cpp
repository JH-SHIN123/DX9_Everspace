#include "pch.h"
#include "..\Headers\Dummy.h"
#include <MainFrm.h>
#include "MainView.h"

CDummy::CDummy(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CDummy::CDummy(const CDummy & other)
	: CGameObject(other)
{
}

HRESULT CDummy::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CDummy::Ready_GameObject(void * pArg/* = nullptr*/)
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

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Monster",
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
		(CComponent**)&m_pTransform
		,pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	//m_pTransform->Set_WorldMatrix(TransformDesc->matWorld);

	return S_OK;
}

_uint CDummy::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	

	m_pTransform->Update_Transform();
	return NO_EVENT;
}

_uint CDummy::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CDummy::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();

#ifdef _DEBUG // Render Collide
#endif

	return _uint();
}

_uint CDummy::Movement(_float fDeltaTime)
{
	return _uint();
}

CDummy * CDummy::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDummy* pInstance = new CDummy(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDummy::Clone(void * pArg/* = nullptr*/)
{
	CDummy* pClone = new CDummy(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Dummy");
		Safe_Release(pClone);
	}

	return pClone;
}

void CDummy::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
