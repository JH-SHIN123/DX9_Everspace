#include "..\Headers\Component.h"

USING(Engine)

CComponent::CComponent(LPDIRECT3DDEVICE9 pDevice) /* 프로토타입 생성시 호출*/
	: m_pDevice(pDevice)
{
	Safe_AddRef(m_pDevice);
}

CComponent::CComponent(const CComponent & other) /* 클론 생성시 호출 */
	: m_pDevice(other.m_pDevice)
	, m_IsClone(true)
{
	Safe_AddRef(m_pDevice);
}

HRESULT CComponent::Ready_Component_Prototype()
{
	return S_OK;
}

HRESULT CComponent::Ready_Component(void * pArg)
{
	return S_OK;
}

void CComponent::Free()
{
	Safe_Release(m_pDevice);
}
