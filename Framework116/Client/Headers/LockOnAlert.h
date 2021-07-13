#pragma once
#ifndef __LOCKONALERT_H__

#include "GameObject.h"

USING(Engine)
class CLockOnAlert final : public CUI
{
public:
	enum MAKERID {MAKER_PLAYER, MAKER_BOSS_MONSTER, MAKER_MONSTER, MAKER_END};

public:
	explicit CLockOnAlert(LPDIRECT3DDEVICE9 pDevice);
	explicit CLockOnAlert(const CLockOnAlert& other);
	virtual ~CLockOnAlert() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	// ÇÇ±ð´Â¿ëµµ!
	void Set_ScaleX(_float _fDamage);
	_uint Who_Make_Me(MAKERID _iMakerName);

private:
	_uint Movement(_float fDeltaTime);
	_uint Adjust_Pos(_float fDeltaTime);

public:
	static CLockOnAlert* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	const list<class CGameObject*>* m_listCheckMonsters = nullptr;
	_float3 vTargetPos = {};
	MAKERID m_eMakerID = MAKER_END;

	_float m_fScaleX = 0.f;
	_float m_fScaleY = 0.f;
	_bool m_bAddScale = true;
	
};

#define __LOCKONALERT_H__
#endif