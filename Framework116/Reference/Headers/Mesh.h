#pragma once
#ifndef __MESH_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMesh abstract : public CComponent
{
protected:
	explicit CMesh(LPDIRECT3DDEVICE9 pDevice);
	explicit CMesh(const CMesh& other);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override PURE;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override PURE;
	virtual _uint	Render_Mesh() PURE;

public:
	virtual CComponent* Clone(void* pArg = nullptr) override PURE;
	virtual void Free() override PURE;

protected:
	LPD3DXMESH m_pMesh = nullptr;
};
END

#define __MESH_H__
#endif
