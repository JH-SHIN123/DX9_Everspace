#pragma once
#ifndef __PLAYER_LAZER_H__

#include "GameObject.h"

USING(Engine)
class CPlayer_Lazer final : public CGameObject
{
public:
	explicit CPlayer_Lazer(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData);
	explicit CPlayer_Lazer(const CPlayer_Lazer& other);
	virtual ~CPlayer_Lazer() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CPlayer_Lazer* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData = nullptr);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CGeoMesh_Cylinder*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	//CTexture*	m_pTexture = nullptr;
	CCollideSphere* m_pCollide = nullptr;

private:
	//CTransform* m_pTargetTransform = nullptr;
	CTransform* m_pPlayerTransform = nullptr;

private:
	_float m_fLifeTime = 0.f;
	_bool m_IsLeft = true;
	_float3 m_vMuzzlePos = {};
	_float3 m_vPlayerLook = {};
	_bool m_IsFirst = true;

};

#define __PLAYER_LAZER_H__
#endif