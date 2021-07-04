#pragma once
#ifndef __BOSS_WARMWHOLE_H__

#include "GameObject.h"

USING(Engine)
class CBoss_Warmhole final : public CGameObject
{
public:
	explicit CBoss_Warmhole(LPDIRECT3DDEVICE9 pDevice);
	explicit CBoss_Warmhole(const CBoss_Warmhole& other);
	virtual ~CBoss_Warmhole() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint Billboard(_float fDeltaTime);
	_uint Spin(_float fDeltaTime);
	_uint Scale(_float fDeltaTime);
	_uint Spawn_Monster(_float fDeltaTime);

public:
	static CBoss_Warmhole* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	//CVIBuffer_TerrainTexture* m_pTerrainBuffer = nullptr;
	//CCollideSphere* m_pCollide = nullptr;
	CTexture*	m_pTexture = nullptr;
	CTransform* m_pTransform = nullptr;
	CVIBuffer_RectTexture*  m_pVIBuffer = nullptr;

private:
	_float m_fOpenTime = 7.f;
	_float m_fSpawnTime = 3.f;
	_float m_fSpawnTime_Reset = 3.f;
	_bool m_bSpawn = false;
	_uint m_iSpawnCount = 10;
	_float3 m_vScale = { 5.f,5.f,5.f };

};

#define __BOSS_WARMWHOLE_H__ 
#endif