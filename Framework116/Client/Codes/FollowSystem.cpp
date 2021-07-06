#include "stdafx.h"
#include "FollowSystem.h"
#include "Pipeline.h"

CFollowSystem::CFollowSystem(LPDIRECT3DDEVICE9 pDevice)
	: CParticleSystem(pDevice)
{
}

CFollowSystem::CFollowSystem(const CFollowSystem& other)
	: CParticleSystem(other)
{
}

void CFollowSystem::ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute)
{
	if (nullptr == m_pTarget) return;

	CTransform* pTransform = (CTransform*)m_pTarget->Get_Component(L"Com_Transform");

	attribute.isAlive = true;

	// target pos, target direction
	attribute.vPos = pTransform->Get_State(EState::Position);

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

HRESULT CFollowSystem::Ready_GameObject_Prototype()
{
	CParticleSystem::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CFollowSystem::Ready_GameObject(void* pArg)
{
	CParticleSystem::Ready_GameObject(pArg);

	return S_OK;
}

_uint CFollowSystem::Update_GameObject(_float fDeltaTime)
{
	CParticleSystem::Update_GameObject(fDeltaTime);

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

	return  m_pTransform->Update_Transform();
}

_uint CFollowSystem::LateUpdate_GameObject(_float fDeltaTime)
{
	if (m_IsDead) return DEAD_OBJECT;

	CParticleSystem::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Particle, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CFollowSystem::Render_GameObject()
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

CFollowSystem* CFollowSystem::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CFollowSystem* pInstance = new CFollowSystem(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFollowSystem::Clone(void* pArg)
{
	CFollowSystem* pClone = new CFollowSystem(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CFollowSystem::Free()
{
	CParticleSystem::Free();
}
