#pragma once

#ifndef __FONT_H__
#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

typedef struct tagMyFontDesc : public BASE_DESC
{
	TRANSFORM_DESC tTransformDesc;
	wstring wstrText = L"";
	D3DXCOLOR tTextColor = D3DXCOLOR(255, 255, 255, 255);
}MYFONT_DESC;


class ENGINE_DLL CMyFont : public CGameObject
{
protected:
	explicit CMyFont(LPDIRECT3DDEVICE9 pDevice);
	explicit CMyFont(const CMyFont& other);
	virtual ~CMyFont() = default;

public:
	void Set_Text(const wstring& wstrText);
	void Set_TextColor(D3DXCOLOR tTextColor);
	void Set_Position(const _float3& vPos);
	void Set_Scale(const _float3& vScale);

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CMyFont* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

protected:
	class CTransform* m_pTransform = nullptr;
	wstring		m_wstrText = L"";
	D3DXCOLOR	m_tTextColor = D3DXCOLOR(255, 255, 255, 255);
};

END

#define __FONT_H__
#endif