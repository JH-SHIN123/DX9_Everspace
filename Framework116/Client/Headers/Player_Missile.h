#pragma once
#ifndef __PLAYER_MISSILE_H__

#include "GameObject.h"

USING(Engine)
class CPlayer_Missile final : public CGameObject
{
public:
	explicit CPlayer_Missile(LPDIRECT3DDEVICE9 pDevice);
	explicit CPlayer_Missile(const CPlayer_Missile& other);
	virtual ~CPlayer_Missile() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint Homing(_float fDeltaTime);
	_uint Searching_Target(_float fDeltaTime);
	_uint Search_Shortest_Target(_float fDeltaTime);

public:
	static CPlayer_Missile* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CGeoMesh_Cylinder*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
	CCollideSphere* m_pCollide = nullptr;
	CStatus_Info* m_pInfo = nullptr;

private:
	CTransform* m_pTargetTransform = nullptr;
	CTransform* m_pPlayerTransform = nullptr;

private:
	_float m_fLifeTime = 0.f;
	_bool m_IsLeft = true;
	_float3 m_vMuzzlePos = {};
	_float3 m_vPlayerLook = {};
	_bool m_IsFirst = true;

private:
	CGameObject* m_pHeadParticle = nullptr;
	CGameObject* m_pBulletParticle = nullptr;

private:
	_float m_fBeforeHoming = 0.f;
	_float m_fAddSpeed = 50.f;
	_float m_fRotateSpeed = D3DXToRadian(120.f);


private:
	_float3* vDir = {};


	// 가장 가까운 애 찾아갈 변수들
private:
	_float m_fDistToBoss = 0.f;
	_float m_fDistToDrone = 0.f;
	_float m_fDistToSniper = 0.f;
	_float m_fDistToNormalMonster = 0.f;

};

#define __PLAYER_MISSILE_H__
#endif