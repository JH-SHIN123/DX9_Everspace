#include "..\Headers\FadeOut.h"
#include "Management.h"
#include "Scene.h"

USING(Engine)

CFadeOut::CFadeOut(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CFadeOut::CFadeOut(const CFadeOut& other)
	: CUI(other)
{
}

_bool CFadeOut::Fade_Out()
{
	return m_bFadeOut;
}

HRESULT CFadeOut::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CFadeOut::Ready_GameObject(void* pArg)
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
		L"Component_Texture_FadeOut",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	_float2 vWinSize = CManagement::Get_Instance()->Get_WindowSize();
	m_tTransformDesc.vPosition = { 0.f, 0.f, 0.f };
	m_tTransformDesc.vScale = { vWinSize.x, vWinSize.y, 0.f };
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

	m_pScene = (CScene*)pArg;

	return S_OK;
}

_uint CFadeOut::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);

	if (m_fAlpha <= 0)
	{
		m_fAlpha = 0.f;
		m_bFadeOut = true;
		m_pScene->Set_bEnterScene();
		return DEAD_OBJECT;
	}
	else
		m_fAlpha -= m_fAlphaSpeed;

	return NO_EVENT;
}

_uint CFadeOut::LateUpdate_GameObject(_float fDeltaTime)
{
	if (m_bFadeOut) return DEAD_OBJECT;

	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::AlphaUI, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CFadeOut::Render_GameObject()
{
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	m_pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB((int)m_fAlpha, 255, 255, 255));

	CUI::Render_GameObject();

	return _uint();
}

CFadeOut* CFadeOut::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CFadeOut* pInstance = new CFadeOut(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFadeOut::Clone(void* pArg)
{
	CFadeOut* pClone = new CFadeOut(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CFadeOut::Free()
{
	CUI::Free();
}
