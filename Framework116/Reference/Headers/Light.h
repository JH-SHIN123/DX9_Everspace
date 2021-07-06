#pragma once

#ifndef __LIGHT_H__

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

enum class ELightType {
	Directional,
	PointLight,
	SpotLight,
	End
};

typedef struct tagLightDesc : public BASE_DESC
{
	TRANSFORM_DESC tTransformDesc;
	ELightType eLightType = ELightType::End;
	_float3 vLightDir = { 1.0f, -0.0f, 0.25f }; // Default : 의미없음.
	D3DXCOLOR tLightColor = D3DCOLOR_XRGB(255, 255, 255);
}LIGHT_DESC;

class ENGINE_DLL CLight : public CGameObject
{
	static int s_iLightIndex;

protected:
	explicit CLight(LPDIRECT3DDEVICE9 pDevice);
	explicit CLight(const CLight& other);
	virtual ~CLight() = default;

protected:
	void InitDirectionalLight(_float3* direction, D3DXCOLOR* color);
	void InitPointLight(_float3* position, D3DXCOLOR* color);
	void InitSpotLight(_float3* position, _float3* direction, D3DXCOLOR* color);

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CLight* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

protected:
	class CTransform* m_pTransform = nullptr;
	D3DLIGHT9 m_tLight;
	ELightType m_eLightType = ELightType::End;
};
END

#define __LIGHT_H__
#endif