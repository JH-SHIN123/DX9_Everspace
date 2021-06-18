#pragma once

#include "Base.h"

USING(Engine)
class CMainApp final : public CBase
{
private:
	CMainApp();
	~CMainApp() = default;

public:
	HRESULT Ready_MainApp();
	_uint Update_MainApp();

private:
	HRESULT Ready_StaticResources();
	HRESULT Setup_DefaultSetting();

public:
	static CMainApp* Create();
	virtual void Free() override;

private:
	CManagement*		m_pManagement = nullptr;
	LPDIRECT3DDEVICE9	m_pDevice = nullptr;
};

