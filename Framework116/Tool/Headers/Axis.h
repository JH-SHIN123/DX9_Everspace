#pragma once
#ifndef __AXIS_H__

#include "GameObject.h"

USING(Engine)
class CAxis final : public CGameObject
{
public:
	explicit CAxis(LPDIRECT3DDEVICE9 pDevice);
	explicit CAxis(const CAxis& other);
	virtual ~CAxis() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CAxis* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CMesh*  m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
};

#define __AXIS_H__
#endif