#pragma once
#ifndef __LAYER_H__

#include "Base.h"

BEGIN(Engine)
class CLayer final : public CBase
{
protected:
	CLayer();
	virtual ~CLayer() = default;

public:
	const class CGameObject* Get_GameObject(_uint iIndex = 0) const;
	const class CComponent* Get_Component(const wstring& ComponentTag, _uint iIndex = 0) const;
	const class list<class CGameObject*>* Get_GameObjectList() const { return &m_GameObjects; }

public:
	HRESULT Add_GameObject_InLayer(class CGameObject* pObject);
	_uint Update_Layer(_float fDeltaTime);
	_uint LateUpdate_Layer(_float fDeltaTime);
	
public:
	static CLayer* Create();
	virtual void Free() override;

private:
	typedef list<class CGameObject*>	GAMEOBJECTS;
	GAMEOBJECTS	m_GameObjects;	/* 클론 보관 */


};
END

#define __LAYER_H__
#endif
