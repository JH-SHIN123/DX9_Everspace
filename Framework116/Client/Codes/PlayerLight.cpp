#include "stdafx.h"
#include "PlayerLight.h"

CPlayerLight::CPlayerLight(LPDIRECT3DDEVICE9 pDevice) :
	CLight(pDevice)
{
}

CPlayerLight::CPlayerLight(const CPlayerLight& other) :
	CLight(other)
{
}

HRESULT CPlayerLight::Ready_GameObject_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CPlayerLight::Ready_GameObject(void* pArg)
{
	return E_NOTIMPL;
}

_uint CPlayerLight::Update_GameObject(_float fDeltaTime)
{
	return _uint();
}

_uint CPlayerLight::LateUpdate_GameObject(_float fDeltaTime)
{
	return _uint();
}

_uint CPlayerLight::Render_GameObject()
{
	return _uint();
}

CPlayerLight* CPlayerLight::Create(LPDIRECT3DDEVICE9 pDevice)
{
	return nullptr;
}

CGameObject* CPlayerLight::Clone(void* pArg)
{
	return nullptr;
}

void CPlayerLight::Free()
{
}
