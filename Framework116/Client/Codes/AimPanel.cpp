#include "stdafx.h"
#include "AimPanel.h"

CAimPanel::CAimPanel(LPDIRECT3DDEVICE9 pDevice) :
	CUI(pDevice)
{
}

CAimPanel::CAimPanel(const CAimPanel& other) :
	CUI(other)
{
}

HRESULT CAimPanel::Ready_GameObject(void* pArg)
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
		L"Component_Texture_Head_Up_Display",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	m_tTransformDesc.vPosition = { 0.f, 0.f, 0.f };
	m_tTransformDesc.vScale = { 945.f, 763.f, 0.f };
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

_uint CAimPanel::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsDead == true)
		return DEAD_OBJECT;

	m_pTransform->RotateZ(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CAimPanel::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_VIEW, &m_pTransform->Get_TransformDesc().matWorld);

	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();

	return _uint();
}

CAimPanel* CAimPanel::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CAimPanel* pInstance = new CAimPanel(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAimPanel::Clone(void* pArg)
{
	CAimPanel* pClone = new CAimPanel(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}
