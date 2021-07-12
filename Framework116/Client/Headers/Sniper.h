#pragma once
#ifndef __SNIPER_H__

#include "GameObject.h"

USING(Engine)
class CSniper final : public CGameObject
{
public:
	explicit CSniper(LPDIRECT3DDEVICE9 pDevice);
	explicit CSniper(const CSniper& other);
	virtual ~CSniper() = default;

public:
	enum State { Research, Warning, LockOn, Attack, Die, End };

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint	Movement(_float fDeltaTime);
	_uint	Researching(_float fDeltaTime);

private:
	void	StateCheck();

public:
	static CSniper* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	State m_eCurState = State::End;
	State m_eNextState = State::End;

private:
	_float3 m_vCreatePosition = { 0.f,0.f,0.f };
	_float3 m_vResearchRange = {0.f,0.f,0.f};

private:
	CModelMesh*  m_pModelMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CCollideSphere* m_pCollide = nullptr;

	CTransform* m_pPlayerTransform = nullptr;
};

#define __SNIPER_H__
#endif