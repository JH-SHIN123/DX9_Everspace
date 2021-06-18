#include "..\Headers\VIBuffer_RectColor.h"

USING(Engine)

CVIBuffer_RectColor::CVIBuffer_RectColor(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{
}

CVIBuffer_RectColor::CVIBuffer_RectColor(const CVIBuffer_RectColor & other)
	: CVIBuffer(other)
{
}

HRESULT CVIBuffer_RectColor::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iFVF = FVF_VTX_COLOR;
	m_iVertexCount = 4;
	m_iTriCount = 2;

	CVIBuffer::Ready_Component_Prototype();

	VTX_COLOR* pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	pVertices[0].iColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].iColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].iColor = D3DCOLOR_ARGB(255, 0, 0, 255);

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].iColor = D3DCOLOR_ARGB(255, 255, 0, 0);	

	m_pVB->Unlock();

	INDEX16* pIndices = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._1 = 0;
	pIndices[0]._2 = 1;
	pIndices[0]._3 = 2;

	pIndices[1]._1 = 0;
	pIndices[1]._2 = 2;
	pIndices[1]._3 = 3;

	m_pIB->Unlock();


	return S_OK;
}

HRESULT CVIBuffer_RectColor::Ready_Component(void * pArg)
{
	CVIBuffer::Ready_Component(pArg);

	return S_OK;
}

_uint CVIBuffer_RectColor::Render_VIBuffer()
{
	if (CVIBuffer::Render_VIBuffer())
		return RENDER_ERROR;	

	if (FAILED(m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexCount, 0, m_iTriCount)))
	{
		PRINT_LOG(L"Error", L"Failed To DrawIndexedPrimitive");
		return RENDER_ERROR;
	}

	return NO_EVENT;
}

CVIBuffer_RectColor * CVIBuffer_RectColor::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CVIBuffer_RectColor* pInstance = new CVIBuffer_RectColor(pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create VIBuffer TriColor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectColor::Clone(void * pArg)
{
	CVIBuffer_RectColor* pClone = new CVIBuffer_RectColor(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone VIBuffer TriColor");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_RectColor::Free()
{
	CVIBuffer::Free();
}
