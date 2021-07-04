#pragma once
#ifndef __BULLET_ENERGYBALL_H__

#include "GameObject.h"

USING(Engine)
class CBullet_EnergyBall final : public CGameObject
{
public:
	explicit CBullet_EnergyBall(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData);
	explicit CBullet_EnergyBall(const CBullet_EnergyBall& other);
	virtual ~CBullet_EnergyBall() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint Fire_Triger(_float fDeltaTime);

public:
	static CBullet_EnergyBall* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData = nullptr);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer_CubeTexture*  m_pCube = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
	CCollideSphere* m_pCollide = nullptr;

	CTransform* m_pTargetTransform = nullptr;
	//CTransform* m_pParentTransform = nullptr;

	_float m_fTrackingTime = 1.f;
	_bool m_IsTracking = false;
	_float m_fLiveTime = 10.f;


};

#define __BULLET_ENERGYBALL_H__
#endif