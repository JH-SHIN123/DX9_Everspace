#pragma once
#ifndef __MAINCAM_H__

#include "Camera.h"

USING(Engine)
class CMainCam final : public CCamera
{
public:
	explicit CMainCam(LPDIRECT3DDEVICE9 pDevice);
	explicit CMainCam(const CMainCam& other);
	virtual ~CMainCam() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	void Set_DistanceFromTarget(_float fDist) { m_fDistanceFromTarget = fDist; }
	void Set_CamAngle(_float fRadian) { m_fCamAngle = fRadian; }

	// 1인칭 카메라 1차 Test 실패 ㅋㅋ
	_uint FPSMovement(_float fDeltaTime);
	_uint SetCameraFPS(_float fDeltaTime);

	// 카메라 쉐이킹
	_uint CameraShakingStart(_float fDeltaTime);

private:
	_uint Movement(_float fDeltaTime);
	_uint KeyInput(_float fDeltaTime);

public:
	static CMainCam* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform* m_pPlayerTransform = nullptr;
	_float m_fDistanceFromTarget = 20.f;
	_float m_fCamAngle = 15.f;
	_float m_fMouseSensitive = 0.05f;

private:
	D3DXQUATERNION m_qCameraRot;

private:
	_bool m_IsFPS = false;
};

#define __MAINCAM_H__
#endif