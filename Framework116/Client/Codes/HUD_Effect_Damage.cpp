#include "stdafx.h"
#include "HUD_Effect_Damage.h"

CHUD_Effect_Damage::CHUD_Effect_Damage(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CHUD_Effect_Damage::CHUD_Effect_Damage(const CHUD_Effect_Damage& other)
	: CUI(other)
{
}

HRESULT CHUD_Effect_Damage::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CHUD_Effect_Damage::Ready_GameObject(void* pArg)
{
	UI_DESC uiDesc;
	uiDesc.tTransformDesc.vScale = { WINCX * m_fScaleRate, WINCY * m_fScaleRate, 0.f };
	uiDesc.wstrTexturePrototypeTag = L"Component_Texture_Effect_Damage";
	CUI::Ready_GameObject(&uiDesc);

	return S_OK;
}

_uint CHUD_Effect_Damage::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);

	if (m_IsDead == true)
		return DEAD_OBJECT;

	if (m_fScaleRate >= 2.5f) {
		//m_fScaleRate = 1.f;
		return DEAD_OBJECT;
	}
	else m_fScaleRate += fDeltaTime;

	_float3 vScale = { WINCX * m_fScaleRate, WINCY * m_fScaleRate, 0.f };
	m_pTransform->Set_Scale(vScale);

	return NO_EVENT;
}

_uint CHUD_Effect_Damage::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);

	return _uint();
}

_uint CHUD_Effect_Damage::Render_GameObject()
{
	//m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	//m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);

	CUI::Render_GameObject();

	return _uint();
}

CHUD_Effect_Damage* CHUD_Effect_Damage::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CHUD_Effect_Damage* pInstance = new CHUD_Effect_Damage(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHUD_Effect_Damage::Clone(void* pArg)
{
	CHUD_Effect_Damage* pClone = new CHUD_Effect_Damage(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CHUD_Effect_Damage::Free()
{
	CUI::Free();

}
