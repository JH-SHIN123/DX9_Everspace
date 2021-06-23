#include "Collision.h"

USING(Engine)

void CCollision::CreatePickingRay(RAY& pOutRay, const HWND hWnd, const int iWinCX, const int iWinCY, const LPDIRECT3DDEVICE9 pDevice)
{
	if (nullptr == pDevice) {
		PRINT_LOG(L"Error", L"CreatePickingRay - pDevice is nullptr");
		return;
	}

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	/* 뷰포트 -> 투영스페이스 */
	_float3 vMouse = _float3(0.f, 0.f, 0.f);
	vMouse.x = ptMouse.x / (iWinCX * 0.5f) - 1.f;
	vMouse.y = 1.f - ptMouse.y / (iWinCY * 0.5f);

	/* 투영스페이스 -> 뷰스페이스 */
	_float4x4 matProj;
	pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixInverse(&matProj, 0, &matProj);
	D3DXVec3TransformCoord(&vMouse, &vMouse, &matProj);

	/* 뷰스페이스 상에서 광선의 출발점과 방향을 구해준다. */
	pOutRay.vPos = { 0.f, 0.f, 0.f };
	pOutRay.vDirection = vMouse - pOutRay.vPos;
}

void CCollision::TransformRay(RAY& pOutRay, _float4x4& matrix)
{
	_float4x4 InvMatrix;
	D3DXMatrixInverse(&InvMatrix, 0, &matrix);

	// transform the ray's origin, w = 1.
	D3DXVec3TransformCoord(
		&pOutRay.vPos,
		&pOutRay.vPos,
		&InvMatrix);

	// transform the ray's direction, w = 0.
	D3DXVec3TransformNormal(
		&pOutRay.vDirection,
		&pOutRay.vDirection,
		&InvMatrix);

	// normalize the direction
	D3DXVec3Normalize(&pOutRay.vDirection, &pOutRay.vDirection);
}

bool CCollision::IntersectRayToSphere(const RAY& pInRay, const BOUNDING_SPHERE& pBounds)
{
	_float3 vBoundsToRay = pInRay.vPos - pBounds.Get_Position();

	float b = 2.f * D3DXVec3Dot(&pInRay.vDirection, &vBoundsToRay);
	float c =  D3DXVec3Dot(&vBoundsToRay, &vBoundsToRay) - (pBounds.fRadius * pBounds.fRadius);

	// b^2 - 4*c
	// 판별식을 찾는다.
	float discriminant = (b * b) - (4.0f * c);

	// 가상의 수에 대한 테스트
	if (discriminant < 0.0f)
		return false;

	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;

	// 해가 >= 0 일경우 구체를 교차하는 것이다.
	if (s0 >= 0.0f || s1 >= 0.0f)
		return true;

	return false;
}
