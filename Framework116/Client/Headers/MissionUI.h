#pragma once
#ifndef __MISSIONUI_H__

#include "GameObject.h"

USING(Engine)
class CMissionUI final : public CUI
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
	_uint Key_Input();

public:
	static CMissionUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private: 
	// 퀘스트의 조건이나 완료 유무 자체는 스트림이 관리하고 여기서는 출력 할 값만 따지자
	wstring m_wstrMissionName = L"";
	_uint m_iMissionCount = 0;
	_uint m_iMissionMaxCount = 0;
	_bool m_IsClear = false;

private:
	CTransform* m_pTransform_Name = nullptr;
	CTransform* m_pTransform_Deco = nullptr;

	_float	m_fPoint = 735.f;
};

#define __MISSIONUI_H__
#endif