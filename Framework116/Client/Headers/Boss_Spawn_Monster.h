#pragma once
#ifndef __BOSS_SPAWN_MONSTER_H__

#include "GameObject.h"

USING(Engine)
class CBoss_Spawn_Monster final : public CGameObject
{
public:
	explicit CBoss_Spawn_Monster(LPDIRECT3DDEVICE9 pDevice);
	explicit CBoss_Spawn_Monster(const CBoss_Spawn_Monster& other);
	virtual ~CBoss_Spawn_Monster() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CBoss_Spawn_Monster* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	//CVIBuffer_TerrainTexture* m_pTerrainBuffer = nullptr;
	CTexture*	m_pTexture = nullptr;
	CTransform* m_pTransform = nullptr;
	CVIBuffer*  m_pVIBuffer = nullptr;
	CCollideSphere* m_pCollide = nullptr;

private:
	_float m_fSpawnTime = 2.f;

};

#define __BOSS_SPAWN_MONSTER_H__ 
#endif