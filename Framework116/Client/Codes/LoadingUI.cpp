#include "stdafx.h"
#include "LoadingUI.h"

CLoadingUI::CLoadingUI(LPDIRECT3DDEVICE9 pDevice) :
	CUI(pDevice)
{
}

CLoadingUI::CLoadingUI(const CLoadingUI& other) :
	CUI(other)
{
}

HRESULT CLoadingUI::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);

	UI_DESC* uiDescPtr = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (uiDescPtr = dynamic_cast<UI_DESC*>(ptr))
		{
			m_wstrTexturePrototypeTag = uiDescPtr->wstrTexturePrototypeTag;
			m_tTransformDesc = uiDescPtr->tTransformDesc;
		}
	}

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
		m_wstrTexturePrototypeTag,
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
		(void*)&m_tTransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	mt19937 engine((unsigned int)time(NULL));
	_float iTexSize = (_float)m_pTexture->Get_TextureSize();

	// 생성 범위
	uniform_real_distribution<_float> distribution(0.f, iTexSize);
	auto generator = bind(distribution, engine);

	m_iIndexLoadingImage = (_uint)generator();
	if (m_iIndexLoadingImage >= iTexSize)
		m_iIndexLoadingImage = 0;

	return S_OK;
}

_uint CLoadingUI::Render_GameObject()
{
	CGameObject::Render_GameObject();

	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = transformDesc.vScale.x;
	matView._22 = transformDesc.vScale.y;
	matView._41 = transformDesc.vPosition.x;
	matView._42 = transformDesc.vPosition.y;
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);

	/////////////////////////////////////////////////////////////////
	m_pTexture->Set_Texture(m_iIndexLoadingImage);
	m_pVIBuffer->Render_VIBuffer();
	/////////////////////////////////////////////////////////////////

	return _uint();
}

CLoadingUI* CLoadingUI::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLoadingUI* pInstance = new CLoadingUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadingUI::Clone(void* pArg)
{
	CLoadingUI* pClone = new CLoadingUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

