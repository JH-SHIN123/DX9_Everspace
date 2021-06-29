#include "stdafx.h"
#include "LaserSystem.h"
#include "Pipeline.h"
#include "Transform.h"
#include "MainCam.h"

CLaserSystem::CLaserSystem(LPDIRECT3DDEVICE9 pDevice)
	: CParticleSystem(pDevice)
{
}

CLaserSystem::CLaserSystem(const CLaserSystem& other)
	: CParticleSystem(other)
{
}

void CLaserSystem::ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute)
{
	if (nullptr == m_pTransform) return;

	CTransform* pTransform = (CTransform*)CManagement::Get_Instance()->Get_Component(L"Layer_Player", L"Com_Transform");
	if (nullptr == pTransform) return;

	CMainCam* pCam = (CMainCam*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Cam");
	if (nullptr == pCam) return;

<<<<<<< HEAD
	attribute.isAlive = true;
	RAY ray;
	CPipeline::CreatePickingRay(ray, g_hWnd, WINCX, WINCY, m_pDevice);
	
	_float4x4 view;
	m_pDevice->GetTransform(D3DTS_VIEW, &view);
	CPipeline::TransformRay(ray, view);

	// C Vector : Camera -> Mouse
	// A Vector : Camera -> Player
	_float3 cVector = ray.vDirection;
	_float3 aVector = pTransform->Get_State(EState::Position) - pCam->Get_CameraDesc().vEye;
	D3DXVec3Normalize(&cVector, &cVector);
	D3DXVec3Normalize(&aVector,&aVector);
	_float3 bVector = cVector - aVector;
	D3DXVec3Normalize(&bVector, &bVector);

	attribute.vPos = pTransform->Get_State(EState::Position);
	attribute.vVel = bVector * m_tResetAttribute.fParticleSpeed;
=======
	RAY ray;
	CPipeline::CreatePickingRay(ray, g_hWnd, WINCX, WINCY, m_pDevice, 
		pTransform->Get_State(EState::Position), pCam->Get_CameraDesc().vEye);

	attribute.isAlive = true;

	attribute.vPos = ray.vPos;
	attribute.vVel = ray.vDirection * m_tResetAttribute.fParticleSpeed;
>>>>>>> origin/main

	attribute.tColor = D3DXCOLOR(
		CPipeline::GetRandomFloat(m_tResetAttribute.vColorRed_RandomRange),
		CPipeline::GetRandomFloat(m_tResetAttribute.vColorGreen_RandomRange),
		CPipeline::GetRandomFloat(m_tResetAttribute.vColorBlue_RandomRange),
		1.f);

	attribute.fAge = 0.f;
	attribute.fLifeTime = m_tResetAttribute.fLifeTime;
}

HRESULT CLaserSystem::Ready_GameObject_Prototype()
{
	CParticleSystem::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLaserSystem::Ready_GameObject(void* pArg)
{
	CParticleSystem::Ready_GameObject(pArg);

	return S_OK;
}

_uint CLaserSystem::Update_GameObject(_float fDeltaTime)
{
	CParticleSystem::Update_GameObject(fDeltaTime);

<<<<<<< HEAD
	//AddParticle_ParticleSystem();
=======
	AddParticle_ParticleSystem();
>>>>>>> origin/main

	for (auto& p : m_listParticles)
	{
		if (p.isAlive)
		{
			p.vPos += p.vVel * fDeltaTime;
			p.fAge += fDeltaTime;

			if (p.fAge > p.fLifeTime) {
				p.isAlive = false;
			}
		}
	}
	RemoveDeadParticle_ParticleSystem();

	//return m_pTransform->Update_Transform();
	return NO_EVENT;
}

_uint CLaserSystem::LateUpdate_GameObject(_float fDeltaTime)
{
	CParticleSystem::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Particle, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CLaserSystem::Render_GameObject()
{
	CParticleSystem::Render_GameObject();

	return _uint();
}

CLaserSystem* CLaserSystem::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLaserSystem* pInstance = new CLaserSystem(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLaserSystem::Clone(void* pArg)
{
	CLaserSystem* pClone = new CLaserSystem(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLaserSystem::Free()
{
	CParticleSystem::Free();
}
