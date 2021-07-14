#pragma once
#ifndef __NEW_LOCKON_H__

#include "GameObject.h"

USING(Engine)
class CNew_LockOn final : public CUI
{
public:
	enum MAKERID {MAKER_PLAYER, MAKER_BOSS_MONSTER, MAKER_MONSTER, MAKER_SNIPER, MAKER_DRONE, MAKER_END};

public:
	explicit CNew_LockOn(LPDIRECT3DDEVICE9 pDevice);
	explicit CNew_LockOn(const CNew_LockOn& other);
	virtual ~CNew_LockOn() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	// 뒤야 앞이야!
	void Set_IsBack(_bool bCheck) { m_IsBack = bCheck; }
	// 피깎는용도!
	void Set_ScaleX(_float _fDamage);
	void Set_Pos(_float3 _vPos) { m_pTransform->Set_Position(_vPos); }
	_float3 Get_Pos() { return m_pTransform->Get_State(EState::Position); }

	_uint Who_Make_Me(MAKERID _iMakerName);
	_uint Check_Degree();

private:
	_uint Movement(_float fDeltaTime);
	_uint Adjust_Pos(_float fDeltaTime);

public:
	static CNew_LockOn* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	const list<class CGameObject*>* m_listCheckMonsters = nullptr;
	_float3 vTargetPos = {};
	MAKERID m_eMakerID = MAKER_END;

private: // 뒤에있을땐 안나오게
	_bool m_IsBack = false;

private: // 플레이어 트랜스폼
	CTransform* m_pPlayerTransform = nullptr;
	// 최초참조시에 AddRef
	_bool m_IsRef = false;
	
};

#define __NEW_LOCKON_H__
#endif