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
	HRESULT Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc);
	HRESULT Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc);
	HRESULT Add_Layer_ExplosionSystem(const wstring& LayerTag, const PARTICLESYSTEM_DESC* pParticleSystemDesc);
	HRESULT Add_Layer_LaserSystem(const wstring& LayerTag, const PARTICLESYSTEM_DESC* pParticleSystemDesc);

public:
	static CStage* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;
};

#define __STAGE_H__
#endif
