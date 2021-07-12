#include "stdafx.h"
#include "..\Headers\LobbyCursor.h"

CLobbyCursor::CLobbyCursor(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CLobbyCursor::CLobbyCursor(const CLobbyCursor & other)
	: CGameObject(other)
{
}

HRESULT CLobbyCursor::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLobbyCursor::Ready_GameObject(void * pArg/* = nullptr*/)
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
		L"Component_Texture_Cursor",
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

	ShowCursor(FALSE);
	m_pTransform->Set_Scale(_float3(10.f, 10.f, 0));
	return S_OK;
}

_uint CLobbyCursor::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);	

	return m_pTransform->Update_Transform();
}

_uint CLobbyCursor::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
		return UPDATE_ERROR;


	return _uint();
}

_uint CLobbyCursor::Render_GameObject()
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
	////////////////////////////////////////
	/////////////////////////
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	return _uint();
}

_uint CLobbyCursor::Movement(_float fDeltaTime)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	_float3 vDecartPos = { (_float)pt.x,(_float)pt.y,0.f };
	_float3 vPos;
	vPos.x = vDecartPos.x - _float(WINCX / 2.f);
	vPos.y = _float(WINCY / 2.f) - vDecartPos.y;
	vPos.z = 1.f;
	m_pTransform->Set_Position(vPos);
	return _uint();
}


CLobbyCursor * CLobbyCursor::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLobbyCursor* pInstance = new CLobbyCursor(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLobbyCursor::Clone(void * pArg/* = nullptr*/)
{
	CLobbyCursor* pClone = new CLobbyCursor(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLobbyCursor::Free()
{
	
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
