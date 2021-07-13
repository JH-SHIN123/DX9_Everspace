#pragma once
#ifndef __SCENE_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CScene abstract : public CBase
{
public:
	explicit CScene(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CScene() = default;

public:
	void Set_ChangeScene() { m_bChangeScene = true; }

public:
	virtual HRESULT Ready_Scene() = 0;
	virtual _uint Update_Scene(_float fDeltaTime) = 0;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) = 0;

public:
	virtual void Free()  = 0;

protected:
	LPDIRECT3DDEVICE9	m_pDevice = nullptr;
	class CManagement* m_pManagement = nullptr;

protected:
	_bool m_bChangeScene = false;
};
END

#define __SCENE_H__
#endif