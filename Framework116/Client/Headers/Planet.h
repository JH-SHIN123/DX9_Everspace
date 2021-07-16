#pragma once
#ifndef __PLANET_H__

#include "GameObject.h"

USING(Engine)
enum class EPlanetType {
	Basic, Ice, Gas, None
};

typedef struct tagPlanetDesc : public BASE_DESC {
	wstring wstrTextureTag = L"";
	TRANSFORM_DESC tTransformDesc;
}PLANET_DESC;


class CPlanet final : public CGameObject
{
public:
	explicit CPlanet(LPDIRECT3DDEVICE9 pDevice, const EPlanetType ePlanentType);
	explicit CPlanet(const CPlanet& other);
	virtual ~CPlanet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	void Set_IsMove(_bool IsMove);

private:
	_uint Movement(_float fDeltaTime);

public:
	static CPlanet* Create(LPDIRECT3DDEVICE9 pDevice, const EPlanetType ePlanentType);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	EPlanetType m_ePlanetType = EPlanetType::None;

private:
	CModelMesh*			m_pModelMesh = nullptr;
	CTransform*			m_pTransform = nullptr;
	CTexture*			m_pTexture = nullptr;
	CCollideSphere*		m_pCollide = nullptr;

private:
	_bool m_IsMoving = false;

};

#define __PLANET_H__
#endif