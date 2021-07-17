#include"stdafx.h"
#include "EffectHandler.h"

HRESULT CEffectHandler::Add_Layer_Effect_Explosion(const _float3& _vPos, const _float _fSize)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Fire";
	pSystemDesc.iNumParticles = 20;
	pSystemDesc.tResetAttribute.fParticleSize = 20.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 30.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.04f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.5f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Plasma";
	pSystemDesc.iNumParticles = 20;
	pSystemDesc.tResetAttribute.fParticleSize = 25.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 15.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.01f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 0.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 0.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 0.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Explosion_GatchaBox(const _float3& _vPos, const _float _fSize)
{
	// ¿¬±â + ÆøÁ×

	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Effect_Party";
	pSystemDesc.iNumParticles = 40;
	pSystemDesc.tResetAttribute.fParticleSize = 2.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 10.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.01f;
	pSystemDesc.tResetAttribute.fLifeTime = 3.f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Plasma";
	pSystemDesc.iNumParticles = 30;
	pSystemDesc.tResetAttribute.fParticleSize = 3.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 10.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.01f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 0.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 0.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 0.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Bullet_Explosion(const _float3& _vPos)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Plasma";
	pSystemDesc.iNumParticles = 5;
	pSystemDesc.tResetAttribute.fParticleSize = 2.5f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 2.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.05f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Missile_Head(CGameObject* pTarget, CGameObject** ppGameObject)
{
	// Normal
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Spark";
	pSystemDesc.iNumParticles = 1;
	pSystemDesc.tResetAttribute.fParticleSize = 2.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 3.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;

	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 0.28f,0.28f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 0.28f,0.28f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 0.28f,0.28f };
	pSystemDesc.pTarget = pTarget;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		L"Layer_FollowSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Follow In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Missile_Smoke( CGameObject* pTarget, CGameObject** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Smoke";
	pSystemDesc.iNumParticles = 2;
	pSystemDesc.tResetAttribute.fParticleSize = 1.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 1.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.01f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.5f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 0.5f,0.5f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 0.5f,0.5f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 0.5f,0.5f };
	pSystemDesc.pTarget = pTarget;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		L"Layer_FollowSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Follow In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Missile_Explosion(const _float3& _vPos)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Spark";
	pSystemDesc.iNumParticles = 10;
	pSystemDesc.tResetAttribute.fParticleSize = 8.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 15.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.05f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 0.6f,0.6f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 0.6f,0.6f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 0.6f,0.6f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Bullet(CGameObject* pTarget, CGameObject** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Smoke";
	pSystemDesc.iNumParticles = 2;
	pSystemDesc.tResetAttribute.fParticleSize = 1.2f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 5.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.4f;
	pSystemDesc.tResetAttribute.fLifeTime = 0.2f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 0.f,0.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 0.8f,0.8f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };
	pSystemDesc.pTarget = pTarget;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		L"Layer_FollowSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Follow In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Gatling(CGameObject * pTarget, CGameObject ** ppGameObject)
{
	/*PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Smoke";
	pSystemDesc.iNumParticles = 1;
	pSystemDesc.tResetAttribute.fParticleSize = 0.5f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 5.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.05f;
	pSystemDesc.tResetAttribute.fLifeTime = 0.2f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 0.f, 0.f };
	pSystemDesc.pTarget = pTarget;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		L"Layer_FollowSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Follow In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);*/

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_EngineBoost(CGameObject** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Boost";
	pSystemDesc.iNumParticles = 1;
	pSystemDesc.tResetAttribute.fParticleSize = 0.7f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 2.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.2f;
	pSystemDesc.tResetAttribute.fLifeTime = 0.5f;

	// Normal
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 0.08f,0.08f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 0.08f,0.08f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 0.08f,0.08f };


	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_EngineEffectSystem",
		L"Layer_EngineEffectSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle LoopExplosion In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_WingBoost(CGameObject** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Smoke";
	pSystemDesc.iNumParticles = 2;
	pSystemDesc.tResetAttribute.fParticleSize = 0.6f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 0.01f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.0001f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 0.05f,0.05f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 0.05f,0.05f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 0.05f,0.05f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_WingBoostSystem",
		L"Layer_WingBoostSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle LoopExplosion In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Ring_Pass(const _float3& _vPos)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Ring_Pass";
	pSystemDesc.iNumParticles = 1;
	pSystemDesc.tResetAttribute.fParticleSize = 5.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 5.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.05f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Particle_Yellow(const _float3 & _vPos)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Particle_Yellow";
	pSystemDesc.iNumParticles = 10;
	pSystemDesc.tResetAttribute.fParticleSize = 1.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 20.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Yellow In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Boss_FireBullet(const _float3 & _vPos, const _float _fSize)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Fire_Effet";
	pSystemDesc.iNumParticles = 2;
	pSystemDesc.tResetAttribute.fParticleSize = 20.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 5.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.001f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Yellow In Layer");
	}

	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Dust_Effet";
	pSystemDesc.iNumParticles = 5;
	pSystemDesc.tResetAttribute.fParticleSize = 10.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 10.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.01f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_BossBullet_EnergyBall_Trail(class CGameObject* pTarget, class CGameObject** ppGameObject, _float fScale)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Bullet_Trail_Puple";
	pSystemDesc.iNumParticles = 1;
	pSystemDesc.tResetAttribute.fParticleSize = 3.f + fScale;
	pSystemDesc.tResetAttribute.fParticleSpeed = 4.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;

	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f, 1.f };
	pSystemDesc.pTarget = pTarget;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		L"Layer_FollowSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Follow In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_BossBullet_EnergyBall_Dead(const _float3 & _vPos, const _float _fSize)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Bullet_Trail_Puple";
	pSystemDesc.iNumParticles = 5;
	pSystemDesc.tResetAttribute.fParticleSize = 3.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 5.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Yellow In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_Sniper_Bullet_Trail(CGameObject * pTarget, CGameObject ** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Sniper_Bullet_Trail";
	pSystemDesc.iNumParticles = 1;
	pSystemDesc.tResetAttribute.fParticleSize = 1.4f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 2.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 10.f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;

	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f, 1.f };
	pSystemDesc.pTarget = pTarget;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		L"Layer_FollowSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Follow In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}
