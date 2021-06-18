#pragma once
#ifndef __VIBUFFER_CUBETEXTURE_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_CubeTexture final : public CVIBuffer
{
private:
	explicit CVIBuffer_CubeTexture(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer_CubeTexture(const CVIBuffer_CubeTexture& other);
	virtual ~CVIBuffer_CubeTexture() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void * pArg = nullptr) override;
	virtual _uint Render_VIBuffer() override;

public:
	static CVIBuffer_CubeTexture* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
};
END

#define __VIBUFFER_CUBETEXTURE_H__
#endif