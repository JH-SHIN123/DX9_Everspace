#pragma once

#ifndef __PARTICLESYSTEM_H__

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

typedef struct tagParticleResetAttribute {
	float	fParticleSize = 0.f;
	float	fEmitRate = 0.f;

	float	fParticleSpeed = 0.f;
	_float2 vColorRed_RandomRange = { 0.f,1.f };
	_float2 vColorGreen_RandomRange = { 0.f,1.f };
	_float2 vColorBlue_RandomRange = { 0.f,1.f };
	float	fLifeTime = 1.f;
}PARTICLE_RESETATTRIBUTE;

typedef struct tagParticleSystemDesc : public BASE_DESC
{
	TRANSFORM_DESC tTransformDesc;
	wstring wstrTexturePrototypeTag = L"";
	_uint iNumParticles = 0;
	
	// 파티클 리셋 어트리뷰트 파라미터
	PARTICLE_RESETATTRIBUTE tResetAttribute;
}PARTICLESYSTEM_DESC;

class ENGINE_DLL CParticleSystem abstract : public CGameObject
{
public:
	typedef struct tagParticleAttribute
{
	tagParticleAttribute()
	{
		fLifeTime = 0.f;
		fAge = 0.f;
		isAlive = true;
	}

	_float3		vPos;
	_float3		vVel;
	_float3		vAcc;
	float		fLifeTime;
	float		fAge;
	D3DXCOLOR	tColor;
	D3DXCOLOR	tColorFade;
	_bool		isAlive;
}PARTICLE_ATTRIBUTE;

protected:
	explicit CParticleSystem(LPDIRECT3DDEVICE9 pDevice);
	explicit CParticleSystem(const CParticleSystem& other);
	virtual ~CParticleSystem() = default;

public:
	bool IsEmpty_ParticleSystem();
	bool IsDead_ParticleSystem();

protected:
	virtual void Reset_ParticleSystem();
	virtual void ResetParticle_ParticleSystem(PARTICLE_ATTRIBUTE& attribute) PURE;
	virtual void AddParticle_ParticleSystem();

public:
	virtual HRESULT Ready_GameObject_Prototype() override PURE;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override PURE;
	virtual _uint Update_GameObject(_float fDeltaTime) override PURE;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override PURE;
	virtual _uint Render_GameObject() override PURE;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) override PURE;
	virtual void Free() override PURE;

protected: // Component
	// Component - VIBuffer_Particle
	LPDIRECT3DVERTEXBUFFER9 m_pVB = nullptr;
	DWORD m_vbSize = 0;
	DWORD m_vbOffset = 0;
	DWORD m_vbBatchSize = 0;

	CTransform*				m_pTransform = nullptr;
	class CTexture*			m_pTexture = nullptr;
	//class CCollideSphere*	m_pCollide;

protected:
	list<PARTICLE_ATTRIBUTE> m_listParticles;
	_uint m_iNumParticles = 0;

	wstring m_wstrTexturePrototypeTag = L"";
	PARTICLE_RESETATTRIBUTE m_tResetAttribute;
};
END

#define __PARTICLESYSTEM_H__
#endif