#pragma once
#ifndef __ENGINE_TYPEDEF_H__

BEGIN(Engine)

typedef bool _bool;

typedef char _byte;
typedef unsigned char _ubyte;

typedef int _int;
typedef unsigned int _uint;

typedef float _float;

typedef D3DXVECTOR2 _float2;
typedef D3DXVECTOR3 _float3;
typedef D3DXVECTOR4 _float4;
typedef D3DXMATRIX _float4x4;

END

#define __ENGINE_TYPEDEF_H__
#endif