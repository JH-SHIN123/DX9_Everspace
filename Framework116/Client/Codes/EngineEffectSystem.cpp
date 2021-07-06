#include "stdafx.h"
#include "EngineEffectSystem.h"
#include "Pipeline.h"
#include "Player.h"

CEngineEffectSystem::CEngineEffectSystem(LPDIRECT3DDEVICE9 pDevice)
	: CParticleSystem(pDevice)
{
}

CEngineEffectSystem::CEngineEffectSystem(const CEngineEffectSystem& other)
	: CParticleSystem(other)
{
}

void CEngineEffectSystem::ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute)
{
	if (nullptr == m_pTransform) return;

	attribute.isAlive = true;
	attribute.vPos = m_vEngineOffset;

	_float3 min = _float3(-1.0f, -1.0f, -1.0f);
	_float3 max = _float3(1.0f, 1.0f, 1.0f);

	CPipeline::GetRandomVector(
		&attribute.vVel,
		&min,
		&max);

	D3DXVec3Normalize(&attribute.vVel, &attribute.vVel);

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

HRESULT CEngineEffectSystem::Ready_GameObject_Prototype()
{
	CParticleSystem::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CEngineEffectSystem::Ready_GameObject(void* pArg)
{
	CParticleSystem::Ready_GameObject(pArg);

	return S_OK;
}

_uint CEngineEffectSystem::Update_GameObject(_float fDeltaTime)
{
	CParticleSystem::Update_GameObject(fDeltaTime);

	// 부스트시, 파티클 길이(알파) 변경
	if (m_bBoost) {
		m_iNumParticles = 3;

		m_tResetAttribute.vColorRed_RandomRange = { 0.2f,0.2f };
		m_tResetAttribute.vColorGreen_RandomRange = { 0.2f,0.2f };
		m_tResetAttribute.vColorBlue_RandomRange = { 0.2f,0.2f };
		m_tResetAttribute.fParticleAlphaFadeSpeed = 0.1f;
		m_tResetAttribute.fParticleSize = 0.6f;
		m_tResetAttribute.fLifeTime = 2.5f;
	}
	else {
		m_iNumParticles = 1;

		m_tResetAttribute.vColorRed_RandomRange = { 0.08f,0.08f };
		m_tResetAttribute.vColorGreen_RandomRange = { 0.08f,0.08f };
		m_tResetAttribute.vColorBlue_RandomRange = { 0.08f,0.08f };
		m_tResetAttribute.fParticleAlphaFadeSpeed = 0.2f;
		m_tResetAttribute.fParticleSize = 0.7f;
		m_tResetAttribute.fLifeTime = 0.5f;
	}

	for (size_t i = 0; i < m_iNumParticles; ++i)
		AddParticle_ParticleSystem();

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
	RemoveDeadParticle_ParticleSystem();

	return m_pTransform->Update_Transform();
}

_uint CEngineEffectSystem::LateUpdate_GameObject(_float fDeltaTime)
{
	if (m_IsDead) {
		return DEAD_OBJECT;
	}

	CParticleSystem::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Particle, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CEngineEffectSystem::Render_GameObject()
{
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	CParticleSystem::Render_GameObject();

	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	return _uint();
}

CEngineEffectSystem* CEngineEffectSystem::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CEngineEffectSystem* pInstance = new CEngineEffectSystem(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEngineEffectSystem::Clone(void* pArg)
{
	CEngineEffectSystem* pClone = new CEngineEffectSystem(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CEngineEffectSystem::Free()
{
	CParticleSystem::Free();
}
