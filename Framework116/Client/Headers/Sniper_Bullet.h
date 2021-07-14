#pragma once
#ifndef __SNIPER_BULLET_H__

#include "GameObject.h"

USING(Engine)
class CSniper_Bullet final : public CGameObject
{
public:
	explicit CSniper_Bullet(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData);
	explicit CSniper_Bullet(const CSniper_Bullet& other);
	virtual ~CSniper_Bullet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint Fire_Triger(_float fDeltaTime);
	_uint BillBoard();

public:
	static CSniper_Bullet* Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pData = nullptr);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CGeoMesh_Sphere*  m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;
	CCollideSphere* m_pCollide = nullptr;

	CTransform* m_pTargetTransform = nullptr;
	CStatus_Info* m_pInfo = nullptr;
	//CTransform* m_pParentTransform = nullptr;

	_float m_fTrackingTime = 1.f;
	_bool m_IsTracking = false;
	_float m_fLiveTime = 2.f;

private:
	_float3 m_vEffectOffset = { 0.f, 0.f, 0.f };
	class CGameObject* m_pEffect = nullptr;


};

#define __SNIPER_BULLET_H__
#endif