#pragma once

#ifndef __COLLIDE_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollide final : public CComponent
{
private:
	explicit CCollide(LPDIRECT3DDEVICE9 pDevice, const LPD3DXMESH pMesh);
	explicit CCollide(LPDIRECT3DDEVICE9 pDevice, const class CVIBuffer* pMesh);
	explicit CCollide(const CCollide& other);
	virtual ~CCollide() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;
	virtual _uint	Render_Collide();

public:
	static CCollide* Create(LPDIRECT3DDEVICE9 pDevice, const LPD3DXMESH pMesh);
	static CCollide* Create(LPDIRECT3DDEVICE9 pDevice, const class CVIBuffer* pMesh);

	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	BOUNDING_SPHERE m_tBoundingSphere;
	LPD3DXMESH m_pSphere; // µð¹ö±ë¿ë
};

#define __COLLIDE_H__
#endif
