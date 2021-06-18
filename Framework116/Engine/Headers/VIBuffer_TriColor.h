#pragma once
#ifndef __VIBUFFER_TRICOLOR_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_TriColor final : public CVIBuffer
{
private:
	explicit CVIBuffer_TriColor(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer_TriColor(const CVIBuffer_TriColor& other);
	virtual ~CVIBuffer_TriColor() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void * pArg = nullptr) override;
	virtual _uint Render_VIBuffer() override;

public:
	static CVIBuffer_TriColor* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
};
END

#define __VIBUFFER_TRICOLOR_H__
#endif