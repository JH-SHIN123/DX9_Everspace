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
	m_tTransformDesc.fRotatePerSec = D3DXToRadian(9.f);
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

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	// 회전 영역 설정
	RECT rc;
	POINT p1, p2;
	GetClientRect(g_hWnd, &rc);

	p1.x = rc.left + 800;
	p1.y = rc.top + 100;
	p2.x = rc.right - 800;
	p2.y = rc.bottom - 100;

	ClientToScreen(g_hWnd, &p1);
	ClientToScreen(g_hWnd, &p2);

	rc.left = p1.x;
	rc.top = p1.y;
	rc.right = p2.x;
	rc.bottom = p2.y;

	if (PtInRect(&rc, pt))
	{
		if (abs(m_fRotateAngleZ) > 0.1f)
		{
			// 원점으로 돌아오기
			_float fRotateSpeed = fDeltaTime * 4.5f;

			if (m_fRotateAngleZ > -0.f)
				fRotateSpeed *= -1.f;


			m_fRotateAngleZ += fRotateSpeed;

			m_pTransform->RotateZ(fRotateSpeed);
		}
	}
	else {
		if (abs(m_fRotateAngleZ) < 5.f)
		{
			Rotate_AimPanel(fDeltaTime);
		}
	}

	return m_pTransform->Update_Transform_Quaternion();
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

void CAimPanel::Rotate_AimPanel(const _float fDeltaTime)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_float3 vMouse = { (_float)pt.x, (_float)pt.y, 0.f };
	_float3 vScreenCenter = { WINCX / 2.f, WINCY / 2.f, 0.f };
	_float3 vGap = vMouse - vScreenCenter;

	// 0.05f 플레이어가 얼만큼 더 회전할건지
	_float fSpeed = D3DXVec3Length(&vGap);
	D3DXVec3Normalize(&vGap, &vGap);

	_float fRotateSpeed = -D3DXToRadian((vGap.x) * 0.8f) * fDeltaTime * fSpeed;
	m_fRotateAngleZ += fRotateSpeed;

	m_pTransform->RotateZ(fRotateSpeed);
}
