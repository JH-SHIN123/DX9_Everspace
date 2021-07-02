#pragma once

#ifndef __GEO_MESH_CYLINDER_H__

#include "GeoMesh.h"

BEGIN(Engine)

class ENGINE_DLL CGeoMesh_Cylinder final  : public CGeoMesh
{
private:
	explicit CGeoMesh_Cylinder(LPDIRECT3DDEVICE9 pDevice, FLOAT Radius1, FLOAT Radius2, FLOAT Length);
	explicit CGeoMesh_Cylinder(const CGeoMesh_Cylinder& other);
	virtual ~CGeoMesh_Cylinder() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;
	virtual _uint	Render_Mesh() override;

public:
	static CGeoMesh_Cylinder* Create(LPDIRECT3DDEVICE9 pDevice, FLOAT Radius1, FLOAT Radius2, FLOAT Length);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	FLOAT m_fRadius1 = 0.f;
	FLOAT m_fRadius2 = 0.f;
	FLOAT m_fLength = 0.f;
};

END

#define __GEO_MESH_CYLINDER_H__
#endif