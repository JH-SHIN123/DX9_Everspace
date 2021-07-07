#pragma once
#ifndef __SCRIPTUI_H__

#include "GameObject.h"

USING(Engine)

enum Script {
	Tutorial
	, End
};

class CScruptUI final : public CGameObject
{
public:
	explicit CScruptUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CScruptUI(const CScruptUI& other);
	virtual ~CScruptUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint Search_Target(_float fDeltaTime);
	_uint BillBorad(_float fDeltaTime);

public:
	static CScruptUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;

private:
	Script m_eScriptMode = End;

};

#define __SCRIPTUI_H__
#endif