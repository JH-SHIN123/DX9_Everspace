#pragma once

#ifndef __COLLISION_H__

#include "Engine_Include.h"

BEGIN(Engine)

class ENGINE_DLL CCollision
{
private:
	CCollision() = default;
	~CCollision() = default;

public:  // Picking & Ray
	static class CGameObject* PickingObject(float& OutDist, const HWND hWnd, const int iWinCX, const int iWinCY, const LPDIRECT3DDEVICE9 pDevice,
		const list<class CGameObject*>* listObject);

public: // Intersect Function
	static bool IntersectRayToSphere(const RAY& pInRay, const BOUNDING_SPHERE& pBounds);
	static bool IntersectSphereToSphere(const BOUNDING_SPHERE& pBounds1, const BOUNDING_SPHERE& pBounds2, const _float vDstScaleRate = 1.f, const _float vSrcScaleRate = 1.f);
};

END

#define __COLLISION_H__
#endif