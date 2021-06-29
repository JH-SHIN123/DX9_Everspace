#include "SpriteTexture.h"
#include "Management.h"

USING(Engine)

CSpriteTexture::CSpriteTexture(LPDIRECT3DDEVICE9 pDevice, ETextureType eType, const TCHAR* pFilePath, _uint iCount)
	: CTexture(pDevice, eType, pFilePath, iCount)
{
}

CSpriteTexture::CSpriteTexture(const CSpriteTexture& other)
	: CTexture(other)
{
}

void CSpriteTexture::Set_Frame(float _fStartX)
{
	m_tSpriteFrame.fStartX = _fStartX;
}

void CSpriteTexture::Set_Frame(float _fStartX, float _fEndX)
{
	m_tSpriteFrame.fStartX = _fStartX;
	m_tSpriteFrame.fEndX = _fEndX;
}

void CSpriteTexture::Set_FrameSpeed(const float _fSpeed)
{
	m_tSpriteFrame.fFrameSpeed = _fSpeed;
}

_bool CSpriteTexture::Update_SpriteTexture()
{
	m_tSpriteFrame.fStartX += m_tSpriteFrame.fEndX * CManagement::Get_Instance()->Get_DeltaTime() * m_tSpriteFrame.fFrameSpeed;

	if (m_tSpriteFrame.fStartX >= m_tSpriteFrame.fEndX)
	{
		m_tSpriteFrame.fStartX = 0.f;
		return true;
	}
	return false;
}

_bool CSpriteTexture::IsDone_SpriteTexture()
{
	if (m_tSpriteFrame.fStartX >= m_tSpriteFrame.fEndX)
		return true;
	return false;
}

HRESULT CSpriteTexture::Ready_Component(void* pArg)
{
	CTexture::Ready_Component(pArg);

	SPRITE_TEXTURE_DESC* pSpriteTexDesc = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (pSpriteTexDesc = dynamic_cast<SPRITE_TEXTURE_DESC*>(ptr))
		{
			m_tSpriteFrame.fFrameSpeed = pSpriteTexDesc->fFrameSpeed;
		}
	}

	m_tSpriteFrame.fBeginX = 0.f;
	m_tSpriteFrame.fStartX = 0.f;
	m_tSpriteFrame.fEndX = (_float)m_Textures.size();

	return S_OK;
}

_uint CSpriteTexture::Set_Texture(_uint iIndex)
{
	return CTexture::Set_Texture((int)m_tSpriteFrame.fStartX);
}

CSpriteTexture* CSpriteTexture::Create(LPDIRECT3DDEVICE9 pDevice, ETextureType eType, const TCHAR* pFilePath, _uint iCount)
{
	CSpriteTexture* pInstance = new CSpriteTexture(pDevice, eType, pFilePath, iCount);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CSpriteTexture::Clone(void* pArg)
{
	CSpriteTexture* pClone = new CSpriteTexture(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Texture");
		Safe_Release(pClone);
	}

	return pClone;
}