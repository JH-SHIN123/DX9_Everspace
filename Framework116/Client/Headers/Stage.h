#pragma once
#ifndef __STAGE_H__

#include "Scene.h"

USING(Engine)
class CStage final : public CScene
{
public:
	explicit CStage(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CStage() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

private:
	HRESULT Add_Layer_Player(const wstring& LayerTag);
	HRESULT Add_Layer_Terrain(const wstring& LayerTag);
	HRESULT Add_Layer_Cam(const wstring& LayerTag);
	HRESULT Add_Layer_Monster(const wstring& LayerTag);
	HRESULT Add_Layer_Grass(const wstring& LayerTag);
	HRESULT Add_Layer_Skybox(const wstring& LayerTag);
	HRESULT Add_Layer_UI(const wstring& LayerTag, const TRANSFORM_DESC& tTransformDesc, const wstring& wstrTexturePrototypeTag);
	HRESULT Add_Layer_DirectionalLight(const wstring& LayerTag, const _float3 vDir, const D3DXCOLOR tColor);

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;
};

#define __STAGE_H__
#endif
