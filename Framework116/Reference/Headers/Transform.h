#pragma once
#ifndef __TRANSFORM_H__

#include "Component.h"

BEGIN(Engine)
enum class EState
{
	Right,
	Up,
	Look,
	Position
};

typedef struct tagTransformDesc : public BASE_DESC
{
	tagTransformDesc() 
	{
		D3DXMatrixIdentity(&matWorld);
	}

	_float3 vPosition = _float3(0.f, 0.f, 0.f);
	_float3 vScale = _float3(1.f, 1.f, 1.f);
	_float3 vRotate = _float3(0.f, 0.f, 0.f);
	_float4x4 matWorld;

	_float fSpeedPerSec = 0.f;
	_float fRotatePerSec = 0.f;
}TRANSFORM_DESC;

class ENGINE_DLL CTransform : public CComponent
{
private:
	explicit CTransform(LPDIRECT3DDEVICE9 pDevice);
	explicit CTransform(const CTransform& other);
	virtual ~CTransform() = default;

public:
	_float3 Get_State(EState eState) const;

public:
	void Set_Position(const _float3& vPos);
	void Set_Scale(const _float3& vScale);
	void Set_ScaleX(const _float fScaleX);
	void Set_ScaleY(const _float fScaleY);
	void Set_ScaleZ(const _float fScaleZ);

	void Set_WorldMatrix(const _float4x4& NewWorld);

public:
	const TRANSFORM_DESC& Get_TransformDesc() const;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void * pArg = nullptr) override;

public:
	_uint Update_Transform();
	_uint Go_Straight(_float fDeltaTime);
	_uint Go_Side(_float fDeltaTime);

	_uint RotateX(_float fDeltaTime);
	_uint RotateY(_float fDeltaTime);
	_uint RotateZ(_float fDeltaTime);

	_uint Scale(const _float3& _vScale);

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	TRANSFORM_DESC m_TransformDesc;
};
END

#define __TRANSFORM_H__
#endif
