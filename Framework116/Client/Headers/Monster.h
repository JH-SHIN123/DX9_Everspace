#pragma once
#ifndef __MONSTER_H__

#include "GameObject.h"

USING(Engine)
class CMonster final : public CGameObject
{
public:
	explicit CMonster(LPDIRECT3DDEVICE9 pDevice);
	explicit CMonster(const CMonster& other);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint	Movement(_float fDeltaTime);
	_uint	Search_Target(_float fDeltaTime);
	_uint	Monster_Battle(_float fDeltaTime);
	_bool	RotateToPlayer(_float fDeltaTime);

public:
	static CMonster* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	_float3 m_vCreatePosition = { 0.f,0.f,0.f };
	_float3 m_vResearchRange = {0.f,0.f,0.f};

private:
	CModelMesh*  m_pModelMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CCollideSphere* m_pCollide = nullptr;

	CTransform* m_pTargetTransform = nullptr;

	//
	_bool	m_bBattle = false;
	_float	m_fMoveDist = 0.f;
	_float  m_fAttackDelay = 0.f;
	_bool m_IsLookingPlayer = false;
};

#define __MONSTER_H__
#endif