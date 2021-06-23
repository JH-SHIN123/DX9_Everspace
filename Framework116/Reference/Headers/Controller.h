#pragma once
#ifndef __CONTROLLER_H__

#include "Component.h"

BEGIN(Engine)

#define KEY_LEFT		0x00000001
#define KEY_RIGHT		0x00000002
#define KEY_UP			0x00000004
#define KEY_DOWN		0x00000008
#define KEY_LBUTTON		0x00000010

class ENGINE_DLL CController final : public CComponent
{
private:
	explicit CController(LPDIRECT3DDEVICE9 pDevice);
	explicit CController(const CController& other);
	virtual ~CController() = default;

public:
	void Update_Controller();

	bool Key_Up(DWORD dwKey);
	bool Key_Down(DWORD dwKey);
	bool Key_Pressing(DWORD dwKey);

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;

public:
	static CController* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	DWORD m_dwKey = 0;
	DWORD m_dwKeyDOWN = 0;
	DWORD m_dwKeyUP = 0;
};
END

#define __CONTROLLER_H__
#endif