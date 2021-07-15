#pragma once

#ifndef __BACKUI_H__

#include "GameObject.h"

USING(Engine)

enum class EPortraitNumber
{
	Admiral = 0,
	Player,
	Friendly,
	Delivery,
	Politician,
	End
};
class CBackUI final : public CGameObject
{

protected:
	explicit CBackUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CBackUI(const CBackUI& other);
	virtual ~CBackUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	const TRANSFORM_DESC Get_UI_TransformDesc();
	HRESULT Change_Texture(const wstring& wstrTexturePrototypeTag);
	HRESULT Change_TextureNumber(const EPortraitNumber& ePortraitNumber);

public:
	static CBackUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

protected:
	class CVIBuffer* m_pVIBuffer = nullptr;
	class CTransform* m_pTransform = nullptr;
	class CTexture* m_pTexture = nullptr;

protected:
	wstring m_wstrTexturePrototypeTag = L"";
	TRANSFORM_DESC m_tTransformDesc;
	RECT m_tUIBounds;
	EPortraitNumber m_ePortraitNumber = EPortraitNumber::End;
	D3DMATERIAL9 m_tMaterial;
};

#define __BACKUI_H__
#endif