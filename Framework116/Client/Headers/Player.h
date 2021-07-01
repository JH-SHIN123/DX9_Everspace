#pragma once
#ifndef __PLAYER_H__

#include "GameObject.h"

USING(Engine)
class CPlayer final : public CGameObject
{
public:
	explicit CPlayer(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pPassData);
	explicit CPlayer(const CPlayer& other);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pPassData);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CMesh*  m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CCollideSphere*	m_pCollide = nullptr;
	CController* m_pController = nullptr;

private:
	POINT m_tCurCursorPos = { 0,0 };
	POINT m_tPrevCursorPos = { 0,0 };

};

#define __PLAYER_H__
#endif