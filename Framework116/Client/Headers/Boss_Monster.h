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
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CBoss_Monster* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData = nullptr);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer_CubeTexture*  m_pCube = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
	CCollideSphere* m_pCollide = nullptr;
	CTransform* m_pTargetTransform = nullptr;

};

#define __BOSS_MONSTER_H__
#endif