#include "..\Headers\VIBuffer.h"

USING(Engine)

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & other)
	: CComponent(other)
	, m_pVB(other.m_pVB)
	, m_pIB(other.m_pIB)
	, m_iVertexSize(other.m_iVertexSize)
	, m_iVertexCount(other.m_iVertexCount)
	, m_iFVF(other.m_iFVF)
	, m_iTriCount(other.m_iTriCount)
	, m_iIndexSize(other.m_iIndexSize)
	, m_IndexFmt(other.m_IndexFmt)
	, m_pVertices(other.m_pVertices)
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	m_pVertices = new _byte[m_iVertexSize * m_iVertexCount];

	if (FAILED(m_pDevice->CreateVertexBuffer(
		m_iVertexSize * m_iVertexCount, /* 배열의 메모리 총사이즈 */
		0, /* D3DUSAGE_DYNAMIC는 동적버퍼 생성, 그 외는 정적버퍼 생성 */
		m_iFVF,
		D3DPOOL_MANAGED,
		&m_pVB, 0)))
	{
		PRINT_LOG(L"Error", L"Failed To CreateVertexBuffer");
		return E_FAIL;
	}

	if (m_iIndexSize)
	{
		if (FAILED(m_pDevice->CreateIndexBuffer(
			m_iIndexSize * m_iTriCount, /* 배열의 메모리 총사이즈 */
			0, /* D3DUSAGE_DYNAMIC는 동적버퍼 생성, 그 외는 정적버퍼 생성 */
			m_IndexFmt,
			D3DPOOL_MANAGED,
			&m_pIB, 0)))
		{
			PRINT_LOG(L"Error", L"Failed To CreateIndexBuffer");
			return E_FAIL;
		}
	}	

	return S_OK;
}

HRESULT CVIBuffer::Ready_Component(void * pArg/* = nullptr*/)
{
	CComponent::Ready_Component(pArg);

	return S_OK;
}

_uint CVIBuffer::Render_VIBuffer()
{
	if (FAILED(m_pDevice->SetStreamSource(0, m_pVB, 0, m_iVertexSize)))
	{
		PRINT_LOG(L"Error", L"Failed To SetStreamSource");
		return RENDER_ERROR;
	}

	if (FAILED(m_pDevice->SetFVF(m_iFVF)))
	{
		PRINT_LOG(L"Error", L"Failed To SetFVF");
		return RENDER_ERROR;
	}

	if (m_pIB)
	{
		if (FAILED(m_pDevice->SetIndices(m_pIB)))
		{
			PRINT_LOG(L"Error", L"Failed To SetIndices");
			return RENDER_ERROR;
		}
	}	

	return NO_EVENT;
}

void CVIBuffer::Free()
{
	if (!m_IsClone) /* 클론이 아닐 때 (프로토타입일 때) */
	{
		Safe_Delete_Array(m_pVertices);
	}

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	CComponent::Free();
}
