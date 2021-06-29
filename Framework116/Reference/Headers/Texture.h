#pragma once
#ifndef __TEXTURE_H__

#include "Component.h"

BEGIN(Engine)
enum class ETextureType
{
	SINGLE,
	MULTI,
	Cube
};

class ENGINE_DLL CTexture final : public CComponent
{
private:
	explicit CTexture(LPDIRECT3DDEVICE9 pDevice, ETextureType eType, const TCHAR* pFilePath, _uint iCount = 1);
	explicit CTexture(const CTexture& other);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void * pArg = nullptr) override;

public:
	_uint Set_Texture(_uint iIndex = 0);

public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pDevice, ETextureType eType, const TCHAR* pFilePath, _uint iCount = 1);
	virtual CComponent * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:	
	typedef vector<IDirect3DBaseTexture9*>	TEXTURES;
	TEXTURES m_Textures;

	const TCHAR* m_pFilePath = nullptr;
	_uint m_iCount = 0;


	ETextureType m_eType = ETextureType::SINGLE;
};
END

#define __TEXTURE_H__
#endif
