#include "CollideSphere.h"

USING(Engine)

CCollideSphere::CCollideSphere(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CCollideSphere::CCollideSphere(const CCollideSphere& other)
	: CComponent(other),
	m_pSphere(other.m_pSphere),
	m_tBoundingSphere(other.m_tBoundingSphere),
	m_tMaterial(other.m_tMaterial)
{
	Safe_AddRef(m_pSphere);
}

const BOUNDING_SPHERE& CCollideSphere::Get_BoundingSphere() const
{
	return m_tBoundingSphere;
}

HRESULT CCollideSphere::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	// 바운딩 박스 확인용 메시 생성
	// Radius 미터 단위로 변경(*100)
	UINT iMeshDetail = 20;
	if (FAILED(D3DXCreateSphere(m_pDevice, m_tBoundingSphere.fRadius,
		iMeshDetail, iMeshDetail, &m_pSphere, NULL))) 
	{
		PRINT_LOG(L"Error", L"Failed to D3DXCreateSphere");
		return E_FAIL;
	};

	// 하얀색 바운딩스피어 출력
	ZeroMemory(&m_tMaterial, sizeof(m_tMaterial));
	m_tMaterial.Diffuse.r = m_tMaterial.Ambient.r = 1.f;
	m_tMaterial.Diffuse.g = m_tMaterial.Ambient.g = 1.f;
	m_tMaterial.Diffuse.b = m_tMaterial.Ambient.b = 1.f;
	m_tMaterial.Diffuse.a = m_tMaterial.Ambient.a = 1.f;

	return S_OK;
}

HRESULT CCollideSphere::Ready_Component(void* pArg)
{
	CComponent::Ready_Component(pArg);

	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (dynamic_cast<BOUNDING_SPHERE*>(ptr))
		{
			memcpy(&m_tBoundingSphere, ptr, sizeof(BOUNDING_SPHERE));
		}
	}

	// Scale
	m_tBoundingSphere.matWorld._11 = m_tBoundingSphere.fRadius;
	m_tBoundingSphere.matWorld._22 = m_tBoundingSphere.fRadius;
	m_tBoundingSphere.matWorld._33 = m_tBoundingSphere.fRadius;

	return S_OK;
}

_uint CCollideSphere::Update_Collide(const _float3& vPos)
{
	// Pos
	m_tBoundingSphere.matWorld._41 = vPos.x + m_tBoundingSphere.vCenter.x;
	m_tBoundingSphere.matWorld._42 = vPos.y + m_tBoundingSphere.vCenter.y;
	m_tBoundingSphere.matWorld._43 = vPos.z + m_tBoundingSphere.vCenter.z;

	return _uint();
}

_uint CCollideSphere::Render_Collide()
{
	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_tBoundingSphere.matWorld);
	m_pDevice->SetMaterial(&m_tMaterial);
	m_pDevice->SetTexture(0,nullptr);
	m_pSphere->DrawSubset(0);
	m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	return _uint();
}

CCollideSphere* CCollideSphere::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CCollideSphere* pInstance = new CCollideSphere(pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Texture");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CComponent* CCollideSphere::Clone(void* pArg)
{
	CCollideSphere* pClone = new CCollideSphere(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Texture");
		Safe_Release(pClone);
	}

	return pClone;
}

void CCollideSphere::Free()
{
	Safe_Release(m_pSphere);
	CComponent::Free();
}
