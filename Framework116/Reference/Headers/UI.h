#pragma once

#ifndef __UI_H__

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
typedef struct tagUIDesc : public BASE_DESC
{
	TRANSFORM_DESC tTransformDesc;
	wstring wstrTexturePrototypeTag = L"";
}UI_DESC;

class ENGINE_DLL CUI : public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CUI(const CUI& other);
	virtual ~CUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	const TRANSFORM_DESC Get_UI_TransformDesc();
	HRESULT Change_Texture(const wstring& wstrTexturePrototypeTag);

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pDevice);
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
};
END

#define __UI_H__
#endif