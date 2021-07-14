#pragma once

#ifndef __BROKEN_PLANE_H__

#include "GameObject.h"

USING(Engine)

class CBrokenPlane final : public CGameObject
{
public:
	explicit CBrokenPlane(LPDIRECT3DDEVICE9 pDevice);
	explicit CBrokenPlane(const CBrokenPlane& other);
	virtual ~CBrokenPlane() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CBrokenPlane* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform* m_pTransform = nullptr;
	CModelMesh* m_pMesh = nullptr;
	CCollideSphere* m_pCollide = nullptr;
	CGameObject* m_pSmokeEffect = nullptr;

private:
	_bool m_bCreateEffect = false;
	_float m_fCreateEffectTime = 0.f;
	_float m_fMoveUpDeltaT = 0.f;
};

#define __BROKEN_PLANE_H__ 
#endif