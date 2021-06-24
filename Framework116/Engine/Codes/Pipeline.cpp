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

	/* 자전 */
	//RotateX(&vRight, vRight, pRotate->x);
	//RotateY(&vRight, vRight, pRotate->y);
	//RotateZ(&vRight, vRight, pRotate->z);

	//RotateX(&vUp, vUp, pRotate->x);
	//RotateY(&vUp, vUp, pRotate->y);
	//RotateZ(&vUp, vUp, pRotate->z);

	//RotateX(&vLook, vLook, pRotate->x);
	//RotateY(&vLook, vLook, pRotate->y);
	//RotateZ(&vLook, vLook, pRotate->z);

	D3DXQUATERNION Quaternion;
	D3DXQuaternionNormalize(&Quaternion, &Quaternion);
	_float4x4 matRot,matScale;
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixIdentity(&matRot);
	matScale._11 = pScale->x;
	matScale._22 = pScale->y;
	matScale._33 = pScale->z;
	D3DXQuaternionRotationYawPitchRoll(&Quaternion, pRotate->y,pRotate->x,pRotate->z);
	D3DXMatrixRotationQuaternion(&matRot, &Quaternion);
	matScale *= matRot;
	*pOut = matScale;
	memcpy(&pOut->_41, pPosition, sizeof(_float3));
	//Setup_StateMatrix(pOut, &vRight, &vUp, &vLook, pPosition);

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

float CPipeline::Get_Distance(const _float3& vPos1, const _float3& vPos2)
{
	return sqrtf((vPos1.x - vPos2.x) * (vPos1.x - vPos2.x) + (vPos1.y - vPos2.y) * (vPos1.y - vPos2.y) + (vPos1.z - vPos2.z) * (vPos1.z - vPos2.z));
}
