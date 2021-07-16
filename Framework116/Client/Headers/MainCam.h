#pragma once
#ifndef __MAINCAM_H__

#include "Camera.h"
#define CAMERA_SKIP -1
USING(Engine)

enum class ESoloMoveMode // Lock 보다 작아야 스킵 문구가 뜸
{
	Stage1_Ring,
	Stage2_Asteroid,
	Stage2_FinishAsteroid,
	Stage3_Delivery,
	Stage3_Boss,
	Lock,
	OutLock,
	End
};

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
	void Set_Transform(CTransform* pTransform) { m_pPlayerTransform = pTransform; }
	// 1인칭 카메라 1차 Test 실패 ㅋㅋ
	_uint FPSMovement(_float fDeltaTime);
	_uint SetCameraFPS(_float fDeltaTime);

	// 카메라 연출
	void Set_IsSoloMove(ESoloMoveMode eMove);
	const ESoloMoveMode Get_SoloMoveMode();

	// 카메라 쉐이킹
	_uint CameraShakingStart(_float fDeltaTime, _float Range);

private:
	_uint Movement(_float fDeltaTime);
	_uint KeyInput(_float fDeltaTime);

private: // 카메라 연출
	void Check_SoloMoveMode(_float fDeltaTime);
	_uint Solo_Lock(_float fDeltaTime);
	_uint Solo_OutLock(_float fDeltaTime);
	_uint Solo_Stage1_Ring(_float fDeletaTime);
	_uint Solo_Stage2_Asteroid(_float fDeltaTime);
	_uint Solo_Stage2FinishAsteroid(_float fDeltaTime);
	_uint Solo_Stage3_Dilevery(_float fDeltaTime);
	_uint Solo_Stage3_Boss(_float fDeltaTime);

public:
	static CMainCam* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform* m_pPlayerTransform = nullptr;
	_float m_fDistanceFromTarget = 20.f;
	_float m_fCamAngle = 15.f;
	vector<PASSDATA_ROUTE> m_vecNaviRoute;
private:
	D3DXQUATERNION m_qCameraRot;

private:
	_bool m_IsFPS = false;

private: // 카메라 연출 End일 때 플레이어를 따라감
	ESoloMoveMode m_eSoloMoveMode = ESoloMoveMode::End;
	CTransform* m_pTargetTransform = nullptr;
	BYTE m_byMoveCount = 0;
	_bool m_IsMoveCountCheck = false;
	_float m_fCameraMoveTime = 0.f;
	_float m_fCameraMoveAngle = 0.f;
	_float3 m_vCameraMovePos;
	//_bool 
};

#define __MAINCAM_H__
#endif