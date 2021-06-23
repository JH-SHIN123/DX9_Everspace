#pragma once

#ifndef __COLLISION_H__

#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CCollision
{
private:
	CCollision() = default;
	~CCollision() = default;

public:
	static void CreatePickingRay(RAY& pOutRay, const HWND hWnd, const int iWinCX, const int iWinCY, const LPDIRECT3DDEVICE9 pDevice);
	static void TransformRay(RAY& pOutRay, _float4x4& matrix);
	static bool IntersectRayToSphere(const RAY& pInRay, const BOUNDING_SPHERE& pBounds);
};

END

#define __COLLISION_H__
#endif