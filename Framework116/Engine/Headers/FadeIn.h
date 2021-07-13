#pragma once

#ifndef __FADE_IN_H__
#include "UI.h"

BEGIN(Engine)

class ENGINE_DLL CFadeIn final : public CUI
{
private:
	explicit CFadeIn(LPDIRECT3DDEVICE9 pDevice);
	explicit CFadeIn(const CFadeIn& other);
	virtual ~CFadeIn() = default;
	
public:
	_bool Fade_In();

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CFadeIn* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool m_bFadeIn = false;
	_float m_fAlpha = 0.f;
	_float m_fAlphaSpeed = 1.f;

private:
	class CScene* m_pScene = nullptr;
};

END

#define __FADE_IN_H__
#endif