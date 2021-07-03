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
	HRESULT	Load_Model(wstring wstrObjectPrototypePath);
public:
	static CLobbyModel* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CMesh*  m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CCollideSphere*	m_pCollide = nullptr;
	CController* m_pController = nullptr;
	class CPlayer* m_pPlayer;
};

#define __LOBBYMODEL_H__
#endif