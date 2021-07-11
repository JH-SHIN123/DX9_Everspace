#pragma once
#ifndef __LOBBYMODEL_H__

#include "GameObject.h"

USING(Engine)
class CLobbyModel final : public CGameObject
{
public:
	explicit CLobbyModel(LPDIRECT3DDEVICE9 pDevice);
	explicit CLobbyModel(const CLobbyModel& other);
	virtual ~CLobbyModel() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	void	KeyProcess(_float fDeltaTime);
	_uint	Movement(_float fDeltaTime);
	void	StartSceneChange(_float fDeltaTime);
	void	Add_Layer_CancelButton();
public:
	static CLobbyModel* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
public:
	void Set_GotoNextScene(_bool _bSet) { m_bGotoNextScene = _bSet; }
	void Set_Scene(class CLobby* _pLobby) { m_pLobby = _pLobby; }
private:
	CMesh*  m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CController* m_pController = nullptr;
	

private:
	_bool m_bGotoNextScene = false;
	_bool m_bGo_Straight = false;
	_float m_fDelaySceneChange = 0.f;

	class CLobby* m_pLobby = nullptr;
	_bool m_bSetCreateCancelButton = false;

	//Sound
	_float m_fSoundTiming = 0.f;

};

#define __LOBBYMODEL_H__
#endif