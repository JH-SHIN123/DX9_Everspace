#include "stdafx.h"
#include "LoopExplosionSystem.h"
#include "Pipeline.h"

CLoopExplosionSystem::CLoopExplosionSystem(LPDIRECT3DDEVICE9 pDevice)
	: CExplosionSystem(pDevice)
{
}

CLoopExplosionSystem::CLoopExplosionSystem(const CLoopExplosionSystem& other)
	: CExplosionSystem(other)
{
}


_uint CLoopExplosionSystem::Update_GameObject(_float fDeltaTime)
{
	CParticleSystem::Update_GameObject(fDeltaTime);

	for (auto& p : m_listParticles)
	{
		if (p.isAlive)
		{
			p.vPos += p.vVel * fDeltaTime;
			p.fAge += fDeltaTime;

			p.tColor.r -= (p.fAge / p.fLifeTime) * m_tResetAttribute.fParticleAlphaFadeSpeed;
			p.tColor.g -= (p.fAge / p.fLifeTime) * m_tResetAttribute.fParticleAlphaFadeSpeed;
			p.tColor.b -= (p.fAge / p.fLifeTime) * m_tResetAttribute.fParticleAlphaFadeSpeed;

			if (p.tColor.r <= 0.f) p.tColor.r = 0.f;
			if (p.tColor.g <= 0.f) p.tColor.g = 0.f;
			if (p.tColor.b <= 0.f) p.tColor.b = 0.f;

			if (p.fAge > p.fLifeTime) {
				ResetParticle_ParticleSystem(p);
			}
		}
	}

	return m_pTransform->Update_Transform();
}

CLoopExplosionSystem* CLoopExplosionSystem::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLoopExplosionSystem* pInstance = new CLoopExplosionSystem(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoopExplosionSystem::Clone(void* pArg)
{
	CLoopExplosionSystem* pClone = new CLoopExplosionSystem(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLoopExplosionSystem::Free()
{
	CExplosionSystem::Free();
}
