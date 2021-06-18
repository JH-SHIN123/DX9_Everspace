#include "stdafx.h"
#include "..\Headers\Grass.h"

CGrass::CGrass(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CGrass::CGrass(const CGrass & other)
	: CGameObject(other)
{
}

HRESULT CGrass::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CGrass::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Grass",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	m_pTerrainBuffer = (CVIBuffer_TerrainTexture*)m_pManagement->Get_Component(L"Layer_Terrain", L"Com_VIBuffer");
	Safe_AddRef(m_pTerrainBuffer);
	if (nullptr == m_pTerrainBuffer)
	{
		PRINT_LOG(L"Error", L"m_pTerrainBuffer is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

_uint CGrass::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);	

	return m_pTransform->Update_Transform();
}

_uint CGrass::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;

	IsBillboarding();

	return _uint();
}

_uint CGrass::Render_GameObject()
{
	CGameObject::Render_GameObject();	

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();	

	return _uint();
}

_uint CGrass::Movement(_float fDeltaTime)
{
	_float3 vOutPos = m_pTransform->Get_State(EState::Position);
	if (true == m_pTerrainBuffer->Is_OnTerrain(&vOutPos))
	{
		vOutPos.y += 0.5f;
		m_pTransform->Set_Position(vOutPos);
	}	

	return _uint();
}

_uint CGrass::IsBillboarding()
{
	/* Y축 빌보드 */
	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);

	_float4x4 matBill;
	D3DXMatrixIdentity(&matBill);

	matBill._11 = matView._11;
	matBill._13 = matView._13;
	matBill._31 = matView._31;
	matBill._33 = matView._33;

	D3DXMatrixInverse(&matBill, 0, &matBill);
	_float4x4 matWorld = m_pTransform->Get_TransformDesc().matWorld;

	/*
	matBill * matWorld
	matBill * 스 * 자 * 이 * 공 * 부
	*/
	m_pTransform->Set_WorldMatrix(matBill * matWorld);

	return _uint();
}

CGrass * CGrass::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGrass* pInstance = new CGrass(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGrass::Clone(void * pArg/* = nullptr*/)
{
	CGrass* pClone = new CGrass(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CGrass::Free()
{
	Safe_Release(m_pTerrainBuffer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
