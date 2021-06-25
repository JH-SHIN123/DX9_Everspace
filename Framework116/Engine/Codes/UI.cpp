#include "UI.h"
#include "Transform.h"

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

	return S_OK;
}

_uint CUI::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	return _uint();
}

_uint CUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	return _uint();
}

_uint CUI::Render_GameObject()
{
	CGameObject::Render_GameObject();

	return _uint();
}

_uint CUI::IsBillboarding()
{
	// View의 역행렬, 회전행렬(자전) 곱
	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView);

	_float4x4 matWorld = m_pTransform->Get_TransformDesc().matWorld;

	/*
	matBill * matWorld
	matBill * 스 * 자 * 이 * 공 * 부
	*/
	m_pTransform->Set_WorldMatrix(matView * matWorld);

	return _uint();
}
