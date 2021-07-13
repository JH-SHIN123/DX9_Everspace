#pragma once

#ifndef __HUD_EEFECT_BOOST_H__
#define __HUD_EEFECT_BOOST_H__

#include "UI.h"

USING(Engine)
class CHUD_Effect_Boost : public CUI
{
public:
	explicit CHUD_Effect_Boost(LPDIRECT3DDEVICE9 pDevice);
	explicit CHUD_Effect_Boost(const CHUD_Effect_Boost& other);
	virtual ~CHUD_Effect_Boost() = default;

public:
	void Set_Operate(const _bool bOnOff) { m_bOperate = bOnOff; }

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CHUD_Effect_Boost* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	bool m_bOperate = false;
	bool m_bSwitch = false;
	float m_fScaleDeltaT = 0.f;
	float m_fScaleRate = 1.4f;
};


#endif // __HUD_EEFECT_BOOST_H__


