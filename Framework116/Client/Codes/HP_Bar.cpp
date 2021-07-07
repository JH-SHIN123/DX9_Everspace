#include "stdafx.h"
#include "..\Headers\HP_Bar.h"
#include "Pipeline.h"
#include "Collision.h"

CHP_Bar::CHP_Bar(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CHP_Bar::CHP_Bar(const CHP_Bar & other)
	: CUI(other)
{
}

HRESULT CHP_Bar::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CHP_Bar::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CUI::Ready_GameObject(pArg);
	return S_OK;
}

_uint CHP_Bar::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);
	m_pTransform->Set_Position(_float3(100.f, 200.f, 0.f));
	
	return m_pTransform->Update_Transform();
}

_uint CHP_Bar::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);

	//if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
	//	return UPDATE_ERROR;

	return _uint();
}

_uint CHP_Bar::Render_GameObject()
{
	CUI::Render_GameObject();

	return _uint();
}

_uint CHP_Bar::Movement(_float fDeltaTime)
{
	return _uint();
}

_uint CHP_Bar::IsBillboarding()
{
	return _uint();
}

CHP_Bar * CHP_Bar::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CHP_Bar* pInstance = new CHP_Bar(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create HP_Bar");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject * CHP_Bar::Clone(void * pArg/* = nullptr*/)
{
	CHP_Bar* pClone = new CHP_Bar(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone HP_Bar");
		Safe_Release(pClone);
	}
   	return pClone;
}

void CHP_Bar::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}

