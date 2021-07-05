#pragma once
#include "Scene.h"

USING(Engine)
#ifndef __LOBBY_H__


class CLobby final : public CScene
{
public:
	explicit CLobby(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLobby() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;
	
	HRESULT Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc);
	HRESULT Add_Layer_Lobby_Model(const wstring& LayerTag);
	HRESULT Add_Layer_LobbyCam(const wstring& LayerTag);
	HRESULT Add_Layer_Skybox(const wstring & LayerTag);
	HRESULT Add_Layer_UI(const wstring & LayerTag);
	HRESULT Add_Layer_GatchaBox(const wstring & LayerTag);

	void Set_GotoNextScene(_bool bSet);
	void Set_IsGatcha(_bool bSet);


	_bool Get_IsGatcha();
public:
	static CLobby* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;

private:

	class CPlayer* m_pPlayer;

	_bool m_bGotoNextScene = false;
	_bool m_bIsGatcha = false;
};

#define __LOBBY_H__
#endif


