#pragma once
#ifndef __BULLET_EMP_BOMB_H__

#include "GameObject.h"

USING(Engine)
class CBullet_EMP_Bomb final : public CGameObject
{
public:
	explicit CBullet_EMP_Bomb(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData);
	explicit CBullet_EMP_Bomb(const CBullet_EMP_Bomb& other);
	virtual ~CBullet_EMP_Bomb() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint Fire_Triger(_float fDeltaTime);
	_uint Move_Dir(_float fDeltaTime);

public:
	static CBullet_EMP_Bomb* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData = nullptr);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer_CubeTexture*  m_pCube = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
	CCollideSphere* m_pCollide = nullptr;

	CTransform* m_pTargetTransform = nullptr;
	//CTransform* m_pParentTransform = nullptr;

	//_float m_fTrackingTime = 1.f;
	//_bool m_IsTracking = false;
	_float m_fLiveTime = 5.25f;
	_float m_fExplosionTime = 4.f;
	_float m_fExplosionRadius = 1.f;
	_float3 m_vMoveDir = {};
	_bool m_IsTracking = false;
	_bool m_IsExplosion = false;


};

#define __BULLET_EMP_BOMB_H__
#endif