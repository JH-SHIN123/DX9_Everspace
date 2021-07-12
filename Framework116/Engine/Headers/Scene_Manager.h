#pragma once
#ifndef __SCENE_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
class CScene_Manager final : public CBase
{
	DECLARE_SINGLETON(CScene_Manager)

public:
	CScene_Manager();
	virtual ~CScene_Manager() = default;

public:
	HRESULT Setup_CurrentScene(_uint iNewSceneType, class CScene* pNewScene);
	_uint Update_Scene(_float fDeltaTime);
	_uint LateUpdate_Scene(_float fDeltaTime);

public:
	virtual void Free() override;

private:
	class CScene*	m_pCurrentScene = nullptr;
	_uint			m_iCurrentSceneType = 4294967295;

};
END

#define __SCENE_MANAGER_H__
#endif