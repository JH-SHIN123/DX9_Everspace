#pragma once
#ifndef __VIBUFFER_TERRAINTEXTURE_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_TerrainTexture final : public CVIBuffer
{
private:
	explicit CVIBuffer_TerrainTexture(LPDIRECT3DDEVICE9 pDevice, _uint iVertexCountX, _uint iVertexCountZ, _float fVertexInterval = 1.f,
		const wstring& wstrHeightmapPath = L"");
	explicit CVIBuffer_TerrainTexture(const CVIBuffer_TerrainTexture& other);
	virtual ~CVIBuffer_TerrainTexture() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void * pArg = nullptr) override;
	virtual _uint Render_VIBuffer() override;

public:
	_bool Is_OnTerrain(_float3* pOutPos);

private:
	_uint* Load_Heightmap(const TCHAR* pFilePath);

public:
	static CVIBuffer_TerrainTexture* Create(LPDIRECT3DDEVICE9 pDevice,
		_uint iVertexCountX, _uint iVertexCountZ, _float fVertexInterval = 1.f,
		const wstring& wstrHeightmapPath = L"");
	virtual CComponent * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	_uint m_iVertexCountX = 0;
	_uint m_iVertexCountZ = 0;
	_float m_fVertexInterval = 1.f;
	wstring m_wstrHeightmapPath = L"";
};
END

#define __VIBUFFER_TERRAINTEXTURE_H__
#endif