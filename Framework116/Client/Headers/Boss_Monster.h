#pragma once
#ifndef __BOSS_MONSTER_H__

#include "GameObject.h"



USING(Engine)

class CBoss_Monster final : public CGameObject
{
public:
	explicit CBoss_Monster(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData);
	explicit CBoss_Monster(const CBoss_Monster& other);
	virtual ~CBoss_Monster() = default;

public:
	enum AI_DIR { Forward, Back };

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint Check_Direction();

public:
	static CBoss_Monster* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer_CubeTexture*  m_pCube = nullptr;
	CTexture*	m_pTexture = nullptr;
	CTransform* m_pTransform = nullptr;
	CCollideSphere*	m_pCollide = nullptr;
	CTransform* m_pTargetTransform = nullptr;

private:
	PASSDATA_OBJECT* m_pPassData;

private:
	_float m_fCheckDir = 5.f;
};

#define __BOSS_MONSTER_H__
#endif