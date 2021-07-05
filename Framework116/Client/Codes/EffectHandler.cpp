#include"stdafx.h"
#include "EffectHandler.h"

HRESULT CEffectHandler::Add_Layer_Effect_Explosion(const _float3& _vPos, const _float _fSize)
{
	PARTICLESYSTEM_DESC pSystemDesc;
	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Fire";
	pSystemDesc.iNumParticles = 100;
	pSystemDesc.tResetAttribute.fParticleSize = 10.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 40.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.02f;
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

	pSystemDesc.wstrTexturePrototypeTag = L"Component_Texture_Smoke";
	pSystemDesc.iNumParticles = 100;
	pSystemDesc.tResetAttribute.fParticleSize = 15.f;
	pSystemDesc.tResetAttribute.fParticleSpeed = 20.f;
	pSystemDesc.tResetAttribute.fParticleAlphaFadeSpeed = 0.01f;
	pSystemDesc.tResetAttribute.fLifeTime = 2.5f;

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
