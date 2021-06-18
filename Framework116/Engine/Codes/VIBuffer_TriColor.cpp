#include "..\Headers\VIBuffer_TriColor.h"

USING(Engine)

CVIBuffer_TriColor::CVIBuffer_TriColor(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{
}

CVIBuffer_TriColor::CVIBuffer_TriColor(const CVIBuffer_TriColor & other)
	: CVIBuffer(other)
{
}

HRESULT CVIBuffer_TriColor::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iFVF = FVF_VTX_COLOR;
	m_iVertexCount = 3;
	m_iTriCount = 1;

	CVIBuffer::Ready_Component_Prototype();

	VTX_COLOR* pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _float3(0.f, 1.f, 0.f);
	pVertices[0].iColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[1].vPosition = _float3(1.f, -1.f, 0.f);
	pVertices[1].iColor = D3DCOLOR_ARGB(255, 0, 255, 0);

	pVertices[2].vPosition = _float3(-1.f, -1.f, 0.f);
	pVertices[2].iColor = D3DCOLOR_ARGB(255, 0, 0, 255);

	m_pVB->Unlock();


	return S_OK;
}

HRESULT CVIBuffer_TriColor::Ready_Component(void * pArg)
{
	CVIBuffer::Ready_Component(pArg);

	return S_OK;
}

_uint CVIBuffer_TriColor::Render_VIBuffer()
{
	if (CVIBuffer::Render_VIBuffer())
		return RENDER_ERROR;

	if (FAILED(m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_iTriCount)))
	{
		PRINT_LOG(L"Error", L"Failed To DrawPrimitive");
		return RENDER_ERROR;
	}

	return NO_EVENT;
}

CVIBuffer_TriColor * CVIBuffer_TriColor::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CVIBuffer_TriColor* pInstance = new CVIBuffer_TriColor(pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create VIBuffer TriColor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TriColor::Clone(void * pArg)
{
	CVIBuffer_TriColor* pClone = new CVIBuffer_TriColor(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone VIBuffer TriColor");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_TriColor::Free()
{
	CVIBuffer::Free();
}
