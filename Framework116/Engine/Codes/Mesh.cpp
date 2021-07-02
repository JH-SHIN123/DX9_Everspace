#include "Mesh.h"

USING(Engine)

CMesh::CMesh(LPDIRECT3DDEVICE9 pDevice) :
	CComponent(pDevice)
{
}

CMesh::CMesh(const CMesh& other) :
	CComponent(other) ,
	m_pMesh(other.m_pMesh)
{
	Safe_AddRef(m_pMesh);
}

HRESULT CMesh::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	return S_OK;
}

HRESULT CMesh::Ready_Component(void* pArg)
{
	CComponent::Ready_Component(pArg);

	return S_OK;
}

void CMesh::Free()
{
	CComponent::Free();
	Safe_Release(m_pMesh);
}
