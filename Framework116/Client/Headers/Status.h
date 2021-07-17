#pragma once
#ifndef __STATUS_H__

#include "GameObject.h"
#include"Lobby.h"
USING(Engine)
class CStatus :
	public CGameObject
{
public:
	explicit CStatus(LPDIRECT3DDEVICE9 pDevice);
	explicit CStatus(const CStatus& other);
	virtual ~CStatus() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_bool UpdateHexagon(_float fDeltaTime);
public:
	static CStatus* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
public:
	void Set_Scene(class CLobby* pLobby) { m_pLobby = pLobby; }
private:
	class CLobby* m_pLobby = nullptr;
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;

	STAT_INFO*	m_pUnitInfo = nullptr;
	STAT_INFO	m_tPreUnitInfo;

};

#define __STATUS_H__
#endif // !__STATUS_H__
