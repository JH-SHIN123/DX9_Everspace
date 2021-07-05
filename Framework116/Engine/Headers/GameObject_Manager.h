#pragma once
#ifndef __GAMEOBJECT_MANAGER_H__

#include "Base.h"
#include "UI.h"
#include "Light.h"
#include "ParticleSystem.h"

BEGIN(Engine)
class CGameObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CGameObject_Manager)

private:
	CGameObject_Manager();
	virtual ~CGameObject_Manager() = default;

public:
	const class list<class CGameObject*>* Get_GameObjectList(const wstring& LayerTag) const;
	const class CGameObject* Get_GameObject(const wstring& LayerTag, _uint iIndex = 0) const;
	const class CComponent* Get_Component(const wstring& LayerTag, const wstring& ComponentTag, _uint iIndex = 0) const;

public:
	HRESULT Add_GameObject_Prototype(EResourceType eType, const TCHAR* PrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject_InLayer(EResourceType eType, const wstring& PrototypeTag, const wstring& LayerTag, void* pArg = nullptr, class CGameObject** ppGameObject = nullptr);

	HRESULT Add_GameObject_InLayer_Tool(EResourceType eType, const wstring& PrototypeTag, const wstring& LayerTag, const int _iListboxIndex, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(EResourceType eType, const wstring& PrototypeTag, void* pArg = nullptr);
	void Clear_NonStatic_Resources();
	_uint Update_GameObject(_float fDeltaTime);
	_uint LateUpdate_GameObject(_float fDeltaTime);

public:
	virtual void Free() override;

private:
	typedef unordered_map<wstring, class CGameObject*> GAMEOBJECTS;
	GAMEOBJECTS m_GameObjects[(_uint)EResourceType::End]; /* 프로토타입 보관 */

	typedef unordered_map<wstring, class CLayer*> LAYERS;
	LAYERS m_Layers; /* layer */
};
END

#define __GAMEOBJECT_MANAGER_H__
#endif
