#include "..\Headers\Pipeline.h"

USING(Engine)

CPipeline::CPipeline()
{
}


CPipeline::~CPipeline()
{
}

void CPipeline::Setup_WorldMatrix(
	_float4x4 * pOut, 
	const _float3 * pScale, 
	const _float3 * pRotate, 
	const _float3 * pPosition)
{
	D3DXMatrixIdentity(pOut);

	_float3 vRight	= _float3(1.f, 0.f, 0.f);
	_float3 vUp		= _float3(0.f, 1.f, 0.f);
	_float3 vLook	= _float3(0.f, 0.f, 1.f);

	/* 스케일 */
	vRight.x *= pScale->x;
	vUp.y *= pScale->y;
	vLook.z *= pScale->z;


	D3DXQUATERNION Quaternion;
	//단위 사원수로 만들기
	D3DXQuaternionNormalize(&Quaternion, &Quaternion);
	_float4x4 matRot,matScale;
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot);
	matScale._11 = pScale->x;
	matScale._22 = pScale->y;
	matScale._33 = pScale->z;
	//Yaw Y(Up)축회전 pitch X(Right)축 회전	roll Z(Look벡터)축 회전.
	//쿼터니언 회전은 세축을 한번에 계산하기 때문에 따로 나누어져있지 않다
	//짐벌락이 일어나지 않음.
	D3DXQuaternionRotationYawPitchRoll(&Quaternion, pRotate->y,pRotate->x,pRotate->z);
	//나온 쿼터니언을 회전행렬로 바꿔줌
	D3DXMatrixRotationQuaternion(&matRot, &Quaternion);
	//스*자
	matScale *= matRot;
	*pOut = matScale;
	//이동
	memcpy(&pOut->_41, pPosition, sizeof(_float3));

	

}

void CPipeline::Setup_ViewMatrix(_float4x4* pOut, const _float4x4* matCameraWorld)
{
	D3DXMatrixIdentity(pOut);

	_float3 vRight = _float3(matCameraWorld->_11, matCameraWorld->_12, matCameraWorld->_13);
	_float3 vUp = _float3(matCameraWorld->_21, matCameraWorld->_22, matCameraWorld->_23);
	_float3 vLook = _float3(matCameraWorld->_31, matCameraWorld->_32, matCameraWorld->_33);
	_float3 vPos = _float3(matCameraWorld->_41, matCameraWorld->_42, matCameraWorld->_43);

	// Build the view matrix:
	float x = -D3DXVec3Dot(&vRight, &vPos);
	float y = -D3DXVec3Dot(&vUp, &vPos);
	float z = -D3DXVec3Dot(&vLook, &vPos);

	(*pOut)(0, 0) = vRight.x; (*pOut)(0, 1) = vUp.x; (*pOut)(0, 2) = vLook.x; (*pOut)(0, 3) = 0.0f;
	(*pOut)(1, 0) = vRight.y; (*pOut)(1, 1) = vUp.y; (*pOut)(1, 2) = vLook.y; (*pOut)(1, 3) = 0.0f;
	(*pOut)(2, 0) = vRight.z; (*pOut)(2, 1) = vUp.z; (*pOut)(2, 2) = vLook.z; (*pOut)(2, 3) = 0.0f;
	(*pOut)(3, 0) = x;        (*pOut)(3, 1) = y;     (*pOut)(3, 2) = z;       (*pOut)(3, 3) = 1.0f;
}

void CPipeline::Setup_ViewMatrix(
	_float4x4 * pOut, 
	const _float3 * pEye, 
	const _float3 * pAt, 
	const _float3 * pUp)
{
	D3DXMatrixIdentity(pOut);

	_float3 vRight	= _float3(1.f, 0.f, 0.f);
	_float3 vRealUp		= _float3(0.f, 1.f, 0.f);
	_float3 vLook	= _float3(0.f, 0.f, 1.f);

	vLook = *pAt - *pEye;
	D3DXVec3Normalize(&vLook, &vLook);

	D3DXVec3Cross(&vRight, pUp, &vLook); /* 외적 순서 주의!! */
	D3DXVec3Normalize(&vRight, &vRight);

	D3DXVec3Cross(&vRealUp, &vLook, &vRight); /* 외적 순서 주의!! */
	D3DXVec3Normalize(&vRealUp, &vRealUp);

	Setup_StateMatrix(pOut, &vRight, &vRealUp, &vLook, pEye);
	D3DXMatrixInverse(pOut, 0, pOut); /* 뷰행렬은 카메라행렬의 역행렬이다. */
}

void CPipeline::Setup_ProjectionMatrix(
	_float4x4 * pOut, 
	_float fFovY, 
	_float fAspect, 
	_float fNear, 
	_float fFar)
{
	D3DXMatrixIdentity(pOut);

	/* 
	시야각에 따라 사물이 축소 또는 확대 된다.
	x, y 범위가 -1 ~ 1인 투영스페이스에서 여러 시야각을 표현할 수 있다.
	*/
	_float fScale = 1.f / tanf(fFovY * 0.5f);

	pOut->_11 = fScale / fAspect; /* 뷰포트로 변환시 사이즈 왜곡을 방지하기 위해 미리 종횡비로 나눠놓는다 */
	pOut->_22 = fScale;

	/* 
	우리가 설정한 n ~ f를 0 ~ f로 만들기위한 원소 
	그래야 마지막에 z나누기를 했을 때 그 범위가 0 ~ 1이 된다.
	*/
	pOut->_33 = fFar / (fFar - fNear);
	pOut->_43 = (-fNear * fFar) / (fFar - fNear);

	/*
	나중에 Z나누기를 하기 위해서는 어딘가 Z를 보관하고 있어야 한다.
	결과 벡터의 w자리에 보관하기 위한 원소이다.
	*/
	pOut->_34 = 1.f;
	pOut->_44 = 0.f;
}

