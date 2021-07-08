#pragma once
#ifndef __ALERTARROW_H__

#include "GameObject.h"

USING(Engine)
class CAlertArrow final : public CGameObject
{
public:
	explicit CAlertArrow(LPDIRECT3DDEVICE9 pDevice);
	explicit CAlertArrow(const CAlertArrow& other);
	virtual ~CAlertArrow() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CAlertArrow* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;

	CGameObject* m_pTargetMonster = nullptr;
	CTransform* m_pPlayerTransform = nullptr;
};

#define __ALERTARROW_H__
#endif