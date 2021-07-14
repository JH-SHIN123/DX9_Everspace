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

public:
	_uint	Movement(_float fDeltaTime);
	_bool Get_IsLockOn() { return m_IsLockOn; }
	_bool RotateToPlayer(_float fDeltaTime);
	_uint Sniper_Battle(_float fDeltaTime);
	_uint Lock_On(_float fDeltaTime);

public: // Add_HP_Bar
	_uint Add_Hp_Bar(_float fDeltaTime);
	void Set_Hp_Pos();
	_bool Get_Is_Hp_Bar() { return m_IsHPBar; }

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
	CStatus_Info* m_pInfo = nullptr;

private:
	_bool m_IsBattle = false;
	_bool m_IsLookingPlayer = false;
	_bool m_IsLockOn = false;
	_bool m_IsTextile = false;

	// 락온 -> 발사 까지의 딜레이.
	_float m_fSniperShootDelay = 0.f;
	// 락온까지의 딜레이!
	_float m_fLockOnDelay = 0.f;

	// HP Bar 추가
	_bool m_IsHPBar = false;
	class CHP_Bar* m_pHp_Bar = nullptr;
	class CHP_Bar_Border* m_pHP_Bar_Border = nullptr;
	_float m_fHpLength = 64.f;

	_uint Check_Degree();
};

#define __SNIPER_H__
#endif