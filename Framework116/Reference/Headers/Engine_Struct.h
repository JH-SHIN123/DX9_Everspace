#pragma once
#ifndef __ENGINE_STRUCT_H__

BEGIN(Engine)

typedef struct tagBaseDesc
{
	virtual ~tagBaseDesc() = default;
}BASE_DESC;

typedef struct tagVertexColor
{
	_float3 vPosition = _float3(0.f, 0.f, 0.f);
	_uint iColor = D3DCOLOR_ARGB(255, 255, 255, 255);
}VTX_COLOR;

const _uint FVF_VTX_COLOR = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

typedef struct tagVertexTexture
{
	_float3 vPosition = _float3(0.f, 0.f, 0.f);
	_float2 vUV = _float2(0.f, 0.f);	
}VTX_TEXTURE;

const _uint FVF_VTX_TEXTURE = D3DFVF_XYZ | D3DFVF_TEX1/* | D3DFVF_TEXCOORDSIZE2(0)*/;

typedef struct tagVertexCubeTexture
{
	_float3 vPosition = _float3(0.f, 0.f, 0.f);
	_float3 vUV = _float3(0.f, 0.f, 0.f);
}VTX_CUBETEXTURE;

const _uint FVF_VTX_CUBETEXTURE = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

typedef struct tagIndex16
{
	WORD _1, _2, _3;
}INDEX16;

typedef struct tagIndex32
{
	DWORD _1, _2, _3;
}INDEX32;

<<<<<<< HEAD
typedef struct tagPassingData_Object {

#ifdef _AFX
	CString wstrPrototypeTag;
	CString wstrPrototypeTag_Mesh;
#else 
	wstring wstrPrototypeTag_Object;
	wstring wstrPrototypeTag_Mesh;

	// Material Info
	D3DMATERIAL9 tMaterial;
#endif // _AFX

}PASSDATA_OBJECT;

typedef struct tagPassingData_Map {
	UINT eObjectType;
	_float4x4 matWorld;
}PASSDATA_MAP;
=======
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

	const _float3 Get_Position() const {
		return {matWorld._41 + vCenter.x, matWorld._42 + vCenter.y, matWorld._43 + vCenter.z};
	};

	// World
	_float4x4 matWorld;

	// 각 축을 향한 반지름 길이
	_float3 vCenter = { 0.f, 0.f ,0.f };
	_float fRadius = 1.f;
}BOUNDING_SPHERE;

typedef struct tagRay
{
	_float3 vPos = { 0.f, 0.f, 0.f };
	_float3 vDirection = { 0.f, 0.f, 0.f };
}RAY;

END
>>>>>>> origin/main

typedef struct tagPassingData_Route {
	_float3 vNodePos;
}PASSDATA_ROUTE;

typedef struct tagPassingData_Resource {
#ifdef _AFX
	CString wstrPrototypeTag;
	CString wstrFilePath;
#else 
	wstring wstrPrototypeTag;
	wstring wstrFilePath;
#endif // _AFX

	DWORD dwResourceType;
	DWORD dwTextureCount;
}PASSDATA_RESOURCE;

typedef struct tagPassingData_UI {
	UINT eObjectType;
	_float4x4 matWorld;
}PASSDATA_UI;
END
#define __ENGINE_STRUCT_H__
#endif

