#include "stdafx.h"
#include "..\Headers\AimAssist.h"
#include "Pipeline.h"
#include "Collision.h"
#include "Player.h"

CAimAssist::CAimAssist(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CAimAssist::CAimAssist(const CAimAssist & other)
	: CGameObject(other)
{
}

HRESULT CAimAssist::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CAimAssist::Ready_GameObject(void * pArg/* = nullptr*/)
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
		L"Component_Texture_AimAssist",
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
		(CComponent**)&m_pTransform)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	return S_OK;
}

_uint CAimAssist::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);	

	return m_pTransform->Update_Transform();
}

_uint CAimAssist::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::AlphaUI, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CAimAssist::Render_GameObject()
{
	CGameObject::Render_GameObject();	
	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	POINT	pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	ShowCursor(FALSE);

	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = 70.f;
	matView._22 = 70.f;
	matView._41 = (_float)pt.x - (WINCX >> 1);
	matView._42 = (_float)pt.y * -1.f + (WINCY >> 1) - 15.f;

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
	m_pTexture->Set_Texture(0);
	_int iWeapon = ((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Get_Weapon_Type();
	if(iWeapon == WEAPON_MACHINEGUN && iWeapon != WEAPON_MISSILE)
		m_pVIBuffer->Render_VIBuffer();	
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return _uint();
}

_uint CAimAssist::Movement(_float fDeltaTime)
{
	return _uint();
}

CAimAssist * CAimAssist::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CAimAssist* pInstance = new CAimAssist(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAimAssist::Clone(void * pArg/* = nullptr*/)
{
	CAimAssist* pClone = new CAimAssist(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Crosshair");
		Safe_Release(pClone);
	}

	return pClone;
}

void CAimAssist::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
