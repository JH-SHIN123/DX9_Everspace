#pragma once
#ifndef __NAVI_ARROW_H__

#include "GameObject.h"

USING(Engine)

typedef struct tagNaviArrowDesc : public BASE_DESC {
	_float3 vPos = { 0.f,0.f,0.f };
	_float3 vDir = { 0.f,0.f,0.f };
}NAVI_ARROW_DESC;

class CNaviArrow final : public CGameObject
{
public:
	explicit CNaviArrow(LPDIRECT3DDEVICE9 pDevice);
	explicit CNaviArrow(const CNaviArrow& other);
	virtual ~CNaviArrow() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CNaviArrow* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform*		m_pTransform = nullptr;
	CMesh*			m_pMesh = nullptr;
	D3DMATERIAL9	m_tMaterial;

};

#define __NAVI_ARROW_H__ 
#endif