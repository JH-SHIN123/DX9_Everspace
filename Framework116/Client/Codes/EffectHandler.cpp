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

HRESULT CEffectHandler::Add_Layer_Effect_Bullet( CGameObject* pTarget, const _float _fSize, CGameObject** ppGameObject)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Smoke";
	pSystemDesc.iNumParticles = 2;
	pSystemDesc.tResetAttribute.fParticleSize = 3.f * _fSize;
	pSystemDesc.tResetAttribute.fParticleSpeed = 15.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.04f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.f;
	pSystemDesc.tResetAttribute.vColorRed_RandomRange = { 1.f,1.f };
	pSystemDesc.tResetAttribute.vColorGreen_RandomRange = { 1.f,1.f };
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
