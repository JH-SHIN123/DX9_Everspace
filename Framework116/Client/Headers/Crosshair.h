#pragma once
#ifndef __CROSSHAIR_H__

#include "GameObject.h"

USING(Engine)
class CCrosshair final : public CGameObject
{
public:
	explicit CCrosshair(LPDIRECT3DDEVICE9 pDevice);
	explicit CCrosshair(const CCrosshair& other);
	virtual ~CCrosshair() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CCrosshair* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
};

#define __CROSSHAIR_H__
#endif