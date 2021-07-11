#pragma once
#ifndef __GATCHABOX_H__

#include "GameObject.h"

USING(Engine)
class CGatchaBox final : public CGameObject
{
public:
	explicit CGatchaBox(LPDIRECT3DDEVICE9 pDevice);
	explicit CGatchaBox(const CGatchaBox& other);
	virtual ~CGatchaBox() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_bool CheckPicking();
	_bool StartUnPacking(_float fDeltaTime);
	void Add_Layer_Product(wstring& wstrLayerTag);
public:
	static CGatchaBox* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;
public:
	void Set_Scene(class CLobby* _pLobby) { m_pLobby = _pLobby; }
	void Set_StartUnPacking(_bool bSet) { m_bStartUnpacking = bSet; }
	void Set_CancelUnPacking(_bool bSet) { m_bCancelUnPacking = bSet; }
	_bool Get_StartUnpacking() { return m_bStartUnpacking; }
private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
	CCollideSphere* m_pCollide = nullptr;

	class CLobby* m_pLobby = nullptr;
	_bool m_bStartUnpacking = false;
	_float m_fUnPackingTime = 0.f;
	_bool m_bBomb = false;
	_bool m_bCancelUnPacking = false;

	
};

#define __GATCHABOX_H__
#endif