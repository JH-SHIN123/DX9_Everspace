#include "stdafx.h"
#include "HUD_Effect_Boost.h"

CHUD_Effect_Boost::CHUD_Effect_Boost(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CHUD_Effect_Boost::CHUD_Effect_Boost(const CHUD_Effect_Boost& other)
	: CUI(other)
{
}

HRESULT CHUD_Effect_Boost::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CHUD_Effect_Boost::Ready_GameObject(void* pArg)
{
	UI_DESC uiDesc;
	uiDesc.tTransformDesc.vScale = { WINCX * m_fScaleRate, WINCY * m_fScaleRate, 0.f };
	uiDesc.wstrTexturePrototypeTag = L"Component_Texture_Effect_Boost";
	CUI::Ready_GameObject(&uiDesc);

	return S_OK;
}

_uint CHUD_Effect_Boost::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);

	if (m_IsDead == true)
		return DEAD_OBJECT;

	if (!m_bOperate) { // Init Param
		m_bSwitch = false;
		m_fScaleDeltaT = 0.f;
		m_fScaleRate = 1.4f;
		return NO_EVENT;
	}

	if (m_fScaleDeltaT >= 0.1f) {
		m_bSwitch = !m_bSwitch;
		m_fScaleDeltaT = 0.f;
	}
	else m_fScaleDeltaT += fDeltaTime;


	if (m_bSwitch)
	{
		m_fScaleRate += fDeltaTime;
	}
	else
	{
		m_fScaleRate -= fDeltaTime;
	}

	_float3 vScale = { WINCX * m_fScaleRate, WINCY * m_fScaleRate, 0.f };
	m_pTransform->Set_Scale(vScale);

	return NO_EVENT;
}

_uint CHUD_Effect_Boost::LateUpdate_GameObject(_float fDeltaTime)
{
	if (!m_bOperate)
		return NO_EVENT;

	CUI::LateUpdate_GameObject(fDeltaTime);

	return _uint();
}

_uint CHUD_Effect_Boost::Render_GameObject()
{
	if (!m_bOperate)
		return NO_EVENT;

	CUI::Render_GameObject();

	return _uint();
}

CHUD_Effect_Boost* CHUD_Effect_Boost::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CHUD_Effect_Boost* pInstance = new CHUD_Effect_Boost(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHUD_Effect_Boost::Clone(void* pArg)
{
	CHUD_Effect_Boost* pClone = new CHUD_Effect_Boost(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CHUD_Effect_Boost::Free()
{
	CUI::Free();

}

