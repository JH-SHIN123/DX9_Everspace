#pragma once

#ifndef __DELIVERY_H__

#include "GameObject.h"

USING(Engine)

class CDelivery final : public CGameObject
{
public:
	explicit CDelivery(LPDIRECT3DDEVICE9 pDevice);
	explicit CDelivery(const CDelivery& other);
	virtual ~CDelivery() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	void FindNextRoute();

public:
	static CDelivery* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool m_bArrive = false;

private:
	CTransform* m_pTransform = nullptr;
	CMesh* m_pMesh = nullptr;

private:
	vector<PASSDATA_ROUTE> m_vecNaviRoute;
	_uint m_iCurRouteIndex = 0;
};


#define __DELIVERY_H__ 
#endif