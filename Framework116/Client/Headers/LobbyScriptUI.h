#pragma once
#ifndef __LOBBYSCRIPTUI_H__

#include "GameObject.h"
#include "UI.h"
#include "BackUI.h"
#include"ScriptUI.h"
USING(Engine)




class CLobbyScriptUI final : public CGameObject
{
public:
	explicit CLobbyScriptUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CLobbyScriptUI(const CLobbyScriptUI& other);
	virtual ~CLobbyScriptUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	_bool Get_IsScriptEnd();
	void Set_Scene(class CLobby* pLobby) { m_pLobby = pLobby; }
private: 
	void BlackBar_Start(_float fDeltaTime);
	void BlackBar_End(_float fDeltaTime);
private: // ±‚≈∏
	HRESULT Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc);


public:
	static CLobbyScriptUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	_float m_fShowBlackBar = 3.f;
	EScriptFlow m_eScriptFlow = EScriptFlow::End;

private:
	class CVIBuffer* m_pVIBuffer = nullptr;
	class CTransform* m_pTransform = nullptr;
	class CTexture* m_pTexture = nullptr;

private:
	wstring m_wstrTexturePrototypeTag = L"";
	TRANSFORM_DESC m_tTransformDesc;

private:	
	CTransform* m_pTransfrom_BlackBar_Up = nullptr;
	CTransform* m_pTransfrom_BlackBar_Down = nullptr;

private:
	_float3 m_vUI_BlackBar_Up_Pos = { 0.f, 740.f, 0.f };
	_float3 m_vUI_BlackBar_Down_Pos = { 0.f, -740.f, 0.f };
	//Sound
	_bool m_bSoundOnce = false;

	class CLobby* m_pLobby = nullptr;

};

#define __LOBBYSCRIPTUI_H__
#endif