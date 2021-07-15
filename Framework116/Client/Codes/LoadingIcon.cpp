#include "stdafx.h"
#include "LoadingIcon.h"

CLoadingIcon::CLoadingIcon(LPDIRECT3DDEVICE9 pDevice) :
	CUI(pDevice)
{
}

CLoadingIcon::CLoadingIcon(const CLoadingIcon& other) :
	CUI(other)
{
}

HRESULT CLoadingIcon::Ready_GameObject(void* pArg)
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
		EResourceType::Static,
		L"Component_Texture_LoadingIcon",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	m_tTransformDesc.vPosition = { (WINCX * 0.5f) - 100.f, (WINCY *0.5f) - 100.f, 0.f };
	m_tTransformDesc.vScale = { 100.f, 100.f, 0.f };
	m_tTransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		(void*)&m_tTransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	return S_OK;
}

_uint CLoadingIcon::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsDead == true)
		return DEAD_OBJECT;

	m_pTransform->RotateZ(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CLoadingIcon::Render_GameObject()
{
	CGameObject::Render_GameObject();

	//TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	//_float4x4 matScale, matRotate, matTrans;
	//D3DXMatrixScaling(&matScale, transformDesc.vScale.x, transformDesc.vScale.y, 1.f);
	//D3DXMatrixRotationZ(&matRotate, transformDesc.vRotate.z);
	//D3DXMatrixTranslation(&matTrans, transformDesc.vPosition.x, transformDesc.vPosition.y, 0.f);

	//_float4x4 matView;
	//D3DXMatrixIdentity(&matView);
	//matView = matScale * matRotate * matTrans;

	m_pDevice->SetTransform(D3DTS_VIEW, &m_pTransform->Get_TransformDesc().matWorld);

	/////////////////////////////////////////////////////////////////
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	/////////////////////////////////////////////////////////////////

	return _uint();
}

CLoadingIcon* CLoadingIcon::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLoadingIcon* pInstance = new CLoadingIcon(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadingIcon::Clone(void* pArg)
{
	CLoadingIcon* pClone = new CLoadingIcon(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}
