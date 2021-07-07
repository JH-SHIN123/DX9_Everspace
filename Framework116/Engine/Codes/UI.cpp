#include "UI.h"
#include "Transform.h"
#include "Management.h"

USING(Engine)

CUI::CUI(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CUI::CUI(const CUI& other)
	: CGameObject(other)
{
}

HRESULT CUI::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CUI::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);

	UI_DESC* uiDescPtr = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (uiDescPtr = dynamic_cast<UI_DESC*>(ptr))
		{
			m_wstrTexturePrototypeTag = uiDescPtr->wstrTexturePrototypeTag;
		}
	}

	if (m_wstrTexturePrototypeTag == L"Component_Texture_HP_Bar" || m_wstrTexturePrototypeTag == L"Component_Texture_Stamina_Bar")
	{
		if (FAILED(CGameObject::Add_Component(
			EResourceType::Static,
			L"Component_VIBuffer_RectTexture_HP_Bar",
			L"Com_VIBuffer",
			(CComponent**)&m_pVIBuffer)))
		{
			PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
			return E_FAIL;
		}
	}
	else
	{
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
	}
	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
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
		(void*)&uiDescPtr->tTransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	return S_OK;
}

_uint CUI::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsDead == true)
		return DEAD_OBJECT;

	return NO_EVENT;
}

_uint CUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::AlphaUI, this)))
		return UPDATE_ERROR;

	//// Picking Check
	//POINT	pt = {};
	//GetCursorPos(&pt);
	//ScreenToClient(g_hWnd, &pt);

	// 데카르트 좌표계 -> 화면좌표계로 변경해야함
	//if (PtInRect(&m_tUIBounds, pt))
	//{

	//}

	return _uint();
}

_uint CUI::Render_GameObject()
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
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	/////////////////////////////////////////////////////////////////

	return _uint();
}

CUI* CUI::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CUI* pInstance = new CUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI::Clone(void* pArg)
{
	CUI* pClone = new CUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CUI::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
