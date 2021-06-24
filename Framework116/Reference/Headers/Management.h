#pragma once
#ifndef __MANAGEMENT_H__

#include "Base.h"
#include "Device_Manager.h"
#include "Scene_Manager.h"
#include "Component_Manager.h"
#include "GameObject_Manager.h"
#include "Renderer.h"
#include "Time_Manager.h"

BEGIN(Engine)
class ENGINE_DLL CManagement final : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	CManagement();
	virtual ~CManagement() = default;

public: /* For.General */
	HRESULT Ready_Game(HWND hWnd, _uint iWinCX, _uint iWinCY, EDisplayMode eMode = EDisplayMode::Win);
	_uint Update_Game();
	void Clear_NonStatic_Resources();

public: /* For.Device Manager */
	LPDIRECT3DDEVICE9 Get_Device() const;

public: /* For.Time Manager */
	_float Get_DeltaTime() const;

public: /* For.Scene Manager */
	HRESULT Setup_CurrentScene(_uint iNewSceneType, class CScene* pNewScene);

public: /* For.Component Manager */
	HRESULT Add_Component_Prototype(EResourceType eType, const wstring& PrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(EResourceType eType, const wstring& PrototypeTag, void* pArg = nullptr);

public: /* For.GameObject Manager */
	const class list<class CGameObject*>* Get_GameObjectList(const wstring & LayerTag) const;
	const class CGameObject* Get_GameObject(const wstring & LayerTag, _uint iIndex = 0) const;
	const class CComponent* Get_Component(const wstring& LayerTag, const wstring& ComponentTag, _uint iIndex = 0) const;
	HRESULT Add_GameObject_Prototype(EResourceType eType, const wstring& PrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject_InLayer(EResourceType eType, const wstring& PrototypeTag, const wstring& LayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject_InLayer_Tool(EResourceType eType, const wstring & PrototypeTag, const wstring & LayerTag, const int _iListboxIndex, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(EResourceType eType, const wstring& PrototypeTag, void* pArg = nullptr);

public: /* For.Renderer */
	HRESULT Add_GameObject_InRenderer(ERenderType eType, class CGameObject* pObject);
	

public:
	virtual void Free() override;

private:
	CDevice_Manager*		m_pDevice_Manager = nullptr;
	CScene_Manager*			m_pScene_Manager = nullptr;
	CComponent_Manager*		m_pComponent_Manager = nullptr;
	CGameObject_Manager*	m_pGameObject_Manager = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTime_Manager*			m_pTime_Manager = nullptr;
};
END

#define __MANAGEMENT_H__
#endif

