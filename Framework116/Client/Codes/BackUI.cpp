#include "stdafx.h"
#include "..\Headers\BackUI.h"
#include"Pipeline.h"
#include "MaterialHandler.h"

USING(Engine)

CBackUI::CBackUI(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	ZeroMemory(&m_tMaterial, sizeof(D3DMATERIAL9));
	_float4 color = { 1.f, 1.f, 1.f, 1.f };

	CMaterialHandler::Set_RGBA(color, &m_tMaterial);
}

CBackUI::CBackUI(const CBackUI& other)
	: CGameObject(other)
	, m_ePortraitNumber(other.m_ePortraitNumber)
	, m_tMaterial(other.m_tMaterial)
{

}

HRESULT CBackUI::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CBackUI::Ready_GameObject(void* pArg)
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
		(void*)&m_tTransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	return S_OK;
}

_uint CBackUI::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsDead == true)
		return DEAD_OBJECT;

	return m_pTransform->Update_Transform();
}

_uint CBackUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::AlphaUI, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CBackUI::Render_GameObject()
{
	CGameObject::Render_GameObject();

	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	m_pDevice->SetMaterial(&m_tMaterial);

	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = transformDesc.vScale.x;
	matView._22 = transformDesc.vScale.y;
	matView._41 = transformDesc.vPosition.x;
	matView._42 = transformDesc.vPosition.y;
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
	
	if(m_wstrTexturePrototypeTag == L"Component_Texture_Portrait")
	{
		if(m_ePortraitNumber != EPortraitNumber::End)
			m_pTexture->Set_Texture((_int)m_ePortraitNumber);
	}

	else
		m_pTexture->Set_Texture(0);


	m_pVIBuffer->Render_VIBuffer();


	return _uint();
}

const TRANSFORM_DESC CBackUI::Get_UI_TransformDesc()
{
	return m_tTransformDesc;
}

HRESULT CBackUI::Change_Texture(const wstring & wstrTexturePrototypeTag)
{
	//Safe_Release(m_pTexture);
	//m_wstrTexturePrototypeTag = wstrTexturePrototypeTag;

	//if (FAILED(CGameObject::Add_Component(
	//	EResourceType::NonStatic,
	//	m_wstrTexturePrototypeTag,
	//	L"Com_Texture",
	//	(CComponent**)&m_pTexture)))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
	//	return E_FAIL;
	//}
	//Safe_AddRef(m_pTexture);
	return S_OK;
}

HRESULT CBackUI::Change_TextureNumber(const EPortraitNumber & ePortraitNumber)
{
	if (ePortraitNumber == EPortraitNumber::End)
		return E_FAIL;

	m_ePortraitNumber = ePortraitNumber;

	return S_OK;
}


CBackUI* CBackUI::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CBackUI* pInstance = new CBackUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackUI::Clone(void* pArg)
{
	CBackUI* pClone = new CBackUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CBackUI::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
