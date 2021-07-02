#pragma once

#ifndef __GEO_MESH_H__

#include "Mesh.h"

BEGIN(Engine)

enum class EGeoMeshType {
	Cube = 0,
	Sphere,
	Cylinder,
	Torus
};

class ENGINE_DLL CGeoMesh abstract : public CMesh
{
protected:
	explicit CGeoMesh(LPDIRECT3DDEVICE9 pDevice);
	explicit CGeoMesh(const CGeoMesh& other);
	virtual ~CGeoMesh() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override PURE;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override PURE;
	virtual _uint	Render_Mesh() PURE;

public:
	virtual CComponent* Clone(void* pArg = nullptr) override PURE;
	virtual void Free() override PURE;

protected:
	EGeoMeshType m_eGeoType = EGeoMeshType::Cube;
};

END

#define __GEO_MESH_H__
#endif
