#pragma once

#ifndef __PARTICLESYSTEM_H__

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

typedef struct tagParticleSystemDesc : public BASE_DESC
{
	TRANSFORM_DESC tTransformDesc;
	wstring wstrTexturePrototypeTag = L"";
	_uint numParticles;
	float particleSize = 0.f;
}PARTICLESYSTEM_DESC;

class ENGINE_DLL CParticleSystem abstract : public CGameObject
{
public:
	struct tagAttribute
	{
		tagAttribute()
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
	};

protected:
	explicit CParticleSystem(LPDIRECT3DDEVICE9 pDevice);
	explicit CParticleSystem(const CParticleSystem& other);
	virtual ~CParticleSystem() = default;

public:
	bool IsEmpty_ParticleSystem();
	bool IsDead_ParticleSystem();

protected:
	virtual void Reset_ParticleSystem();
	virtual void ResetParticle_ParticleSystem(tagAttribute& attribute) PURE;
	virtual void AddParticle_ParticleSystem();

	virtual void PreRender_ParticleSystem();
	virtual void PostRender_ParticleSystem();

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
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	DWORD m_vbSize;
	DWORD m_vbOffset;
	DWORD m_vbBatchSize;

	CTransform*				m_pTransform;
	class CCollideSphere*	m_pCollide;
	class CTexture*			m_pTexture;

protected:
	list<tagAttribute> m_listParticles;
	_uint m_iMaxParticles = 0;
	float m_fEmitRate = 0.f; // 새로운 파티클들이 추가되는 레이트
	float m_fParticleSize = 0.f;

	wstring m_wstrTexturePrototypeTag = L"";
};
END

#define __PARTICLESYSTEM_H__
#endif