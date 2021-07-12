#pragma once
#ifndef __STATUSBOARD_H__

#include "GameObject.h"

USING(Engine)
class CStatusBoard final : public CGameObject
{
public:
	explicit CStatusBoard(LPDIRECT3DDEVICE9 pDevice);
	explicit CStatusBoard(const CStatusBoard& other);
	virtual ~CStatusBoard() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	void Render_AxisMean();

public:
	static CStatusBoard* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
public:
	void Set_Scene(class CLobby* pLobby) { m_pLobby = pLobby; }
private:
	class CLobby* m_pLobby = nullptr;
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;

private:
	CMyFont* m_pFont_Atk = nullptr;
};

#define __STATUSBOARD_H__
#endif