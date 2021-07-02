#include "GeoMesh_Torus.h"

USING(Engine)

CGeoMesh_Torus::CGeoMesh_Torus(LPDIRECT3DDEVICE9 pDevice, FLOAT InnerRadius, FLOAT OuterRadius) :
	CGeoMesh(pDevice),
	m_fInnerRadius(InnerRadius),
	m_fOuterRadius(OuterRadius)
{
}

CGeoMesh_Torus::CGeoMesh_Torus(const CGeoMesh_Torus& other) :
	CGeoMesh(other)
{
}

HRESULT CGeoMesh_Torus::Ready_Component_Prototype()
{
	CGeoMesh::Ready_Component_Prototype();
	m_eGeoType = EGeoMeshType::Torus;

	// 1Unit = 1m
	if (FAILED(D3DXCreateTorus(m_pDevice, m_fInnerRadius, m_fOuterRadius, 10, 10, &m_pMesh, nullptr))) {
		PRINT_LOG(L"Error", L"Failed to Create CGeoMesh_Sphere");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGeoMesh_Torus::Ready_Component(void* pArg)
{
	CGeoMesh::Ready_Component(pArg);

	return S_OK;
}

_uint CGeoMesh_Torus::Render_Mesh()
{
	if (m_pMesh)
		m_pMesh->DrawSubset(0);

	return _uint();
}

CGeoMesh_Torus* CGeoMesh_Torus::Create(LPDIRECT3DDEVICE9 pDevice, FLOAT InnerRadius, FLOAT OuterRadius)
{
	CGeoMesh_Torus* pInstance = new CGeoMesh_Torus(pDevice, InnerRadius, OuterRadius);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CGeoMesh_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CGeoMesh_Torus::Clone(void* pArg)
{
	CGeoMesh_Torus* pClone = new CGeoMesh_Torus(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CGeoMesh_Sphere");
		Safe_Release(pClone);
	}

	return pClone;
}

void CGeoMesh_Torus::Free()
{
	CGeoMesh::Free();
}
