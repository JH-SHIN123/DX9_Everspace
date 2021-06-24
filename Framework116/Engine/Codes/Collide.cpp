#include "Collide.h"

USING(Engine)

CCollide::CCollide(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CCollide::CCollide(const CCollide& other)
	: CComponent(other)
{
}

HRESULT CCollide::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	return S_OK;
}

HRESULT CCollide::Ready_Component(void* pArg)
{
	CComponent::Ready_Component(pArg);

	return S_OK;
}

void CCollide::Free()
{
	CComponent::Free();
}
