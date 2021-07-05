#pragma once
#include "UI.h"
class CLobbyUI final :
	public CUI
{
private:
	explicit CLobbyUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CLobbyUI(const CLobbyUI& other);
	virtual ~CLobbyUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;


private:
	void Update_Bounds();
	void Check_Picking();
	void Key_Check(_float fDeltaTime);
	void Set_Text();
public:
	void Set_GotoNextScene(_bool bSet) { m_bGotoNextScene = bSet; }

public:
	static CLobbyUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
public:
	void Set_Scene(class CLobby* _pUI);

private:
	_float m_fDelayCheck = 0;
	_float m_fDeltaTime = 0;
	DWORD m_dwIdx = 0;
	class CLobby* m_pLobby;
	_bool m_bGotoNextScene = false;
	_bool m_bDead = false;
	_bool m_bShowModelIcon = false;

};

