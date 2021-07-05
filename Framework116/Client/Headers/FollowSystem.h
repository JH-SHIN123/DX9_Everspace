#pragma once

#ifndef __FOLLOW_SYSTEM_H__

USING(Engine)
class CFollowSystem final : public CParticleSystem
{
private:
	explicit CFollowSystem(LPDIRECT3DDEVICE9 pDevice);
	explicit CFollowSystem(const CFollowSystem& other);
	virtual ~CFollowSystem() = default;

public:
	virtual void ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute) override;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CFollowSystem* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
};

#define __FOLLOW_SYSTEM_H__
#endif