#pragma once
#ifndef __PLAYER_H__

#include "GameObject.h"

USING(Engine)
class CPlayer final : public CGameObject
{
public:
	explicit CPlayer(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pPassData);
	explicit CPlayer(const CPlayer& other);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	void	KeyProcess(_float fDeltaTime);
	_uint	Movement(_float fDeltaTime);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pPassData);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

	// For.Components
private:
	CMesh*  m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CController* m_pController = nullptr;

	// 플레이어가 사용하실 변수님들.
private:
	// 발사속도
	_float m_fMachinegunFireDelay = 0.f;

	//총열돌리는 시간
	_float m_fMachinegunDelay = 0.f;
	_float m_fLaserDelay = 0.f;
	_float m_fMissileDelay = 0.f;
	_bool m_IsLeft = false;

	// Player Skill
	_float m_fOverDriveTime = 20.f; // 20초 타이머
	_float m_fOverDrive = 1.f; // OverDrive 활성화 시엔 2.f;
	_bool m_bOverDrive = false;


	// enum 사용 권장
	//무기 1번 : 기관총, 2번 : 레이저
	_int m_iWeapon = 1;
	_bool m_IsLazer = false;
	_bool m_IsMissile = false;

	// Effect
	_bool m_IsBoost = false;


	// AlertArrow 관련
private:
	_uint Make_Arrow();
	const list<class CGameObject*>* m_listCheckMonsters = nullptr;
};

#define __PLAYER_H__
#endif