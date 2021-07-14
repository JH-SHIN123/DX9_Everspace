#pragma once

#ifndef __LOOP_EXPLOSION_SYSTEM_H__
#include "ExplosionSystem.h"

USING(Engine)
class CLoopExplosionSystem final : public CExplosionSystem
{
private:
	explicit CLoopExplosionSystem(LPDIRECT3DDEVICE9 pDevice);
	explicit CLoopExplosionSystem(const CLoopExplosionSystem& other);
	virtual ~CLoopExplosionSystem() = default;

public:
	virtual _uint Update_GameObject(_float fDeltaTime) override;

public:
	static CLoopExplosionSystem* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

#define __EXPLOSION_SYSTEM_H__
#endif