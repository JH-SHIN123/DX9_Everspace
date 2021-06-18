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
	const class CComponent* Get_Component(const wstring& ComponentTag, _uint iIndex = 0) const;

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
