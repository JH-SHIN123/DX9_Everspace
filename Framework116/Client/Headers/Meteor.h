#pragma once
#ifndef __METEOR_H__

#include "GameObject.h"

USING(Engine)
class CMeteor final : public CGameObject
{
public:
	explicit CMeteor(LPDIRECT3DDEVICE9 pDevice);
	explicit CMeteor(const CMeteor& other);
	virtual ~CMeteor() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CMeteor* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CTexture*	m_pTexture = nullptr;
	CTransform* m_pTransform = nullptr;
	CGeoMesh_Sphere*  m_pMesh = nullptr;
	CCollideSphere* m_pCollide = nullptr;

private:
	_float m_fSpawnTime = 2.f;

};

#define __METEOR_H__ 
#endif