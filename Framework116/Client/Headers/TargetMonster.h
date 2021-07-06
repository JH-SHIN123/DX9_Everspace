#pragma once
#ifndef __TARGETMONSTER_H__

#include "GameObject.h"

USING(Engine)
class CTargetMonster final : public CGameObject
{
public:
	explicit CTargetMonster(LPDIRECT3DDEVICE9 pDevice);
	explicit CTargetMonster(const CTargetMonster& other);
	virtual ~CTargetMonster() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_bool Collide_Check(_float fDeltaTime);

public:
	static CTargetMonster* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CTexture*	m_pTexture = nullptr;
	CTransform* m_pTransform = nullptr;
	CGeoMesh_Cylinder*  m_pGeoMesh = nullptr;
	CCollideSphere* m_pCollide = nullptr;

private:
	_float m_fReviveTime = 5.f;
	_float m_fColTime = 0.f;


	D3DMATERIAL9 m_tMaterial;
	_float4 vColorRGBA = { 0.8f, 0.f, 0.f, 0.f };

};

#define __TARGETMONSTER_H__ 
#endif