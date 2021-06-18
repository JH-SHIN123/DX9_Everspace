#include "..\Headers\VIBuffer_TerrainColor.h"

USING(Engine)

CVIBuffer_TerrainColor::CVIBuffer_TerrainColor(
	LPDIRECT3DDEVICE9 pDevice, 
	_uint iVertexCountX, 
	_uint iVertexCountZ, 
	_float fVertexInterval/* = 1.f*/)
	: CVIBuffer(pDevice)
	, m_iVertexCountX(iVertexCountX)
	, m_iVertexCountZ(iVertexCountZ)
	, m_fVertexInterval(fVertexInterval)
{
}

CVIBuffer_TerrainColor::CVIBuffer_TerrainColor(const CVIBuffer_TerrainColor & other)
	: CVIBuffer(other)
	, m_iVertexCountX(other.m_iVertexCountX)
	, m_iVertexCountZ(other.m_iVertexCountZ)
	, m_fVertexInterval(other.m_fVertexInterval)
{
}

HRESULT CVIBuffer_TerrainColor::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_COLOR);
	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iFVF = FVF_VTX_COLOR;
	m_iVertexCount = m_iVertexCountX * m_iVertexCountZ;
	m_iTriCount = (m_iVertexCountX - 1) * (m_iVertexCountZ - 1) * 2;

	CVIBuffer::Ready_Component_Prototype();

	VTX_COLOR* pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	_uint iIndex = 0;
	for (_uint z = 0; z < m_iVertexCountZ; ++z)
	{
		for (_uint x = 0; x < m_iVertexCountX; ++x)
		{
			iIndex = z * m_iVertexCountX + x;
			if(m_iVertexCount <= iIndex)
				continue;

			pVertices[iIndex].vPosition = _float3(x * m_fVertexInterval, 0.f, z * m_fVertexInterval);
			pVertices[iIndex].iColor = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}	

	m_pVB->Unlock();

	INDEX16* pIndices = nullptr;
	m_pIB->Lock(0, 0, (void**)&pIndices, 0);
	
	_uint iTriIndex = 0;
	iIndex = 0;
	for (_uint z = 0; z < m_iVertexCountZ - 1; ++z)
	{
		for (_uint x = 0; x < m_iVertexCountX - 1; ++x)
		{
			iIndex = z * m_iVertexCountX + x;
			if (m_iVertexCount <= iIndex)
				continue;

			pIndices[iTriIndex]._1 = iIndex + m_iVertexCountX;
			pIndices[iTriIndex]._2 = iIndex + m_iVertexCountX + 1;
			pIndices[iTriIndex++]._3 = iIndex + 1;

			pIndices[iTriIndex]._1 = iIndex + m_iVertexCountX;
			pIndices[iTriIndex]._2 = iIndex + 1;
			pIndices[iTriIndex++]._3 = iIndex;
		}
	}

	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_TerrainColor::Ready_Component(void * pArg)
{
	CVIBuffer::Ready_Component(pArg);

	return S_OK;
}

_uint CVIBuffer_TerrainColor::Render_VIBuffer()
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

CVIBuffer_TerrainColor * CVIBuffer_TerrainColor::Create(
	LPDIRECT3DDEVICE9 pDevice,
	_uint iVertexCountX,
	_uint iVertexCountZ,
	_float fVertexInterval/* = 1.f*/)
{
	CVIBuffer_TerrainColor* pInstance = new CVIBuffer_TerrainColor(pDevice, iVertexCountX, iVertexCountZ, fVertexInterval);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create VIBuffer TriColor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TerrainColor::Clone(void * pArg)
{
	CVIBuffer_TerrainColor* pClone = new CVIBuffer_TerrainColor(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone VIBuffer TriColor");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_TerrainColor::Free()
{
	CVIBuffer::Free();
}
