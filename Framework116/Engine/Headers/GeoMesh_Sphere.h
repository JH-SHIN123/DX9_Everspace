#pragma once

#ifndef __GEO_MESH_SPHERE_H__

#include "GeoMesh.h"

BEGIN(Engine)

class ENGINE_DLL CGeoMesh_Sphere final : public CGeoMesh
{
private:
	explicit CGeoMesh_Sphere(LPDIRECT3DDEVICE9 pDevice);
	explicit CGeoMesh_Sphere(const CGeoMesh_Sphere& other);
	virtual ~CGeoMesh_Sphere() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;
	virtual _uint	Render_Mesh() override;

public:
	static CGeoMesh_Sphere* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

#define __GEO_MESH_SPHERE_H__
#endif