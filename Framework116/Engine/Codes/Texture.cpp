#include "..\Headers\Texture.h"

USING(Engine)

CTexture::CTexture(
	LPDIRECT3DDEVICE9 pDevice, 
	ETextureType eType,
	const TCHAR* pFilePath, 
	_uint iCount/* = 1*/)
	: CComponent(pDevice)
	, m_eType(eType)
	, m_pFilePath(pFilePath)
	, m_iCount(iCount)
{
}

CTexture::CTexture(const CTexture & other)
	: CComponent(other)
	, m_Textures(other.m_Textures)
	, m_eType(other.m_eType)
	, m_pFilePath(other.m_pFilePath)
	, m_iCount(other.m_iCount)
{
	for_each(m_Textures.begin(), m_Textures.end(), [](auto& pTexture) { Safe_AddRef(pTexture); });

	/*for (auto pTexture : m_Textures)
	{
		Safe_AddRef(pTexture);
	}*/
}

HRESULT CTexture::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	TCHAR szFullPath[128] = L"";
	IDirect3DBaseTexture9* pTexture = nullptr;
	HRESULT hr = 0;

	/*
	m_pFilePath = L"../Resources/Player%d.png";
	*/
	D3DCOLOR transparencycolor;
	transparencycolor = D3DCOLOR_ARGB(0xff, 0x00, 0x00, 0x00); //투명색 사용


	for (_uint i = 0; i < m_iCount; ++i)
	{
		swprintf_s(szFullPath, m_pFilePath, i);

		switch (m_eType)
		{
		case Engine::ETextureType::Normal:
			hr = D3DXCreateTextureFromFileEx(m_pDevice, szFullPath,
				D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN,
				D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
				D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
				transparencycolor,
				NULL, NULL,
				(LPDIRECT3DTEXTURE9*)&pTexture);

			break;
		case Engine::ETextureType::Cube:
			hr = D3DXCreateCubeTextureFromFile(m_pDevice, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);
			break;
		default:
			break;
		}

		if (FAILED(hr))
		{
			wstring ErrMsg = wstring(L"Failed To Load ") + szFullPath;
			PRINT_LOG(L"Error", ErrMsg.c_str());
			return E_FAIL;
		}

		m_Textures.push_back(pTexture);
	}

	return S_OK;
}

HRESULT CTexture::Ready_Component(void * pArg/* = nullptr*/)
{
	CComponent::Ready_Component(pArg);

	return S_OK;
}

_uint CTexture::Set_Texture(_uint iIndex)
{
	if (m_Textures.size() <= iIndex)
	{
		PRINT_LOG(L"Error", L"Out of range m_Textures");
		return RENDER_ERROR;
	}

	if (FAILED(m_pDevice->SetTexture(0, m_Textures[iIndex])))
	{
		PRINT_LOG(L"Error", L"Failed To SetTexture");
		return RENDER_ERROR;
	}	

	return _uint();
}

CTexture* CTexture::Create(
	LPDIRECT3DDEVICE9 pDevice, 
	ETextureType eType,
	const TCHAR* pFilePath,
	_uint iCount)
{
	CTexture* pInstance = new CTexture(pDevice, eType, pFilePath, iCount);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone(void * pArg/* = nullptr*/)
{
	CTexture* pClone = new CTexture(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Texture");
		Safe_Release(pClone);
	}

	return pClone;
}

void CTexture::Free()
{
	for_each(m_Textures.begin(), m_Textures.end(), [](auto& pTexture) { Safe_Release(pTexture); });

	CComponent::Free();
}
