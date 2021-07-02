#include "GeoMesh_Cylinder.h"

USING(Engine)

CGeoMesh_Cylinder::CGeoMesh_Cylinder(LPDIRECT3DDEVICE9 pDevice, FLOAT Radius1, FLOAT Radius2, FLOAT Length) :
	CGeoMesh(pDevice),
	m_fRadius1(Radius1), m_fRadius2(Radius2), m_fLength(Length)
{
}

CGeoMesh_Cylinder::CGeoMesh_Cylinder(const CGeoMesh_Cylinder& other) :
	CGeoMesh(other)
{
}

HRESULT CGeoMesh_Cylinder::Ready_Component_Prototype()
{
	CGeoMesh::Ready_Component_Prototype();
	m_eGeoType = EGeoMeshType::Cylinder;

	// 1Unit = 1m
	if (FAILED(D3DXCreateCylinder(m_pDevice, m_fRadius1, m_fRadius2, m_fLength,10,10, &m_pMesh, nullptr))) {
		PRINT_LOG(L"Error", L"Failed to Create CGeoMesh_Cube");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGeoMesh_Cylinder::Ready_Component(void* pArg)
{
	CGeoMesh::Ready_Component(pArg);

	return S_OK;
}

_uint CGeoMesh_Cylinder::Render_Mesh()
{
	if (m_pMesh)
		m_pMesh->DrawSubset(0);

	return _uint();
}

CGeoMesh_Cylinder* CGeoMesh_Cylinder::Create(LPDIRECT3DDEVICE9 pDevice, FLOAT Radius1, FLOAT Radius2, FLOAT Length)
{
	CGeoMesh_Cylinder* pInstance = new CGeoMesh_Cylinder(pDevice, Radius1, Radius2, Length);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CGeoMesh_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CGeoMesh_Cylinder::Clone(void* pArg)
{
	CGeoMesh_Cylinder* pClone = new CGeoMesh_Cylinder(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CGeoMesh_Cube");
		Safe_Release(pClone);
	}

	return pClone;
}

void CGeoMesh_Cylinder::Free()
{
	CGeoMesh::Free();
}
