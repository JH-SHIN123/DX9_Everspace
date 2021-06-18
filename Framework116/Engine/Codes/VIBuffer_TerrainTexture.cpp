#include "..\Headers\VIBuffer_TerrainTexture.h"

USING(Engine)

CVIBuffer_TerrainTexture::CVIBuffer_TerrainTexture(
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

CVIBuffer_TerrainTexture::CVIBuffer_TerrainTexture(const CVIBuffer_TerrainTexture & other)
	: CVIBuffer(other)
	, m_iVertexCountX(other.m_iVertexCountX)
	, m_iVertexCountZ(other.m_iVertexCountZ)
	, m_fVertexInterval(other.m_fVertexInterval)
{
}

HRESULT CVIBuffer_TerrainTexture::Ready_Component_Prototype()
{
	m_iVertexSize = sizeof(VTX_TEXTURE);
	m_iIndexSize = sizeof(INDEX16);
	m_IndexFmt = D3DFMT_INDEX16;
	m_iFVF = FVF_VTX_TEXTURE;
	m_iVertexCount = m_iVertexCountX * m_iVertexCountZ;
	m_iTriCount = (m_iVertexCountX - 1) * (m_iVertexCountZ - 1) * 2;

	CVIBuffer::Ready_Component_Prototype();

	_uint* pPixels = Load_Heightmap(L"../Resources/Terrain/Height.bmp");
	if (nullptr == pPixels)
	{
		PRINT_LOG(L"Error", L"Failed To Load_Heightmap");
		return E_FAIL;
	}

	VTX_TEXTURE* pVertices = nullptr;
	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	_uint iIndex = 0;
	for (_uint z = 0; z < m_iVertexCountZ; ++z)
	{
		for (_uint x = 0; x < m_iVertexCountX; ++x)
		{
			iIndex = z * m_iVertexCountX + x;
			if(m_iVertexCount <= iIndex)
				continue;

			//_uint iHeight = pPixels[iIndex] & D3DCOLOR_ARGB(0, 0, 0, 255);
			_uint iHeight = pPixels[iIndex] & 0x000000ff;
			pVertices[iIndex].vPosition = _float3(x * m_fVertexInterval, iHeight / 100.f, z * m_fVertexInterval);
			pVertices[iIndex].vUV = _float2(x / (m_iVertexCountX - 1.f), 1.f - z / (m_iVertexCountZ - 1.f));
		}
	}	

	memcpy(m_pVertices, pVertices, m_iVertexSize * m_iVertexCount);

	Safe_Delete_Array(pPixels);
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

HRESULT CVIBuffer_TerrainTexture::Ready_Component(void * pArg)
{
	CVIBuffer::Ready_Component(pArg);

	return S_OK;
}

_uint CVIBuffer_TerrainTexture::Render_VIBuffer()
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

_bool CVIBuffer_TerrainTexture::Is_OnTerrain(_float3* pOutPos)
{
	if (0.f > pOutPos->x ||
		0.f > pOutPos->z ||
		m_iVertexCountX * m_fVertexInterval <= pOutPos->x ||
		m_iVertexCountZ * m_fVertexInterval <= pOutPos->z)
		return false;

	_uint iCol = _uint(pOutPos->x / m_fVertexInterval);
	_uint iRow = _uint(pOutPos->z / m_fVertexInterval);

	_uint iIndex = iRow * m_iVertexCountX + iCol;
	if (m_iVertexCount <= iIndex)
		return false;

	D3DXPLANE plane;
	VTX_TEXTURE* pVertices = (VTX_TEXTURE*)m_pVertices;
	if (nullptr == pVertices)
		return false;

	_float fRatioX = fabs(pOutPos->x - pVertices[iIndex + m_iVertexCountX].vPosition.x);
	_float fRatioZ = fabs(pOutPos->z - pVertices[iIndex + m_iVertexCountX].vPosition.z);

	if (fRatioZ < fRatioX) /* 오른쪽 삼각형 */
	{
		D3DXPlaneFromPoints(
			&plane,
			&pVertices[iIndex + m_iVertexCountX].vPosition,
			&pVertices[iIndex + m_iVertexCountX + 1].vPosition,
			&pVertices[iIndex + 1].vPosition);
	}
	else /* 왼쪽 삼각형 */
	{
		D3DXPlaneFromPoints(
			&plane,
			&pVertices[iIndex + m_iVertexCountX].vPosition,
			&pVertices[iIndex + 1].vPosition,
			&pVertices[iIndex].vPosition);
	}

	/*
	평면의 방정식
	ax + by + cz + d = 0
	y = -(ax + cz + d) / b
	*/

	pOutPos->y = -(plane.a * pOutPos->x + plane.c * pOutPos->z + plane.d) / plane.b;
	return true;
}

_uint * CVIBuffer_TerrainTexture::Load_Heightmap(const TCHAR * pFilePath)
{
	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		PRINT_LOG(L"Error", L"Failed To Open File");
		return nullptr;
	}
		
	DWORD dwBytes = 0;

	BITMAPFILEHEADER bmpFileHeader;
	ZeroMemory(&bmpFileHeader, sizeof(BITMAPFILEHEADER));

	BITMAPINFOHEADER bmpInfoHeader;
	ZeroMemory(&bmpInfoHeader, sizeof(BITMAPINFOHEADER));

	ReadFile(hFile, &bmpFileHeader, sizeof(BITMAPFILEHEADER), &dwBytes, 0);
	ReadFile(hFile, &bmpInfoHeader, sizeof(BITMAPINFOHEADER), &dwBytes, 0);

	_uint iPixelCount = bmpInfoHeader.biWidth * bmpInfoHeader.biHeight;
	_uint* pPixels = new _uint[iPixelCount];
	if (!ReadFile(hFile, pPixels, sizeof(_uint) * iPixelCount, &dwBytes, 0))
	{
		PRINT_LOG(L"Error", L"Failed To Read pPixels");
		Safe_Delete_Array(pPixels);
		CloseHandle(hFile);
		return nullptr;
	}

	CloseHandle(hFile);
	return pPixels;
}

CVIBuffer_TerrainTexture * CVIBuffer_TerrainTexture::Create(
	LPDIRECT3DDEVICE9 pDevice,
	_uint iVertexCountX,
	_uint iVertexCountZ,
	_float fVertexInterval/* = 1.f*/)
{
	CVIBuffer_TerrainTexture* pInstance = new CVIBuffer_TerrainTexture(pDevice, iVertexCountX, iVertexCountZ, fVertexInterval);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create VIBuffer TriColor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CVIBuffer_TerrainTexture::Clone(void * pArg)
{
	CVIBuffer_TerrainTexture* pClone = new CVIBuffer_TerrainTexture(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone VIBuffer TriColor");
		Safe_Release(pClone);
	}

	return pClone;
}

void CVIBuffer_TerrainTexture::Free()
{
	CVIBuffer::Free();
}
