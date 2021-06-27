#include "stdafx.h"
#include "Explosion.h"
#include "Pipeline.h"

CExplosionSystem::CExplosionSystem(LPDIRECT3DDEVICE9 pDevice)
	: CParticleSystem(pDevice)
{
}

CExplosionSystem::CExplosionSystem(const CExplosionSystem& other)
	: CParticleSystem(other)
{
}

void CExplosionSystem::ResetParticle_ParticleSystem(tagAttribute& attribute)
{
	if (nullptr == m_pTransform) return;

	attribute.isAlive = true;
	attribute.vPos = m_pTransform->Get_State(EState::Position);

	_float3 min = _float3(-1.0f, -1.0f, -1.0f);
	_float3 max = _float3(1.0f, 1.0f, 1.0f);

	CPipeline::GetRandomVector(
		&attribute.vVel,
		&min,
		&max);

	D3DXVec3Normalize(&attribute.vVel,&attribute.vVel);

	// Speed 적용
	attribute.vVel *= 100.0f;
	attribute.tColor = D3DXCOLOR(
		CPipeline::GetRandomFloat(0.0f, 1.0f),
		CPipeline::GetRandomFloat(0.0f, 1.0f),
		CPipeline::GetRandomFloat(0.0f, 1.0f),
	1.f);

	attribute.fAge = 0.f;
	attribute.fLifeTime = 2.f;
}

void CExplosionSystem::PreRender_ParticleSystem()
{
	CParticleSystem::PreRender_ParticleSystem();

	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void CExplosionSystem::PostRender_ParticleSystem()
{
	CParticleSystem::PostRender_ParticleSystem();
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
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

			if (p.fAge > p.fLifeTime) {
				ResetParticle_ParticleSystem(p);
				//p.isAlive = false;
			}
		}
	}


	return m_pTransform->Update_Transform();
}

_uint CExplosionSystem::LateUpdate_GameObject(_float fDeltaTime)
{
	CParticleSystem::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CExplosionSystem::Render_GameObject()
{
	CParticleSystem::Render_GameObject();

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
