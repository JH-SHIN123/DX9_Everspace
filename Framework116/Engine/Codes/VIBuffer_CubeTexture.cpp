#include "..\Headers\VIBuffer_CubeTexture.h"

USING(Engine)

CVIBuffer_CubeTexture::CVIBuffer_CubeTexture(LPDIRECT3DDEVICE9 pDevice)
	: CVIBuffer(pDevice)
{
}

CVIBuffer_CubeTexture::CVIBuffer_CubeTexture(const CVIBuffer_CubeTexture & other)
	: CVIBuffer(other)
{
}

HRESULT CVIBuffer_CubeTexture::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_CUBETEXTURE);
	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iFVF = FVF_VTX_CUBETEXTURE;
	m_iVertexCount = 8;
	m_iTriCount = 12;

	CVIBuffer::Ready_Component_Prototype();

	VTX_CUBETEXTURE* pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vUV = pVertices[0].vPosition;

	pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vUV = pVertices[1].vPosition;

	pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vUV = pVertices[2].vPosition;

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vUV = pVertices[3].vPosition;

	pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vUV = pVertices[4].vPosition;

	pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vUV = pVertices[5].vPosition;

	pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vUV = pVertices[6].vPosition;

	pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vUV = pVertices[7].vPosition;

	m_pVB->Unlock();

	INDEX16* pIndices = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	// +x
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 5;
	pIndices[0]._3 = 6;

	pIndices[1]._1 = 1;
	pIndices[1]._2 = 6;
	pIndices[1]._3 = 2;

	// -x
	pIndices[2]._1 = 4;
	pIndices[2]._2 = 0;
	pIndices[2]._3 = 3;

	pIndices[3]._1 = 4;
	pIndices[3]._2 = 3;
	pIndices[3]._3 = 7;

	// +y
	pIndices[4]._1 = 4;
	pIndices[4]._2 = 5;
	pIndices[4]._3 = 1;

	pIndices[5]._1 = 4;
	pIndices[5]._2 = 1;
	pIndices[5]._3 = 0;

	// -y
	pIndices[6]._1 = 3;
	pIndices[6]._2 = 2;
	pIndices[6]._3 = 6;

	pIndices[7]._1 = 3;
	pIndices[7]._2 = 6;
	pIndices[7]._3 = 7;

	// +z
	pIndices[8]._1 = 5;
	pIndices[8]._2 = 4;
	pIndices[8]._3 = 7;

	pIndices[9]._1 = 5;
	pIndices[9]._2 = 7;
	pIndices[9]._3 = 6;

	// -z
	pIndices[10]._1 = 0;
	pIndices[10]._2 = 1;
	pIndices[10]._3 = 2;

	pIndices[11]._1 = 0;
	pIndices[11]._2 = 2;
	pIndices[11]._3 = 3;

	m_pIB->Unlock();


	return S_OK;
}

HRESULT CVIBuffer_CubeTexture::Ready_Component(void * pArg)
{
	CVIBuffer::Ready_Component(pArg);

	return S_OK;
}

_uint CVIBuffer_CubeTexture::Render_VIBuffer()
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

CVIBuffer_CubeTexture * CVIBuffer_CubeTexture::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CVIBuffer_CubeTexture* pInstance = new CVIBuffer_CubeTexture(pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create VIBuffer TriColor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_CubeTexture::Clone(void * pArg)
{
	CVIBuffer_CubeTexture* pClone = new CVIBuffer_CubeTexture(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone VIBuffer TriColor");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_CubeTexture::Free()
{
	CVIBuffer::Free();
}
