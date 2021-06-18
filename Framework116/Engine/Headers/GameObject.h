#pragma once
#ifndef __GAMEOBJECT_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pDevice);
	explicit CGameObject(const CGameObject& other);
	virtual ~CGameObject() = default;

public:
	const class CComponent* Get_Component(const wstring& ComponentTag) const;

public:
	virtual HRESULT Ready_GameObject_Prototype() = 0;	/* 프로토타입 초기화 */
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) = 0; /* 클론 초기화 */
	virtual _uint Update_GameObject(_float fDeltaTime) = 0;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) = 0;
	virtual _uint Render_GameObject() = 0;

protected:
	HRESULT Add_Component(EResourceType eType, const wstring& PrototypeTag, const wstring& ComponentTag, class CComponent** ppComponent = nullptr, void* pArg = nullptr);

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() = 0;

protected:
	LPDIRECT3DDEVICE9 m_pDevice = nullptr;
	_bool m_IsClone = false;

	typedef unordered_map<wstring, class CComponent*>	COMPONENTS;
	COMPONENTS m_Components; /* 클론 보관 */

	class CManagement* m_pManagement = nullptr;
};
END

#define __GAMEOBJECT_H__
#endif