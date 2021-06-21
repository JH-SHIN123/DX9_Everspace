#pragma once

#ifndef __MAINSCENE_H__

#include "Scene.h"

USING(Engine)
class CMainScene final : public CScene
{
public:
	explicit CMainScene(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CMainScene() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

private:
	HRESULT Add_Layer_Player(const wstring & LayerTag);

public:
	static CMainScene* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;
};

#define __MAINSCENE_H__
#endif

