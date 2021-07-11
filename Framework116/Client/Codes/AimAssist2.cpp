#include "stdafx.h"
#include "..\Headers\AimAssist2.h"
#include "Pipeline.h"
#include "Collision.h"
#include "Player.h"

CAimAssist2::CAimAssist2(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CAimAssist2::CAimAssist2(const CAimAssist2 & other)
	: CGameObject(other)
{
}

HRESULT CAimAssist2::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CAimAssist2::Ready_GameObject(void * pArg/* = nullptr*/)
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
		L"Component_Texture_AimAssist2",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Texture (Dot)
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Crosshair_Dot",
		L"Com_Texture4",
		(CComponent**)&m_pTexture_Dot)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture4");
		return E_FAIL;
	}

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	return S_OK;
}

_uint CAimAssist2::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);	

	return m_pTransform->Update_Transform();
}

_uint CAimAssist2::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::AlphaUI, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CAimAssist2::Render_GameObject()
{
	CGameObject::Render_GameObject();	
	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	POINT	pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	ShowCursor(FALSE);

	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = 60.f;
	matView._22 = 60.f;
	matView._41 = (_float)pt.x - (WINCX >> 1);
	matView._42 = (_float)pt.y * -1.f + (WINCY >> 1) - 15.f;

	_int iWeapon = ((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Get_Weapon_Type();

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);

	if (iWeapon == WEAPON_LAZER)
	{
		m_pTexture->Set_Texture(0);
		m_pVIBuffer->Render_VIBuffer();
	}
	else if (iWeapon == WEAPON_MACHINEGUN)
	{
		m_pTexture_Dot->Set_Texture(0);
		m_pVIBuffer->Render_VIBuffer();
	}

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	return _uint();
}

_uint CAimAssist2::Movement(_float fDeltaTime)
{
	return _uint();
}

CAimAssist2 * CAimAssist2::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CAimAssist2* pInstance = new CAimAssist2(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAimAssist2::Clone(void * pArg/* = nullptr*/)
{
	CAimAssist2* pClone = new CAimAssist2(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Crosshair");
		Safe_Release(pClone);
	}

	return pClone;
}

void CAimAssist2::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTexture_Dot);

	CGameObject::Free();
}
