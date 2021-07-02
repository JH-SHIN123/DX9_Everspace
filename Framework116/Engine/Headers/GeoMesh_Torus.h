#pragma once

#ifndef __GEO_MESH_TORUS_H__

#include "GeoMesh.h"

BEGIN(Engine)

class ENGINE_DLL CGeoMesh_Torus final : public CGeoMesh
{
private:
	explicit CGeoMesh_Torus(LPDIRECT3DDEVICE9 pDevice, FLOAT InnerRadius, FLOAT OuterRadius);
	explicit CGeoMesh_Torus(const CGeoMesh_Torus& other);
	virtual ~CGeoMesh_Torus() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;
	virtual _uint	Render_Mesh() override;

public:
	static CGeoMesh_Torus* Create(LPDIRECT3DDEVICE9 pDevice, FLOAT InnerRadius, FLOAT OuterRadius);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	FLOAT m_fInnerRadius = 0.f;
	FLOAT m_fOuterRadius = 0.f;
};

END

#define __GEO_MESH_TORUS_H__
#endif