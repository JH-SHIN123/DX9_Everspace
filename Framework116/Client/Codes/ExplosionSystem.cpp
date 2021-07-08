#include "stdafx.h"
#include "ExplosionSystem.h"
#include "Pipeline.h"

CExplosionSystem::CExplosionSystem(LPDIRECT3DDEVICE9 pDevice)
	: CParticleSystem(pDevice)
{
}

CExplosionSystem::CExplosionSystem(const CExplosionSystem& other)
	: CParticleSystem(other)
{
}

void CExplosionSystem::ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute)
{
	if (nullptr == m_pTransform) return;

	attribute.isAlive = true;
	attribute.vPos = m_pTransform->Get_TransformDesc().vPosition;

	_float3 min = _float3(-1.0f, -1.0f, -1.0f);
	_float3 max = _float3(1.0f, 1.0f, 1.0f);

	CPipeline::GetRandomVector(
		&attribute.vVel,
		&min,
		&max);

	D3DXVec3Normalize(&attribute.vVel,&attribute.vVel);

	// Speed 적용
	attribute.vVel *= m_tResetAttribute.fParticleSpeed;
	attribute.tColor = D3DXCOLOR(
		CPipeline::GetRandomFloat(m_tResetAttribute.vColorRed_RandomRange),
		CPipeline::GetRandomFloat(m_tResetAttribute.vColorGreen_RandomRange),
		CPipeline::GetRandomFloat(m_tResetAttribute.vColorBlue_RandomRange),
	1.f);

	attribute.fAge = 0.f;
	attribute.fLifeTime = m_tResetAttribute.fLifeTime;
}

HRESULT CExplosionSystem::Ready_GameObject_Prototype()
{
	CParticleSystem::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CExplosionSystem::Ready_GameObject(void* pArg)
{
	CParticleSystem::Ready_GameObject(pArg);

	for (size_t i = 0; i < m_iNumParticles; ++i)
		AddParticle_ParticleSystem();

	return S_OK;
}

_uint CExplosionSystem::Update_GameObject(_float fDeltaTime)
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
				p.isAlive = false;
			}
		}
	}

	return m_pTransform->Update_Transform();
}

_uint CExplosionSystem::LateUpdate_GameObject(_float fDeltaTime)
{
	if (IsDead_ParticleSystem()) {
		m_IsDead = true;
		return DEAD_OBJECT;
	}

	CParticleSystem::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Particle, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CExplosionSystem::Render_GameObject()
{
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
	CParticleSystem::Render_GameObject();

	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	return _uint();
}

CExplosionSystem* CExplosionSystem::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CExplosionSystem* pInstance = new CExplosionSystem(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExplosionSystem::Clone(void* pArg)
{
	CExplosionSystem* pClone = new CExplosionSystem(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CExplosionSystem::Free()
{
	CParticleSystem::Free();
}
