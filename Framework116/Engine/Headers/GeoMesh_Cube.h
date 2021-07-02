#pragma once

#ifndef __GEO_MESH_CUBE_H__

#include "GeoMesh.h"

BEGIN(Engine)

class ENGINE_DLL CGeoMesh_Cube final  : public CGeoMesh
{
private:
	explicit CGeoMesh_Cube(LPDIRECT3DDEVICE9 pDevice);
	explicit CGeoMesh_Cube(const CGeoMesh_Cube& other);
	virtual ~CGeoMesh_Cube() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;
	virtual _uint	Render_Mesh() override;

public:
	static CGeoMesh_Cube* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};
END

#define __GEO_MESH_CUBE_H__
#endif