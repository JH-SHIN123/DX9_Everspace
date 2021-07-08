#pragma once
#ifndef __VIBUFFER_RECTTEXTURE_HP_BAR_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_RectTexture_HP_Bar final : public CVIBuffer
{
private:
	explicit CVIBuffer_RectTexture_HP_Bar(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer_RectTexture_HP_Bar(const CVIBuffer_RectTexture_HP_Bar& other);
	virtual ~CVIBuffer_RectTexture_HP_Bar() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void * pArg = nullptr) override;
	virtual _uint Render_VIBuffer() override;

public:
	static CVIBuffer_RectTexture_HP_Bar* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
};
END

#define __VIBUFFER_RECTTEXTURE_HP_BAR_H__
#endif