#pragma once
#ifndef __PLAYERLIGHT_H__

#include "Light.h"

USING(Engine)
class CPlayerLight final : public CLight
{
private:
	explicit CPlayerLight(LPDIRECT3DDEVICE9 pDevice);
	explicit CPlayerLight(const CPlayerLight& other);
	virtual ~CPlayerLight() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CPlayerLight* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float3 m_vOffset = { 0.f, 0.f, 0.f };
};

#define __PLAYERLIGHT_H__
#endif