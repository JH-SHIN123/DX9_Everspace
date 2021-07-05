#pragma once

#ifndef __LASER_SYSTEM_H__

USING(Engine)
class CLaserSystem final : public CParticleSystem
{
private:
	explicit CLaserSystem(LPDIRECT3DDEVICE9 pDevice);
	explicit CLaserSystem(const CLaserSystem& other);
	virtual ~CLaserSystem() = default;

public:
	virtual void ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute) override;
	
public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;
	
public:
	static CLaserSystem* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

#define __LASER_SYSTEM_H__
#endif