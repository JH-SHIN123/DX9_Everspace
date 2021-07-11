#pragma once

#ifndef __EXPLOSION_SYSTEM_H__
#include "ParticleSystem.h"

// ¼Óµµ / RGB ·£´ý°ª

USING(Engine)
class CExplosionSystem final : public CParticleSystem
{
private:
	explicit CExplosionSystem(LPDIRECT3DDEVICE9 pDevice);
	explicit CExplosionSystem(const CExplosionSystem& other);
	virtual ~CExplosionSystem() = default;

public:
	virtual void ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute) override;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CExplosionSystem* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};


#define __EXPLOSION_SYSTEM_H__
#endif