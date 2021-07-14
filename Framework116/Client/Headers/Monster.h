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
	_uint	Shoot_Bullet(_float fDeltaTime);
	_bool	RotateToPlayer(_float fDeltaTime);

public: // Add_HP_Bar
	_uint Add_Hp_Bar(_float fDeltaTime);
	void Set_Hp_Pos();
	_bool Get_Is_Hp_Bar() { return m_IsHPBar; }

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

	// Pattern
	_float m_fPatternTime = 0.f;
	_bool m_bAttack = false;

	// HP Bar 추가
	_bool m_IsHPBar = false;
	class CHP_Bar* m_pHp_Bar = nullptr;
	class CHP_Bar_Border* m_pHP_Bar_Border = nullptr;
	_float m_fHpLength = 64.f;

	_uint Check_Degree();

	// Lock On 추가
	class CNew_LockOn* m_pLockOn = nullptr;
	_bool m_IsLockOn = false;
	_uint Make_LockOn();
	//////////////////////Effect///////////////////////////
private:
	void Update_Effect();

private: // Engine Effect
	_float3 m_vLeftEngineOffset = { 0.f, 0.f, 0.f };
	_float3 m_vRightEngineOffset = { 0.f, 0.f, 0.f };
	class CEngineEffectSystem* m_pLeftEngineEffect = nullptr;
	class CEngineEffectSystem* m_pRightEngineEffect = nullptr;

private: // Wing Effect
	_float3 m_vLeftWingOffset = { 0.f, 0.f, 0.f };
	_float3 m_vRightWingOffset = { 0.f, 0.f, 0.f };
	class CWingBoost_System* m_pLeftWingBoost = nullptr;
	class CWingBoost_System* m_pRightWingBoost = nullptr;

private:
	_bool m_IsBoost = false;
	//////////////////////////////////////////////////////////
};

#define __MONSTER_H__
#endif