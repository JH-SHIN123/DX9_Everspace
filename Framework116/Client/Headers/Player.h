#pragma once
#ifndef __PLAYER_H__

#include "GameObject.h"

USING(Engine)
class CPlayer final : public CGameObject
{
public:
	explicit CPlayer(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pPassData);
	explicit CPlayer(const CPlayer& other);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	void	KeyProcess(_float fDeltaTime);
	_uint	Movement(_float fDeltaTime);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pPassData);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

	// For.Components
private:
	CMesh*  m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CCollideSphere*	m_pCollide = nullptr;
	CController* m_pController = nullptr;

	//CCamera* m_pCam = nullptr;
	//_float m_fCameraDist = 5.f;

	// 플레이어가 사용하실 변수님들.
private:
	_float m_fMachinegunDelay = 0.f;
	_bool m_IsLeft = false;


};

#define __PLAYER_H__
#endif