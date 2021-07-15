#pragma once

#ifndef __LOADINGICON_H__

USING(Engine)

class CLoadingIcon final : public CUI
{
public:
	explicit CLoadingIcon(LPDIRECT3DDEVICE9 pDevice);
	explicit CLoadingIcon(const CLoadingIcon& other);
	virtual ~CLoadingIcon() = default;

public:
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint	Update_GameObject(_float fDeltaTime) override;
	virtual _uint	Render_GameObject() override;

public:
	static CLoadingIcon* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;

};

#define __LOADINGICON_H__
#endif
