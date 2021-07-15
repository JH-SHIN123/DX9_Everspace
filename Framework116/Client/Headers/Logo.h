#pragma once
#ifndef __LOGO_H__

#include "Scene.h"

USING(Engine)
class CLogo final : public CScene
{
public:
	explicit CLogo(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLogo() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

public:
	static CLogo* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;

private:
	_bool	m_bFadeIn = false;

};

#define __LOGO_H__
#endif
