#include "pch.h"
#include "CustomMesh.h"

CCustomMesh::CCustomMesh(LPDIRECT3DDEVICE9 pDevice) :
	CComponent(pDevice)
{
}

CCustomMesh::CCustomMesh(const CCustomMesh& other) :
	CComponent(other)
{
}

void CCustomMesh::Add_GeometryMesh(const CGeometryMesh* pGeometryMesh)
{
	//if (nullptr == pGeometryMesh) return;

	//m_vecGeoMesh.emplace_back(pGeometryMesh);
	//Safe_AddRef(pGeometryMesh);
}

HRESULT CCustomMesh::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	return S_OK;
}

HRESULT CCustomMesh::Ready_Component(void* pArg)
{
	CComponent::Ready_Component(pArg);

	return S_OK;
}

_uint CCustomMesh::Render_Mesh()
{
	//m_pDevice->SetTexture(0, nullptr);

	//for (auto& p : m_vecGeoMesh) {
	//	// SetWorldMatrix
	//	p->Render_Mesh();
	//}

	return _uint();
}

CCustomMesh* CCustomMesh::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CCustomMesh* pInstance = new CCustomMesh(pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCustomMesh::Clone(void* pArg)
{
	CCustomMesh* pClone = new CCustomMesh(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CCustomMesh");
		Safe_Release(pClone);
	}

	return pClone;
}

void CCustomMesh::Free()
{
	//for (auto& p : m_vecGeoMesh) {
	//	Safe_Release(p);
	//}

	CComponent::Free();
}
