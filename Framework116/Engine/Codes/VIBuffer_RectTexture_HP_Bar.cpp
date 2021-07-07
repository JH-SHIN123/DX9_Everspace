#include "..\Headers\VIBuffer_RectTexture_HP_Bar.h"

USING(Engine)

CVIBuffer_RectTexture_HP_Bar::CVIBuffer_RectTexture_HP_Bar(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{
}

CVIBuffer_RectTexture_HP_Bar::CVIBuffer_RectTexture_HP_Bar(const CVIBuffer_RectTexture_HP_Bar & other)
	: CVIBuffer(other)
{
}

HRESULT CVIBuffer_RectTexture_HP_Bar::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_TEXTURE);
	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iFVF = FVF_VTX_TEXTURE;
	m_iVertexCount = 4;
	m_iTriCount = 2;

	CVIBuffer::Ready_Component_Prototype();

	VTX_TEXTURE* pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _float3(0.f, 0.05f, 0.f);
	pVertices[0].vUV = _float2(0.f, 0.f);

	pVertices[1].vPosition = _float3(1.f, 0.05f, 0.f);
	pVertices[1].vUV = _float2(1.f, 0.f);

	pVertices[2].vPosition = _float3(1.f, -0.05f, 0.f);
	pVertices[2].vUV = _float2(1.f, 1.f);

	pVertices[3].vPosition = _float3(0.f, -0.05f, 0.f);
	pVertices[3].vUV = _float2(0.f, 1.f);

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

HRESULT CVIBuffer_RectTexture_HP_Bar::Ready_Component(void * pArg)
{
	CVIBuffer::Ready_Component(pArg);

	return S_OK;
}

_uint CVIBuffer_RectTexture_HP_Bar::Render_VIBuffer()
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

CVIBuffer_RectTexture_HP_Bar * CVIBuffer_RectTexture_HP_Bar::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CVIBuffer_RectTexture_HP_Bar* pInstance = new CVIBuffer_RectTexture_HP_Bar(pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create VIBuffer TriColor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_RectTexture_HP_Bar::Clone(void * pArg)
{
	CVIBuffer_RectTexture_HP_Bar* pClone = new CVIBuffer_RectTexture_HP_Bar(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone VIBuffer TriColor");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_RectTexture_HP_Bar::Free()
{
	CVIBuffer::Free();
}
