#pragma once

#ifndef __HUD_EEFECT_DAMAGE_H__
#define __HUD_EEFECT_DAMAGE_H__

#include "UI.h"

USING(Engine)
class CHUD_Effect_Damage : public CUI
{
public:
	explicit CHUD_Effect_Damage(LPDIRECT3DDEVICE9 pDevice);
	explicit CHUD_Effect_Damage(const CHUD_Effect_Damage& other);
	virtual ~CHUD_Effect_Damage() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CHUD_Effect_Damage* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	float m_fScaleRate = 1.f;
};


#endif // __HUD_EEFECT_DAMAGE_H__


