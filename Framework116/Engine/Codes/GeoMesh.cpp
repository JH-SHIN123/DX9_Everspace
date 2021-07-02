#include "GeoMesh.h"

USING(Engine)

CGeoMesh::CGeoMesh(LPDIRECT3DDEVICE9 pDevice) :
	CMesh(pDevice)
{
}

CGeoMesh::CGeoMesh(const CGeoMesh& other) :
	CMesh(other),
	m_eGeoType(other.m_eGeoType)
{
}

HRESULT CGeoMesh::Ready_Component_Prototype()
{
	CMesh::Ready_Component_Prototype();

	return S_OK;
}

HRESULT CGeoMesh::Ready_Component(void* pArg)
{
	CMesh::Ready_Component(pArg);

	return S_OK;
}

void CGeoMesh::Free()
{
	CMesh::Free();
}
