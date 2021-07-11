#pragma once

#ifndef __SPRITE_TEXTURE_H__

#include "Texture.h"

BEGIN(Engine)
typedef struct tagFrame
{
	float fStartX = 0.f;
	float fBeginX = 0.f;
	float fEndX = 0.f;
	float fFrameSpeed = 0.f;
}FRAME;

typedef struct tagSpriteTextureDesc
{
	// StartX, BeginX, EndX는 자동으로 정해짐
	// EndX = 텍스쳐 개수
	_float fFrameSpeed = 0.f;
}SPRITE_TEXTURE_DESC;

class CSpriteTexture : public CTexture
{
protected:
	explicit CSpriteTexture(LPDIRECT3DDEVICE9 pDevice, ETextureType eType, const TCHAR* pFilePath, _uint iCount = 1);
	explicit CSpriteTexture(const CSpriteTexture& other);
	virtual ~CSpriteTexture() = default;

public:
	void Set_Frame(float _fStartX = 0.f);
	void Set_Frame(float _fStartX, float _fEndX);
	void Set_FrameSpeed(const float _fSpeed = 1.f);

public:
	_bool Update_SpriteTexture();
	_bool IsDone_SpriteTexture();

public:
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;
	virtual _uint Set_Texture(_uint iIndex = 0);

public:
	static CSpriteTexture* Create(LPDIRECT3DDEVICE9 pDevice, ETextureType eType, const TCHAR* pFilePath, _uint iCount = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;

private:
	FRAME m_tSpriteFrame;
};
END

#define __SPRITE_TEXTURE_H__
#endif