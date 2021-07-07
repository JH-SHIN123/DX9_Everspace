#pragma once
#ifndef __TRANSFORM_H__

#include "Component.h"

BEGIN(Engine)
enum class EState
{
	Right,
	Up,
	Look,
	Position,
	End
};

typedef struct tagTransformDesc : public BASE_DESC
{
	tagTransformDesc() 
	{
		D3DXMatrixIdentity(&matWorld);

		D3DXQuaternionIdentity(&qRot);

		vAxis[(_uint)EState::Right] = { 1.f,0.f,0.f };
		vAxis[(_uint)EState::Up] = { 0.f,1.f,0.f };
		vAxis[(_uint)EState::Look] = { 0.f,0.f,1.f };
		vAxis[(_uint)EState::Position] = { 0.f,0.f,0.f };
	}

	_float3 vPosition = _float3(0.f, 0.f, 0.f);
	_float3 vScale = _float3(1.f, 1.f, 1.f);
	_float3 vRotate = _float3(0.f, 0.f, 0.f);
	_float4x4 matWorld;

	// 쿼터니언 회전 변수
	D3DXQUATERNION qRot;
	_float3 vAxis[(_uint)EState::End];

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
	void Set_State(EState eState, const _float3& vState);

	void Set_Position(const _float3& vPos);
	void Set_Rotate(const _float3& vRotate);
	void Set_Scale(const _float3& vScale);
	void Set_ScaleX(const _float fScaleX);
	void Set_ScaleY(const _float fScaleY);
	void Set_ScaleZ(const _float fScaleZ);

	void Set_SpeedPerSec(const _float fSpeedPerSec);
	void Set_RotatePerSec(const _float fRotatePerSec);

	void Set_WorldMatrix(const _float4x4& NewWorld);

public:
	const TRANSFORM_DESC& Get_TransformDesc() const;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void * pArg = nullptr) override;

public:
	_uint Update_Transform();
	_uint Update_Transform_Quaternion();

public:
	_uint Go_Straight(_float fDeltaTime);
	_uint Go_Side(_float fDeltaTime);
	_uint Go_Up(_float fDeltaTime);
	void Go_Dir(const _float3& vDir, _float fDeltaTime); //

	_uint RotateX(_float fDeltaTime);
	_uint RotateY(_float fDeltaTime);
	_uint RotateZ(_float fDeltaTime);
	_uint RotateAxis(_float3 vAxis, _float fDeltaTime);

	// Rotate Quaternion 
	_uint RotateX_Quaternion();
	_uint RotateY_Quaternion();
	_uint RotateZ_Quaternion();

	_uint Scale(const _float3& _vScale);
	_uint Move(const _float3& _vOffset);

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
