#pragma once

#ifndef __WINGBOOST_SYSTEM_H__
#include "ParticleSystem.h"

USING(Engine)
class CWingBoost_System final : public CParticleSystem
{
private:
	explicit CWingBoost_System(LPDIRECT3DDEVICE9 pDevice);
	explicit CWingBoost_System(const CWingBoost_System& other);
	virtual ~CWingBoost_System() = default;

public:
	void Set_IsBoost(const _bool _bSwitch) { m_bBoost = _bSwitch; }
	void Set_WingOffset(const _float3& _vOffset) { m_vWingOffset = _vOffset; }

public:
	virtual void ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute) override;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;
	
public:
	static CWingBoost_System* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool	m_bBoost = false;
	_float3 m_vWingOffset = { 0.f,0.f,0.f };
};

#define __ENGINE_EFFECT_SYSTEM_H__
#endif