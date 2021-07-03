#include "GeoMesh_Cube.h"

USING(Engine)

CGeoMesh_Cube::CGeoMesh_Cube(LPDIRECT3DDEVICE9 pDevice, FLOAT Width, FLOAT Height, FLOAT Depth) :
	CGeoMesh(pDevice),
	m_fWidth(Width), m_fHeight(Height), m_fDepth(Depth)
{
}

CGeoMesh_Cube::CGeoMesh_Cube(const CGeoMesh_Cube& other) :
	CGeoMesh(other)
{
}

HRESULT CGeoMesh_Cube::Ready_Component_Prototype()
{
	CGeoMesh::Ready_Component_Prototype();
	m_eGeoType = EGeoMeshType::Cube;

	// 1Unit = 1m
	if (FAILED(D3DXCreateBox(m_pDevice, m_fWidth, m_fHeight, m_fDepth, &m_pMesh, nullptr))) {
		PRINT_LOG(L"Error", L"Failed to Create CGeoMesh_Cube");
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CGeoMesh_Cube::Ready_Component(void* pArg)
{
	CGeoMesh::Ready_Component(pArg);

	return S_OK;
}

_uint CGeoMesh_Cube::Render_Mesh()
{
	if(m_pMesh)
		m_pMesh->DrawSubset(0);
	
	return _uint();
}

CGeoMesh_Cube* CGeoMesh_Cube::Create(LPDIRECT3DDEVICE9 pDevice, FLOAT Width, FLOAT Height, FLOAT Depth)
{
	CGeoMesh_Cube* pInstance = new CGeoMesh_Cube(pDevice, Width, Height, Depth);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CGeoMesh_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CGeoMesh_Cube::Clone(void* pArg)
{
	CGeoMesh_Cube* pClone = new CGeoMesh_Cube(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CGeoMesh_Cube");
		Safe_Release(pClone);
	}

	return pClone;
}

void CGeoMesh_Cube::Free()
{
	CGeoMesh::Free();
}
