#pragma once
#ifndef __AIMASSIST_H__

#include "GameObject.h"

USING(Engine)
class CAimAssist final : public CGameObject
{
public:
	explicit CAimAssist(LPDIRECT3DDEVICE9 pDevice);
	explicit CAimAssist(const CAimAssist& other);
	virtual ~CAimAssist() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CAimAssist* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
};

#define __AIMASSIST_H__
#endif