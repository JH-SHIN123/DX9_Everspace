#include "Collide.h"

USING(Engine)

CCollide::CCollide(LPDIRECT3DDEVICE9 pDevice, const LPD3DXMESH pMesh)
	: CComponent(pDevice)
{
}

CCollide::CCollide(LPDIRECT3DDEVICE9 pDevice, const CVIBuffer* pMesh)
	: CComponent(pDevice)
{
}

CCollide::CCollide(const CCollide& other)
	: CComponent(other),
	m_pSphere(other.m_pSphere),
	m_tBoundingSphere(other.m_tBoundingSphere)
{
	Safe_AddRef(m_pSphere);
}

HRESULT CCollide::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	D3DXComputeBoundingSphere();


	// 바운딩 박스 확인용 메시 생성
	D3DXCreateSphere(m_pDevice, m_tBoundingSphere._radius,
		10, 10, &m_pSphere, NULL);


	return S_OK;
}

HRESULT CCollide::Ready_Component(void* pArg)
{
	CComponent::Ready_Component(pArg);

	return S_OK;
}

_uint CCollide::Render_Collide()
{
	return _uint();
}

CCollide* CCollide::Create(LPDIRECT3DDEVICE9 pDevice, const LPD3DXMESH pMesh)
{
	CCollide* pInstance = new CCollide(pDevice, pMesh);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCollide* CCollide::Create(LPDIRECT3DDEVICE9 pDevice, const CVIBuffer* pMesh)
{
	CCollide* pInstance = new CCollide(pDevice, pMesh);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Texture");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

//CCollide* CCollide::Create(LPDIRECT3DDEVICE9 pDevice, const TCHAR* pFilePath, const TCHAR* pTexturePath)
//{
//	CCollide* pInstance = new CCollide(pDevice, pFilePath, pTexturePath);
//	if (FAILED(pInstance->Ready_Component_Prototype()))
//	{
//		PRINT_LOG(L"Error", L"Failed To Create Texture");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

CComponent* CCollide::Clone(void* pArg)
{
	CCollide* pClone = new CCollide(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Texture");
		Safe_Release(pClone);
	}

	return pClone;
}

void CCollide::Free()
{
	Safe_Release(m_pSphere);
	CComponent::Free();
}
