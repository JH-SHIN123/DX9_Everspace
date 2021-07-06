#pragma once
#ifndef __PLAYER_BULLET_H__

#include "GameObject.h"

USING(Engine)
class CPlayer_Bullet final : public CGameObject
{
public:
	explicit CPlayer_Bullet(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData);
	explicit CPlayer_Bullet(const CPlayer_Bullet& other);
	virtual ~CPlayer_Bullet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CPlayer_Bullet* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData = nullptr);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer_CubeTexture*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CCollideSphere* m_pCollide = nullptr;
	D3DMATERIAL9	m_tMaterial;

private:
	//CTransform* m_pTargetTransform = nullptr;
	CTransform* m_pPlayerTransform = nullptr;
	//CGameObject* m_pBulletParticle = nullptr;

private:
	_float m_fLifeTime = 0.f;
	_bool m_IsLeft = true;
	_float3 m_vMuzzlePos = {0.f,0.f,0.f};
	_float3 m_vPlayerLook = { 0.f,0.f,0.f };
	_bool m_IsFirst = true;

};

#define __PLAYER_BULLET_H__
#endif