void CPipeline::Setup_OrthoMatrix(_float4x4* pOut, _float fWidth, _float fHeight, _float fNear, _float fFar)
{
	//원근투영과는 다르게 월드의 Z 를 반영하지
	//않습니다. (Far와 Near을 0,1 의 범위로
	//변환하는 과정은 없다는 뜻입니다.)

	//때문에 Z 는 고정값으로 
	//Far = 1
	//Near = 0 
	//으로 삼도록 합니다.
	D3DXMatrixIdentity(pOut);

	float w = 2.f / fWidth;
	float h = 2.f / fHeight;
	float a = 1.f; //1.f / (fFar - fNear);
	float b = 0.f; //-fNear * a;

	pOut->_11 = w;
	pOut->_22 = h;
	pOut->_33 = a;
	pOut->_43 = b;
}

void CPipeline::Setup_StateMatrix(
	_float4x4 * pOut, 
	const _float3 * pRight, 
	const _float3 * pUp, 
	const _float3 * pLook, 
	const _float3 * pPosition)
{
	D3DXMatrixIdentity(pOut);

	memcpy(&pOut->_11, pRight, sizeof(_float3));
	memcpy(&pOut->_21, pUp, sizeof(_float3));
	memcpy(&pOut->_31, pLook, sizeof(_float3));
	memcpy(&pOut->_41, pPosition, sizeof(_float3));
}

void CPipeline::RotateX(_float3 * pOut, _float3 vIn, _float fRadian)
{
	/*
	1	0	0	0
	0	c	s	0
	0	-s	c	0
	0	0	0	1
	*/
	pOut->y = vIn.y * cosf(fRadian) + vIn.z * -sinf(fRadian);
	pOut->z = vIn.y * sinf(fRadian) + vIn.z * cosf(fRadian);
}

void CPipeline::RotateY(_float3 * pOut, _float3 vIn, _float fRadian)
{
	/*
	c	0	-s	0
	0	1	0	0
	s	0	c	0
	0	0	0	1
	*/
	pOut->x = vIn.x * cosf(fRadian) + vIn.z * sinf(fRadian);
	pOut->z = vIn.x * -sinf(fRadian) + vIn.z * cosf(fRadian);
}

void CPipeline::RotateZ(_float3 * pOut, _float3 vIn, _float fRadian)
{	
	/*	
	c	s	0	0
	-s	c	0	0
	0	0	1	0
	0	0	0	1
	*/	
	pOut->x = vIn.x * cosf(fRadian) + vIn.y * -sinf(fRadian);
	pOut->y = vIn.x * sinf(fRadian) + vIn.y * cosf(fRadian);
}

void CPipeline::CreatePickingRay(RAY& pOutRay, const HWND hWnd, const int iWinCX, const int iWinCY, const LPDIRECT3DDEVICE9 pDevice)
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

void CPipeline::CreatePickingRay(RAY& pOutRay, const HWND hWnd, const int iWinCX, const int iWinCY, const LPDIRECT3DDEVICE9 pDevice, _float3 vSubjPos, _float3 vCamPos)
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

	_float4x4 invMatView;
	pDevice->GetTransform(D3DTS_VIEW, &invMatView);
	CPipeline::TransformRay(pOutRay, invMatView);

	// C Vector : Camera -> Mouse
	// A Vector : Camera -> Player
	_float3 cVector = pOutRay.vDirection;
	_float3 aVector = vSubjPos - vCamPos;
	D3DXVec3Normalize(&cVector, &cVector);
	D3DXVec3Normalize(&aVector, &aVector);

	_float3 bVector = cVector - aVector;
	D3DXVec3Normalize(&bVector, &bVector);

	pOutRay.vPos = vSubjPos;
	pOutRay.vDirection = bVector;
}

void CPipeline::TransformRay(RAY& pOutRay, _float4x4& matrix)
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

float CPipeline::Get_Distance(const _float3& vPos1, const _float3& vPos2)
{
	return sqrtf((vPos1.x - vPos2.x) * (vPos1.x - vPos2.x) + (vPos1.y - vPos2.y) * (vPos1.y - vPos2.y) + (vPos1.z - vPos2.z) * (vPos1.z - vPos2.z));
}

float CPipeline::GetRandomFloat(float lowBound, float highBound)
{
	if (lowBound >= highBound) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f;

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound;
}

float CPipeline::GetRandomFloat(_float2& vBounds)
{
	if (vBounds.x >= vBounds.y) // bad input
		return vBounds.x;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f;

	// return float in [lowBound, highBound] interval. 
	return (f * (vBounds.y - vBounds.x)) + vBounds.x;
}

void CPipeline::GetRandomVector(_float3* out, _float3* min, _float3* max)
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}
