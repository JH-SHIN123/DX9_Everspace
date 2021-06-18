#pragma once
#ifndef __VIBUFFER_RECTCOLOR_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_RectColor final : public CVIBuffer
{
private:
	explicit CVIBuffer_RectColor(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer_RectColor(const CVIBuffer_RectColor& other);
	virtual ~CVIBuffer_RectColor() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void * pArg = nullptr) override;
	virtual _uint Render_VIBuffer() override;

public:
	static CVIBuffer_RectColor* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
};
END

#define __VIBUFFER_RECTCOLOR_H__
#endif