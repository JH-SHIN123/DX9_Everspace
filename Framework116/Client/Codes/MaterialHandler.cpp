#include "stdafx.h"
#include "..\Headers\MaterialHandler.h"


CMaterialHandler::CMaterialHandler()
{
}


CMaterialHandler::~CMaterialHandler()
{
}

void CMaterialHandler::Set_RGBA(const _float & r, const _float & g, const _float & b, const _float & a, D3DMATERIAL9 * pMaterial)
{
	(*pMaterial).Diffuse.r	= r;
	(*pMaterial).Ambient.r	= r;
	(*pMaterial).Specular.r = r;
	(*pMaterial).Emissive.r = r;
	 
	(*pMaterial).Diffuse.g	= g;
	(*pMaterial).Ambient.g	= g;
	(*pMaterial).Specular.g = g;
	(*pMaterial).Emissive.g = g;
	 
	(*pMaterial).Diffuse.b	= b;
	(*pMaterial).Ambient.b	= b;
	(*pMaterial).Specular.b = b;
	(*pMaterial).Emissive.b = b;
	 
	(*pMaterial).Diffuse.a	= a;
	(*pMaterial).Ambient.a	= a;
	(*pMaterial).Specular.a = a;
	(*pMaterial).Emissive.a = a;
}

void CMaterialHandler::Set_RGBA(const _float4 & RGBA, D3DMATERIAL9 * pMaterial)
{
	(*pMaterial).Diffuse.r	= RGBA.x;
	(*pMaterial).Ambient.r	= RGBA.x;
	(*pMaterial).Specular.r = RGBA.x;
	(*pMaterial).Emissive.r = RGBA.x;

	(*pMaterial).Diffuse.g	= RGBA.y;
	(*pMaterial).Ambient.g	= RGBA.y;
	(*pMaterial).Specular.g = RGBA.y;
	(*pMaterial).Emissive.g = RGBA.y;

	(*pMaterial).Diffuse.b	= RGBA.z;
	(*pMaterial).Ambient.b	= RGBA.z;
	(*pMaterial).Specular.b = RGBA.z;
	(*pMaterial).Emissive.b = RGBA.z;

	(*pMaterial).Diffuse.a	= RGBA.w;
	(*pMaterial).Ambient.a	= RGBA.w;
	(*pMaterial).Specular.a = RGBA.w;
	(*pMaterial).Emissive.a = RGBA.w;
}

void CMaterialHandler::Set_Power(const _float & Power, D3DMATERIAL9 * pMaterial)
{
	(*pMaterial).Power = Power;
}
