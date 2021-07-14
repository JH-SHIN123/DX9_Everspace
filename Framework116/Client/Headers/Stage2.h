#pragma once

#ifndef __STAGE2_H__
#include "Scene.h"

USING(Engine)

class CStage2 final : public CScene
{
public:
	explicit CStage2(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CStage2() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

public:
	HRESULT Add_Layer_Cam(const wstring& LayerTag);
	HRESULT Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc);
	HRESULT Add_Layer_Skybox(const wstring& LayerTag);
	HRESULT Add_Layer_HUD(const wstring& LayerTag);
	HRESULT Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc);

	HRESULT Add_Layer_Monster(const wstring& LayerTag);
	HRESULT Add_Layer_Sniper(const wstring& LayerTag);


	// Stage3 생기기전까지만 사용하겠습니당~~

public:
	static CStage2* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;

};

#define __STAGE2_H__
#endif
