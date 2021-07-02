#include "GeoMesh_Sphere.h"

USING(Engine)

CGeoMesh_Sphere::CGeoMesh_Sphere(LPDIRECT3DDEVICE9 pDevice, FLOAT Radius) :
	CGeoMesh(pDevice),
	m_fRadius(Radius)
{
}

CGeoMesh_Sphere::CGeoMesh_Sphere(const CGeoMesh_Sphere& other) :
	CGeoMesh(other)
{
}

HRESULT CGeoMesh_Sphere::Ready_Component_Prototype()
{
	CGeoMesh::Ready_Component_Prototype();
	m_eGeoType = EGeoMeshType::Sphere;

	// 1Unit = 1m
	if (FAILED(D3DXCreateSphere(m_pDevice, m_fRadius, 20, 20, &m_pMesh, nullptr))) {
		PRINT_LOG(L"Error", L"Failed to Create CGeoMesh_Sphere");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGeoMesh_Sphere::Ready_Component(void* pArg)
{
	CGeoMesh::Ready_Component(pArg);

	return S_OK;
}

_uint CGeoMesh_Sphere::Render_Mesh()
{
	if (m_pMesh)
		m_pMesh->DrawSubset(0);

	return _uint();
}

CGeoMesh_Sphere* CGeoMesh_Sphere::Create(LPDIRECT3DDEVICE9 pDevice, FLOAT Radius)
{
	CGeoMesh_Sphere* pInstance = new CGeoMesh_Sphere(pDevice, Radius);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CGeoMesh_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CGeoMesh_Sphere::Clone(void* pArg)
{
	CGeoMesh_Sphere* pClone = new CGeoMesh_Sphere(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CGeoMesh_Sphere");
		Safe_Release(pClone);
	}

	return pClone;
}

void CGeoMesh_Sphere::Free()
{
	CGeoMesh::Free();
}
