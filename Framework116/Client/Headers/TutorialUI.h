#pragma once
#ifndef __TUTORIALUI_H__

#include "GameObject.h"

USING(Engine)
class CTutorialUI final : public CGameObject
{
public:
	explicit CTutorialUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CTutorialUI(const CTutorialUI& other);
	virtual ~CTutorialUI() = default;

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
	static CTutorialUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;

private:
	_bool m_IsLockOn = false;
	_bool m_IsMoving = false;
	_float4x4 m_Matview;

	wstring* m_pTargetLayerTag = {};
	wstring m_wstrLayerTag = {};

	list<class CGameObject*> m_listTargetObject;
	CTransform* m_pPlayerTransform = nullptr;
	_float4 m_vSearchTagetDis[20];
	_bool m_bAllTargetCollide = false;
	_float m_fSearchDisMax = 50.f;
	_float m_fSearchDisMin = 1.f;
};

#define __TUTORIALUI_H__
#endif