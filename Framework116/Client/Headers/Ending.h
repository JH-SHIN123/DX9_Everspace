#pragma once

#ifndef __ENDING_H__

#include "Scene.h"

USING(Engine)
class CEnding final : public CScene
{
private:
	explicit CEnding(LPDIRECT3DDEVICE9 pDevice, const ESceneType eSceneType);
	virtual ~CEnding() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;

public:
	static CEnding* Create(LPDIRECT3DDEVICE9 pDevice, const ESceneType eSceneType);
	virtual void Free() override;

private:
	ESceneType m_eSceneType = ESceneType::None;
	_bool	m_bFadeIn = false;

private:
	_bool m_bStart = true;

private:
	TCHAR* m_pAudioFileName = L"";
};


#define __ENDING_H__
#endif
