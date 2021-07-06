#pragma once

#ifndef __MATERIALHANDLER_H__


class CMaterialHandler
{
public:
	CMaterialHandler();
	~CMaterialHandler();

public:
	// 오버로딩
	static void Set_RGBA(const _float& r, const _float& g, const _float& b, const _float& a, D3DMATERIAL9* pMaterial);
	static void Set_RGBA(const _float4& RGBA, D3DMATERIAL9* pMaterial);

	static void Set_Power(const _float& Power, D3DMATERIAL9* pMaterial);


};

#define __MATERIALHANDLER_H__
#endif // !__MATERIALHANDLER_H__
