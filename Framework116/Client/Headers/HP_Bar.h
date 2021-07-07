#pragma once
#ifndef __HP_BAR_H__

#include "GameObject.h"

USING(Engine)
class CHP_Bar final : public CUI
{
public:
	explicit CHP_Bar(LPDIRECT3DDEVICE9 pDevice);
	explicit CHP_Bar(const CHP_Bar& other);
	virtual ~CHP_Bar() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint IsBillboarding();

public:
	static CHP_Bar* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;

private:
	CGameObject* m_pTargetObject = nullptr;
	CCollide* m_pTargetCollide = nullptr;

	_float m_fHp = 30.f;
};

#define __HP_BAR_H__
#endif