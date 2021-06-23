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

typedef struct tagRay
{
	D3DXVECTOR3 _origin = {0.f, 0.f, 0.f};
	D3DXVECTOR3 _direction = {0.f, 0.f, 0.f};
}RAY;

END

#define __ENGINE_STRUCT_H__
#endif