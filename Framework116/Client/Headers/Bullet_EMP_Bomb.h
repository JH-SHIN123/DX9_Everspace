#pragma once
#ifndef __BULLET_EMP_BOMB_H__

#include "GameObject.h"

USING(Engine)
class CBullet_EMP_Bomb final : public CGameObject
{
public:
	explicit CBullet_EMP_Bomb(LPDIRECT3DDEVICE9 pDevice);
	explicit CBullet_EMP_Bomb(const CBullet_EMP_Bomb& other);
	virtual ~CBullet_EMP_Bomb() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Down(_float fDeltaTime);
	_uint Rotate_Ring(_float fDeltaTime);
	_uint Movement(_float fDeltaTime);
	_uint Homing(_float fDeltaTime);
	_uint Move_Rotate(_float fDeltaTime);

private:
	_uint Rotate_X(_float fDeltaTime);
	_uint Rotate_Y(_float fDeltaTime);

private:
	HRESULT Ready_GameObject_EMP();

public:
	static CBullet_EMP_Bomb* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CGeoMesh_Sphere*  m_pMesh = nullptr;
	CGeoMesh_Torus*  m_pRing_1 = nullptr;
	CGeoMesh_Torus*  m_pRing_2 = nullptr;
	CTransform*		m_pTransform = nullptr;
	CTransform*		m_pTransformRing_1 = nullptr;
	CTransform*		m_pTransformRing_2 = nullptr;
	CTransform*		m_pTargetTransform = nullptr;
	CCollideSphere* m_pCollide = nullptr;
	CStatus_Info*	m_pInfo = nullptr;

	_bool	m_IsBOOM		= false;
	_bool	m_IsTracking	= false;
	_bool	m_IsExplosion	= false;
	_bool	m_IsMove		= false;
	_float	m_fTurnTime		= 1.25f;
	_float	m_fLiveTime		= 11.25f;
	_float	m_fExplosionTime = 10.f;
	_float	m_fExplosionRadius = 1.f;
	_float3 m_vHomingDir = {};

	D3DMATERIAL9 m_tMaterial;

	class CGameObject* m_pEffect = nullptr;
};

#define __BULLET_EMP_BOMB_H__
#endif