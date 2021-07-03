#pragma once
#ifndef __BOSS_MONSTER_H__

#include "GameObject.h"

#define BOSSRANGE_NEAR		20.f
#define BOSSRANGE_MIDDLE	50.f
#define BOSSRANGE_FAR		100.f

USING(Engine)
class CBoss_Monster final : public CGameObject
{
public:
	explicit CBoss_Monster(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData);
	explicit CBoss_Monster(const CBoss_Monster& other);
	virtual ~CBoss_Monster() = default;

public:
	enum ActionMode {Near, Middle, Far, SpecialAction, End };

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private: // Update 함수에 쓸 최상위 함수
	_uint Move_AI(_float fDeltaTime);
	_uint Attack_AI(_float fDeltaTime);

private:
	_uint Movement(_float fDeltaTime);
	_uint Move_Near(_float fDeltaTime);
	_uint Move_Middle(_float fDeltaTime);
	_uint Move_Far(_float fDeltaTime);

	_uint Fire_Triger(_float fDeltaTime);
	_uint Fire_Laser(_float fDeltaTime);
	_uint Fire_EMP(_float fDeltaTime);


public:
	static CBoss_Monster* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData = nullptr);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

public:
	HRESULT Add_InLayer_MyParts();

private:
	CVIBuffer_CubeTexture*  m_pCube = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
	CCollideSphere* m_pCollide = nullptr;
	CTransform* m_pTargetTransform = nullptr;


private: /* AI */
	// 거리에 따라 행동을 다르게
	ActionMode m_eActionMode = End;
	_bool m_IsSpecialAction = false;

	// 타겟 탐지 범위
	//const _float m_fDetectionRange_Near = UNIT * 20.f;
	//const _float m_fDetectionRange_Middle = UNIT * 50.f;
	//const float m_fDetectionRange_Far = UNIT * 100.f;
	

private: /* Skills */
	// energyball
	_bool m_IsLeftFire = false;
	_float m_fEnergyBall_CoolTime = 0.f;

	// laser
	_float m_fLaser_CoolTime = 0.f;

	// emp bomb
	_float m_fEmpBomb_CoolTime = 0.f;

};

#define __BOSS_MONSTER_H__
#endif