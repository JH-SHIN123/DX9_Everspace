#pragma once

#ifndef __LOBBYBACKUI_H__

#include "GameObject.h"

USING(Engine)


class CLobbyBackUI final : public CGameObject
{

protected:
	explicit CLobbyBackUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CLobbyBackUI(const CLobbyBackUI& other);
	virtual ~CLobbyBackUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	const TRANSFORM_DESC Get_UI_TransformDesc();

public:
	static CLobbyBackUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

protected:
	class CVIBuffer* m_pVIBuffer = nullptr;
	class CTransform* m_pTransform = nullptr;
	class CTexture* m_pTexture = nullptr;

protected:
	wstring m_wstrTexturePrototypeTag = L"";
	TRANSFORM_DESC m_tTransformDesc;
	D3DMATERIAL9 m_tMaterial;
};

#define __LOBBYBACKUI_H__
#endif