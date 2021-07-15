#include "stdafx.h"
#include "..\Headers\Stamina_Bar.h"
#include "Pipeline.h"
#include "Collision.h"
#include "Player.h"

CStamina_Bar::CStamina_Bar(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CStamina_Bar::CStamina_Bar(const CStamina_Bar & other)
	: CUI(other)
{
}

HRESULT CStamina_Bar::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CStamina_Bar::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CUI::Ready_GameObject(pArg);
	return S_OK;
}

_uint CStamina_Bar::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);
	
	Adjust_Pos(fDeltaTime);

	return NO_EVENT;
}

_uint CStamina_Bar::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);

	if (m_IsDead)
		return DEAD_OBJECT;

	return _uint();
}

_uint CStamina_Bar::Render_GameObject()
{
	if (((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Get_IsAstroidStage() == false)
	{
		CUI::Render_GameObject();
	}
	return _uint();
}

void CStamina_Bar::Set_ScaleX(_float _fDamage)
{
	m_pTransform->Set_ScaleX(m_pTransform->Get_TransformDesc().vScale.x + _fDamage);
}

_uint CStamina_Bar::Movement(_float fDeltaTime)
{
	return _uint();
}

_uint CStamina_Bar::Adjust_Pos(_float fDeltaTime)
{
		if (m_pTransform) {
			m_pTransform->Set_Position(_float3(-(WINCX / 2.f) + 132.f, WINCY / 2.f - 120.f , 0.f));
			m_pTransform->Update_Transform();
		}

	return _uint();
}

_uint CStamina_Bar::Who_Make_Me(MAKERID _iMakerName)
{
	m_eMakerID = _iMakerName;
	return _uint();
}

CStamina_Bar * CStamina_Bar::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CStamina_Bar* pInstance = new CStamina_Bar(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Stamina_Bar");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject * CStamina_Bar::Clone(void * pArg/* = nullptr*/)
{
	CStamina_Bar* pClone = new CStamina_Bar(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Stamina_Bar");
		Safe_Release(pClone);
	}
	
   	return pClone;
}

void CStamina_Bar::Free()
{
	CUI::Free();
}

