#pragma once
#ifndef __BULLET_LASER_H__

#include "GameObject.h"

USING(Engine)
class CBullet_Laser final : public CGameObject
{
public:
	explicit CBullet_Laser(LPDIRECT3DDEVICE9 pDevice);
	explicit CBullet_Laser(const CBullet_Laser& other);
	virtual ~CBullet_Laser() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	_uint BillBoard();

public:
	static CBullet_Laser* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer_RectTexture*  m_pRectTexure		= nullptr;
	CTransform*				m_pTransform		= nullptr;
	CTexture*				m_pTexture			= nullptr;
	CCollideSphere*			m_pCollide			= nullptr;
	CTransform*				m_pTargetTransform	= nullptr;
	CStatus_Info*			m_pInfo				= nullptr;
	class CGameObject*		m_pEffect			= nullptr;

	_bool m_IsTracking = false;
	_float m_fLiveTime = 5.f;
	_float3 m_vMoveDir = {};
	D3DMATERIAL9 m_tMaterial;



};

#define __BULLET_LASER_H__
#endif