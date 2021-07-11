#pragma once
#ifndef __DRONE_H__

#include "GameObject.h"

USING(Engine)
class CDrone final : public CGameObject
{
public:
	explicit CDrone(LPDIRECT3DDEVICE9 pDevice);
	explicit CDrone(const CDrone& other);
	virtual ~CDrone() = default;

public:
	enum State { Research, Die, End };

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint	Movement(_float fDeltaTime);
	_uint	Researching(_float fDeltaTime);

private:
	void	StateCheck();

//private: // Add_HP_Bar
//	_uint Add_Hp_Bar(_float fDeltaTime);

public:
	static CDrone* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	State m_eCurState = State::End;
	State m_eNextState = State::End;

private:
	_float3 m_vCreatePosition = { 0.f,0.f,0.f };
	_float3 m_vResearchRange = { 0.f,0.f,0.f };

//private: // HP Bar Ãß°¡
//	_bool m_IsHPBar = false;
//	class CHP_Bar* m_pHp_Bar = nullptr;
//	class CHP_Bar_Border* m_pHP_Bar_Border = nullptr;
//	_float m_fHpLength = 64.f;

private:
	CModelMesh* m_pModelMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CTransform* m_pTargetTransform = nullptr;

};

#define __DRONE_H__
#endif