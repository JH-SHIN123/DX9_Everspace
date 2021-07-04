#pragma once

#ifndef __COLLIDESPHERE_H__

#include "Collide.h"

BEGIN(Engine)
class ENGINE_DLL CCollideSphere final : public CCollide
{
private:
	explicit CCollideSphere(LPDIRECT3DDEVICE9 pDevice);
	explicit CCollideSphere(const CCollideSphere& other);
	virtual ~CCollideSphere() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;

	virtual _uint	Update_Collide(const _float4x4& matParent) override;
	virtual _uint	Render_Collide() override;

public:
	_uint Resize_Shpere(const _float& fRadius);

public:
	static CCollideSphere* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	// 바운딩박스 여러개 설치할수 있도록 변경
	LPD3DXMESH		m_pSphere; // 디버깅용
	D3DMATERIAL9	m_tMaterial;
};
END

#define __COLLIDESPHERE_H__
#endif