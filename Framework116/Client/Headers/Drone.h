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
	enum State { Idle // 상하운동 전용
		, Turn, Move, BackTurn, BackMove // 랜덤한 방향, 일정한 범위
		, Die, End };

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
	_uint	State_Idle(_float fDelataTime);
	_uint	State_Turn(_float fDelataTime);
	_uint	State_Move(_float fDelataTime);
	_uint	State_BackTurn(_float fDelataTime);
	_uint	State_BackMove(_float fDelataTime);

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

public: // Add_HP_Bar
	_uint Add_Hp_Bar(_float fDeltaTime);
	void Set_Hp_Pos();
	_bool Get_Is_Hp_Bar() { return m_IsHPBar; }
	_uint Check_Degree();

private:
	CModelMesh* m_pModelMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CTransform* m_pTargetTransform = nullptr;
	CStatus_Info* m_pInfo = nullptr;

private:
	_float3 m_vSpawnPos;
	_float m_fDis = 20.f;
	_float m_fTurnSec = 0.f;
	_float m_fAngle = 0.f;
	_bool m_bAnglePlus = true;

	// 왔다리 > 돌아왔다리 > 랜덤 값 만큼 회전 > 왔다리 > 돌아 왔다리....

	// HP Bar 추가
	_bool m_IsHPBar = false;
	class CHP_Bar* m_pHp_Bar = nullptr;
	class CHP_Bar_Border* m_pHP_Bar_Border = nullptr;
	_float m_fHpLength = 32.f;

	// Lock On 추가
	class CNew_LockOn* m_pLockOn = nullptr;
	_bool m_IsLockOn = false;
	_uint Make_LockOn();

};

#define __DRONE_H__
#endif