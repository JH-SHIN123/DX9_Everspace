#pragma once
#ifndef __RING_H__

#include "GameObject.h"

USING(Engine)
class CRing final : public CGameObject
{
public:
	explicit CRing(LPDIRECT3DDEVICE9 pDevice);
	explicit CRing(const CRing& other);
	virtual ~CRing() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_bool CollideCheck();

public:
	static CRing* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CTexture*	m_pTexture = nullptr;
	CTransform* m_pTransform = nullptr;
	//CVIBuffer*  m_pVIBuffer = nullptr;
	CGeoMesh_Torus* m_pGeoMesh = nullptr;
	CCollideSphere* m_pCollide = nullptr;

private:
	D3DMATERIAL9 m_tMaterial;
	_bool m_bHitRing = false;
	//COLLIDES m_TargetCollide;
	//_float m_fSpawnTime = 2.f;
};

#define __RING_H__ 
#endif