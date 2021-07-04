#pragma once

#ifndef __COLLIDE_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollide abstract : public CComponent
{
protected:
	explicit CCollide(LPDIRECT3DDEVICE9 pDevice);
	explicit CCollide(const CCollide& other);
	virtual ~CCollide() = default;

public:
	const ECollideType Get_CollideType() const { return m_eCollideType; }
	const BOUNDING_SPHERE& Get_BoundingSphere() const { return m_tBoundingSphere; }

public:
	virtual HRESULT Ready_Component_Prototype() override PURE;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override PURE;

	virtual _uint	Update_Collide(const _float4x4& matParent) PURE;
	virtual _uint	Render_Collide() PURE;

public:
	virtual CComponent* Clone(void* pArg = nullptr) override PURE;
	virtual void Free() override PURE;

protected:
	ECollideType m_eCollideType = ECollideType::End;
	BOUNDING_SPHERE m_tBoundingSphere;
	//BOUNDING_BOX
};
END

#define __COLLIDE_H__
#endif