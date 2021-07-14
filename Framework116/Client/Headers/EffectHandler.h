#pragma once

#ifndef __EFFECTHANDLER_H__

USING(Engine)
class CEffectHandler final
{
private:
	CEffectHandler() = default;
	~CEffectHandler() = default;

public:
	static HRESULT Add_Layer_Effect_Explosion(const _float3& _vPos, const _float _fSize);
	
	static HRESULT Add_Layer_Effect_Missile_Head(class CGameObject* pTarget, class CGameObject** ppGameObject = nullptr);
	static HRESULT Add_Layer_Effect_Missile_Smoke(class CGameObject* pTarget, class CGameObject** ppGameObject = nullptr);
	static HRESULT Add_Layer_Effect_Missile_Explosion(const _float3& _vPos);
	static HRESULT Add_Layer_Effect_Bullet(class CGameObject* pTarget, class CGameObject** ppGameObject = nullptr);
	static HRESULT Add_Layer_Effect_Gatling(class CGameObject* pTarget, class CGameObject** ppGameObject = nullptr);

	static HRESULT Add_Layer_Effect_Bullet_Explosion(const _float3& _vPos);

	static HRESULT Add_Layer_Effect_EngineBoost(class CGameObject** ppGameObject = nullptr);
	static HRESULT Add_Layer_Effect_WingBoost(class CGameObject** ppGameObject = nullptr);

	static HRESULT Add_Layer_Effect_Ring_Pass(const _float3& _vPos);
	static HRESULT Add_Layer_Effect_Particle_Yellow(const _float3& _vPos);

	static HRESULT Add_Layer_Effect_Boss_FireBullet(const _float3& _vPos, const _float _fSize);
	static HRESULT Add_Layer_Effect_BossBullet_EnergyBall_Trail(class CGameObject* pTarget, class CGameObject** ppGameObject = nullptr);
	static HRESULT Add_Layer_Effect_BossBullet_EnergyBall_Dead(const _float3& _vPos, const _float _fSize);
	static HRESULT Add_Layer_Effect_BossBullet_Laser_Alert(const _float3& _vPos, const _float _fSize);
	static HRESULT Add_Layer_Effect_BossBullet_Laser_Trail(class CGameObject* pTarget, class CGameObject** ppGameObject = nullptr);
	static HRESULT Add_Layer_Effect_BossBullet_Laser_Fire(const _float3& _vPos, const _float _fSize);
	static HRESULT Add_Layer_Effect_BossBullet_EMP_Trail(class CGameObject* pTarget, class CGameObject** ppGameObject = nullptr);
	static HRESULT Add_Layer_Effect_BossBullet_EMP_Exlposion(const _float3& _vPos, const _float _fSize);

	static HRESULT Add_Layer_Effect_Sniper_Bullet_Trail(class CGameObject* pTarget, class CGameObject** ppGameObject = nullptr);
	static HRESULT Add_Layer_Effect_BrokenPlane_Smoke(class CGameObject** ppGameObject = nullptr);
};

#define __EFFECTHANDLER_H__
#endif // !__EFFECTHANDLER_H__