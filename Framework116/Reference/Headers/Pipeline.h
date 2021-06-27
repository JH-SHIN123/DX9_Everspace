#pragma once
#ifndef __PIPELINE_H__

#include "Engine_Include.h"

BEGIN(Engine)
class ENGINE_DLL CPipeline
{
private:
	CPipeline();
	~CPipeline();

public:
	static void Setup_WorldMatrix(_float4x4* pOut, const _float3* pScale, const _float3* pRotate, const _float3* pPosition);
	static void Setup_ViewMatrix(_float4x4* pOut, const _float4x4* matCameraWorld);
	static void Setup_ViewMatrix(_float4x4* pOut, const _float3* pEye, const _float3* pAt, const _float3* pUp);
	static void Setup_ProjectionMatrix(_float4x4* pOut, _float fFovY, _float fAspect, _float fNear, _float fFar); // 원근 (Perspective)
	static void Setup_OrthoMatrix(_float4x4* pOut, _float fWidth, _float fHeight, _float fNear, _float fFar); // 직교 Ortho
	static void Setup_StateMatrix(_float4x4* pOut, const _float3* pRight, const _float3* pUp, const _float3* pLook, const _float3* pPosition);

public:
	static void RotateX(_float3* pOut, _float3 vIn, _float fRadian);
	static void RotateY(_float3* pOut, _float3 vIn, _float fRadian);
	static void RotateZ(_float3* pOut, _float3 vIn,  _float fRadian);

public: // Math
	static float	Get_Distance(const _float3& vPos1, const _float3& vPos2);
	static float	GetRandomFloat(float lowBound, float highBound);
	static void		GetRandomVector(_float3* out, _float3* min, _float3* max);

public: // Etc
	static DWORD FtoDw(float f) { return *((DWORD*)&f); }
};
END

#define __PIPELINE_H__
#endif