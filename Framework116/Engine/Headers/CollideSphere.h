#pragma once

#ifndef __COLLIDE_H__

#include "Component.h"

BEGIN(Engine)

//typedef struct tagBoundingBox : public BASE_DESC
//{
//	D3DXVECTOR3 _min = { 0.f, 0.f, 0.f };
//	D3DXVECTOR3 _max = { 0.f, 0.f, 0.f };
//}BOUNDING_BOX;

typedef struct tagBoundingSphere : public BASE_DESC
{
	tagBoundingSphere() 
	{
		D3DXMatrixIdentity(&matWorld);
	}

	// World
	_float4x4 matWorld;

	// 각 축을 향한 반지름 길이
	_float3 vCenter = { 0.f, 0.f ,0.f };
	_float3 vRadius = { 1.f, 1.f ,1.f };
}BOUNDING_SPHERE;

class ENGINE_DLL CCollideSphere final : public CComponent
{
private:
	explicit CCollideSphere(LPDIRECT3DDEVICE9 pDevice);
	explicit CCollideSphere(const CCollideSphere& other);
	virtual ~CCollideSphere() = default;

public:
	const BOUNDING_SPHERE& Get_BoundingSphere() const;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;

	_uint	Update_Collide(const _float3& vScale, const _float3& vPos);
	_uint	Render_Collide();

public:
	static CCollideSphere* Create(LPDIRECT3DDEVICE9 pDevice);

	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	// 바운딩박스 여러개 설치할수 있도록 변경
	BOUNDING_SPHERE m_tBoundingSphere;
	LPD3DXMESH		m_pSphere; // 디버깅용

	D3DMATERIAL9	m_tMaterial;
};
END

#define __COLLIDE_H__
#endif