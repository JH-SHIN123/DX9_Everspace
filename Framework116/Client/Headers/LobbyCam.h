#pragma once
#ifndef __LOBBYCAM_H__

#include "Camera.h"

USING(Engine)
class CLobbyCam final : public CCamera
{
public:
	explicit CLobbyCam(LPDIRECT3DDEVICE9 pDevice);
	explicit CLobbyCam(const CLobbyCam& other);
	virtual ~CLobbyCam() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	void Set_DistanceFromTarget(_float fDist) { m_fDistanceFromTarget = fDist; }
	void Set_CamAngle(_float fRadian) { m_fCamAngle = fRadian; }

	void Set_GotoNextScene(_bool bSet) { m_bGotoNextScene = bSet; }
	void Set_Scene(class CLobby* pLobby) { m_pLobby = pLobby; }
private:
	_uint OffSet(_float fDeltaTime);
	void StartChangeScene(_float fDeltaTime);
public:
	static CLobbyCam* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform* m_pPlayerTransform = nullptr;
	_float m_fDistanceFromTarget = 10.f;
	_float m_fCamAngle = D3DXToRadian(45.f);

	_bool m_bGotoNextScene = false;
	class CLobby* m_pLobby = nullptr;

	_float m_fSceneChangeDeltaTime = 0.f;
};

#define __LOBBYCAM_H__
#endif