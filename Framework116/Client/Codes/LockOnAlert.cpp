#include "stdafx.h"
#include "..\Headers\LockOnAlert.h"
#include "Pipeline.h"
#include "Collision.h"

CLockOnAlert::CLockOnAlert(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CLockOnAlert::CLockOnAlert(const CLockOnAlert & other)
	: CUI(other)
{
}

HRESULT CLockOnAlert::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLockOnAlert::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CUI::Ready_GameObject(pArg);
	return S_OK;
}

_uint CLockOnAlert::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);
	
	Adjust_Pos(fDeltaTime);

	if (m_fScaleX > 80.f)
	{
		m_bAddScale = false;
	}
	if (m_fScaleX < 0.f)
	{
		m_bAddScale = true;
	}

	if (m_bAddScale)
	{
		m_fScaleX += 2.0f;
		m_fScaleY += 2.0f;
	}
	else
	{
		m_fScaleX -= 2.f;
		m_fScaleY -= 2.f;
	}


	_float3 vScale = { m_fScaleX, m_fScaleY, 0.f };
	m_pTransform->Set_Scale(vScale);

	return NO_EVENT;
}

_uint CLockOnAlert::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);

	if (m_IsDead)
		return DEAD_OBJECT;

	return _uint();
}

_uint CLockOnAlert::Render_GameObject()
{
	CUI::Render_GameObject();

	return _uint();
}

void CLockOnAlert::Set_ScaleX(_float _fDamage)
{

}

_uint CLockOnAlert::Movement(_float fDeltaTime)
{
	return _uint();
}

_uint CLockOnAlert::Adjust_Pos(_float fDeltaTime)
{
		if (m_pTransform) {
			m_pTransform->Set_Position(_float3(-(WINCX / 2.f) + 1500.f, WINCY / 2.f - 120.f , 0.f));
			m_pTransform->Update_Transform();
		}

	return _uint();
}

_uint CLockOnAlert::Who_Make_Me(MAKERID _iMakerName)
{
	m_eMakerID = _iMakerName;
	return _uint();
}

CLockOnAlert * CLockOnAlert::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLockOnAlert* pInstance = new CLockOnAlert(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Stamina_Bar");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject * CLockOnAlert::Clone(void * pArg/* = nullptr*/)
{
	CLockOnAlert* pClone = new CLockOnAlert(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Stamina_Bar");
		Safe_Release(pClone);
	}
	
   	return pClone;
}

void CLockOnAlert::Free()
{
	CUI::Free();
}

