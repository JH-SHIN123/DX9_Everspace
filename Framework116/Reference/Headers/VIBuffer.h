#pragma once
#ifndef __VIBUFFER_H__

#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pDevice);
	explicit CVIBuffer(const CVIBuffer& other);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Ready_Component_Prototype() = 0;
	virtual HRESULT Ready_Component(void * pArg = nullptr) = 0;
	virtual _uint Render_VIBuffer() = 0;

public:
	virtual CComponent * Clone(void * pArg = nullptr) = 0;
	virtual void Free() = 0;

protected:
	LPDIRECT3DVERTEXBUFFER9	m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9	m_pIB = nullptr;
	_uint					m_iVertexSize = 0;
	_uint					m_iVertexCount = 0;
	_uint					m_iFVF = 0;	// FVF (Flexible Vertex Format)
	_uint					m_iTriCount = 0;
	_uint					m_iIndexSize = 0;
	D3DFORMAT				m_IndexFmt = D3DFMT_INDEX16;

	void*	m_pVertices = nullptr;
};
END

#define __VIBUFFER_H__
#endif