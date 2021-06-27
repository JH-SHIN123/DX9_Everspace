#pragma once
#ifndef __RENDERER_H__

#include "Base.h"

BEGIN(Engine)
enum class ERenderType
{
	Priority,
	NonAlpha,
	Alpha,
	Particle,
	UI,
	End
};

class CRenderer : public CBase
{
	DECLARE_SINGLETON(CRenderer)

private:
	CRenderer() = default;
	virtual ~CRenderer() = default;

public:
	HRESULT Add_GameObject(ERenderType eType, class CGameObject* pObject);
	_uint Render_GameObject();

private:
	_uint Render_Priority();
	_uint Render_NonAlpha();
	_uint Render_Alpha();
	_uint Render_Particle();
	_uint Render_UI();

public:
	virtual void Free() override;	

private:
	typedef list<class CGameObject*>	GAMEOBJECTS;
	GAMEOBJECTS m_GameObjects[(_uint)ERenderType::End];
};
END

#define __RENDERER_H__
#endif
