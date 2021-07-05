#pragma once
#ifndef __LOCKON_H__

#include "GameObject.h"

USING(Engine)
class CLockOn final : public CGameObject
{
public:
	explicit CLockOn(LPDIRECT3DDEVICE9 pDevice);
	explicit CLockOn(const CLockOn& other);
	virtual ~CLockOn() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CLockOn* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;

private:
	_bool m_IsLockOn = false;
	_float4x4 m_Matview;

	// Lock On Target
	CCollideSphere* m_pTargetCollide = nullptr;
	wstring* m_pTargetLayerTag = {};
	wstring m_wstrLayerTag = {};
};

#define __LOCKON_H__
#endif