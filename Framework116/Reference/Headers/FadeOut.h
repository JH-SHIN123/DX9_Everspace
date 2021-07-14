#pragma once

#ifndef __FADE_OUT_H__
#include "UI.h"

BEGIN(Engine)

class ENGINE_DLL CFadeOut final : public CUI
{
private:
	explicit CFadeOut(LPDIRECT3DDEVICE9 pDevice);
	explicit CFadeOut(const CFadeOut& other);
	virtual ~CFadeOut() = default;

public:
	_bool Fade_Out();

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CFadeOut* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool m_bFadeOut = false;
	_float m_fAlpha = 255.f;
	_float m_fAlphaSpeed = 1.5f;

private:
	class CScene* m_pScene = nullptr;
};

END

#define __FADE_OUT_H__
#endif