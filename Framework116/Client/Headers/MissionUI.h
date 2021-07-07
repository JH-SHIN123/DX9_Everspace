#pragma once
#ifndef __MISSIONUI_H__

#include "GameObject.h"

USING(Engine)
class CMissionUI final : public CGameObject
{
public:
	explicit CMissionUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CMissionUI(const CMissionUI& other);
	virtual ~CMissionUI() = default;

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
	static CMissionUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;

	// 해당 스테이지의 미션을 알려주는 UI이다.
	// 무엇이 필요 할까 

private:
	wstring m_wstrMissionName = L"";
	_uint m_iMissionCount = 0;
	_uint m_iMissionMaxCount = 0;
};

#define __MISSIONUI_H__
#endif