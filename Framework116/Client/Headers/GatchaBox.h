#pragma once
#ifndef __GATCHABOX_H__

#include "GameObject.h"

USING(Engine)
class CGatchaBox final : public CGameObject
{
public:
	explicit CGatchaBox(LPDIRECT3DDEVICE9 pDevice);
	explicit CGatchaBox(const CGatchaBox& other);
	virtual ~CGatchaBox() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	void CheckPicking();
public:
	static CGatchaBox* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
};

#define __GATCHABOX_H__
#endif