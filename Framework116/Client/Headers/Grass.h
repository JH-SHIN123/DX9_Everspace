#pragma once
#ifndef __GRASS_H__

#include "GameObject.h"

USING(Engine)
class CGrass final : public CGameObject
{
public:
	explicit CGrass(LPDIRECT3DDEVICE9 pDevice);
	explicit CGrass(const CGrass& other);
	virtual ~CGrass() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint IsBillboarding();

public:
	static CGrass* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
	CVIBuffer_TerrainTexture* m_pTerrainBuffer = nullptr;
};

#define __GRASS_H__
#endif