HRESULT CEffectHandler::Add_Layer_Effect_BrokenPlane_Smoke(class CGameObject** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Smoke";
	pSystemDesc.iNumParticles = 30;
	pSystemDesc.tResetAttribute.fParticleSize = 5.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 10.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.01f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_LoopExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Explosion In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}
HRESULT CEffectHandler::Add_Layer_Effect_BossBullet_Laser_Alert(const _float3 & _vPos, const _float _fSize)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_BossLaser_Fire";
	pSystemDesc.iNumParticles = 3;
	pSystemDesc.tResetAttribute.fParticleSize = 5.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 2.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.5f;
	pSystemDesc.tResetAttribute.fLifeTime = 0.8f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Yellow In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_BossBullet_Laser_Trail(CGameObject * pTarget, CGameObject ** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_BossLaser_Trail";
	pSystemDesc.iNumParticles = 1;
	pSystemDesc.tResetAttribute.fParticleSize = 3.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 3.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;

	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f, 1.f };
	pSystemDesc.pTarget = pTarget;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		L"Layer_FollowSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Follow In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_BossBullet_Laser_Fire(const _float3 & _vPos, const _float _fSize)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_BossLaser_Fire";
	pSystemDesc.iNumParticles = 5;
	pSystemDesc.tResetAttribute.fParticleSize = 10.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 2.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.5f;
	pSystemDesc.tResetAttribute.fLifeTime = 0.75f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Yellow In Layer");
	}

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_BossBullet_EMP_Trail(CGameObject * pTarget, CGameObject ** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_BossEMP_2";
	pSystemDesc.iNumParticles = 2;
	pSystemDesc.tResetAttribute.fParticleSize = 5.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 3.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 1.f;

	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f, 1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f, 1.f };
	pSystemDesc.pTarget = pTarget;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_FollowSystem",
		L"Layer_FollowSystem",
		(void*)&pSystemDesc,
		(CGameObject**)ppGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Follow In Layer");
	}

	CGameObject* pGameObject = *ppGameObject;
	Safe_Release(pGameObject);

	return S_OK;
}

HRESULT CEffectHandler::Add_Layer_Effect_BossBullet_EMP_Exlposion(const _float3 & _vPos, const _float _fSize)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_BossEMP";
	pSystemDesc.iNumParticles = 30;
	pSystemDesc.tResetAttribute.fParticleSize = 40.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 300.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Yellow In Layer");
	}

	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_BossEMP_Explosion";
	pSystemDesc.iNumParticles = 10;
	pSystemDesc.tResetAttribute.fParticleSize = 20.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 350.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.5f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Yellow In Layer");
	}

	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_BossEMP_2";
	pSystemDesc.iNumParticles = 10;
	pSystemDesc.tResetAttribute.fParticleSize = 40.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 350.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.5f;
	pSystemDesc.tTransformDesc.vPosition = _vPos;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorBlue_RandomRange = { 1.f,1.f };

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_ExplosionSystem",
		L"Layer_ExplosionSystem",
		(void*)&pSystemDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Particle Yellow In Layer");
	}


	return S_OK;
}
