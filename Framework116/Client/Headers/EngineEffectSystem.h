#pragma once

#ifndef __ENGINE_EFFECT_SYSTEM_H__
#include "ParticleSystem.h"

USING(Engine)
class CEngineEffectSystem final : public CParticleSystem
{
private:
	explicit CEngineEffectSystem(LPDIRECT3DDEVICE9 pDevice);
	explicit CEngineEffectSystem(const CEngineEffectSystem& other);
	virtual ~CEngineEffectSystem() = default;

public:
	void Set_IsBoost(const _bool _bSwitch) { m_bBoost = _bSwitch; }
	void Set_EngineOffset(const _float3& _vOffset) { m_vEngineOffset = _vOffset; }

public:
	virtual void ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE & attribute) override;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CEngineEffectSystem* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool	m_bBoost = false;
	_float3 m_vEngineOffset = { 0.f,0.f,0.f };
};

#define __ENGINE_EFFECT_SYSTEM_H__
#endif