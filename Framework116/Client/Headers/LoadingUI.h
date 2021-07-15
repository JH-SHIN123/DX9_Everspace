#pragma once

#ifndef __LOADINGUI_H__

USING(Engine)

class CLoadingUI final : public CUI
{
public:
	explicit CLoadingUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CLoadingUI(const CLoadingUI& other);
	virtual ~CLoadingUI() = default;

public:
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint	Render_GameObject() override;

public:
	static CLoadingUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;

private:
	_uint m_iIndexLoadingImage = 0;
};

#define __LOADINGUI_H__
#endif
