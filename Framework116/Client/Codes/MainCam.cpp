#include "stdafx.h"
#include "..\Headers\MainCam.h"
#include "Pipeline.h"
#include "Player.h"

CMainCam::CMainCam(LPDIRECT3DDEVICE9 pDevice)
	: CCamera(pDevice)
{
	D3DXQuaternionIdentity(&m_qCameraRot);
}

CMainCam::CMainCam(const CMainCam & other)
	: CCamera(other)
	, m_fDistanceFromTarget(other.m_fDistanceFromTarget)
	, m_fCamAngle(other.m_fCamAngle)
	, m_qCameraRot(other.m_qCameraRot)
{
}

HRESULT CMainCam::Ready_GameObject_Prototype()
{
	CCamera::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CMainCam::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CCamera::Ready_GameObject(pArg);

	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);
	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMainCam::Update_GameObject(_float fDeltaTime)
{
	CCamera::Update_GameObject(fDeltaTime);

	if (m_eSoloMoveMode == ESoloMoveMode::End)
	{
		if (!m_IsFPS)
		{
			Movement(fDeltaTime);
			KeyInput(fDeltaTime);
		}
		else
		{
			SetCameraFPS(fDeltaTime);
			KeyInput(fDeltaTime);
		}
	}

	else
	{
		if (GetAsyncKeyState('C') & 0x8000)
		{
			m_byMoveCount = CAMERA_SKIP;
		}

		Check_SoloMoveMode(fDeltaTime);
	}

	return NO_EVENT;
}

_uint CMainCam::LateUpdate_GameObject(_float fDeltaTime)
{
	CCamera::LateUpdate_GameObject(fDeltaTime);

	// 카메라에도 스피어 달아야하나??





	return _uint();
}

_uint CMainCam::Render_GameObject()
{
	CCamera::Render_GameObject();

	return _uint();
}


_uint CMainCam::Movement(_float fDeltaTime)
{
	_float3 vCameraDir = { 0.f,0.f, -1.f };

	// 0.05f 민감도
	D3DXQuaternionSlerp(&m_qCameraRot, &m_qCameraRot, &m_pPlayerTransform->Get_TransformDesc().qRot, 0.05f);

	_float4x4 matCameraRot;
	D3DXMatrixRotationQuaternion(&matCameraRot, &m_qCameraRot);

	// Camera Pos
	_float4x4 matInitCameraRot;
	D3DXMatrixRotationX(&matInitCameraRot, D3DXToRadian(m_fCamAngle));

	D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matInitCameraRot);
	D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matCameraRot);
	_float3 vCameraPos = m_pPlayerTransform->Get_State(EState::Position) + vCameraDir * m_fDistanceFromTarget;

	// Up
	_float3 vCameraUp = { 0.f,1.f,0.f };
	D3DXVec3TransformNormal(&vCameraUp, &vCameraUp, &matCameraRot);

	// LookAt
	_float3 vCameraLookAt = _float3(0.f, 10.f, 0.f);
	memcpy(&matCameraRot._41, &m_pPlayerTransform->Get_State(EState::Position), sizeof(_float3));
	D3DXVec3TransformCoord(&vCameraLookAt, &vCameraLookAt, &matCameraRot);

	m_CameraDesc.vEye = vCameraPos;
	m_CameraDesc.vUp = vCameraUp;
	m_CameraDesc.vAt = vCameraLookAt;

	return _uint();
}

_uint CMainCam::KeyInput(_float fDeltaTime)
{
	if (!m_IsFPS)
	{
		if (GetAsyncKeyState(VK_ADD) & 0x8000)
		{
			m_fDistanceFromTarget -= 10.f * fDeltaTime;
		}
		if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
		{
			m_fDistanceFromTarget += 10.f * fDeltaTime;
		}
		if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000)
		{
			m_fCamAngle += D3DXToRadian(90.f) * fDeltaTime;
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
		{
			m_fCamAngle += D3DXToRadian(90.f) * -fDeltaTime;
		}

		// 일반이동 카메라 연출
		if (GetAsyncKeyState(L'W') & 0x8000)
		{
			if (m_fDistanceFromTarget < 22.f)
				m_fDistanceFromTarget += 18.f * fDeltaTime;
		}

		// 부스터 카메라 연출
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			_float OriginDis = 20.f;

			if (!(m_fDistanceFromTarget > OriginDis + 4.f))
				m_fDistanceFromTarget += 8.f * fDeltaTime;
		}
		if (!(GetAsyncKeyState(VK_SPACE) & 0x8000))
		{
			_float OriginDis = 20.f;
			if (!(m_fDistanceFromTarget < 20.f))
				m_fDistanceFromTarget -= 5.f * fDeltaTime;
		}
	}

	// 시점 변경 
	if (GetAsyncKeyState(L'V') & 0x8000)
	{
		if (!m_IsFPS)
			m_IsFPS = true;
	}
	if (GetAsyncKeyState(L'B') & 0x8000)
	{
		if (m_IsFPS)
			m_IsFPS = false;
	}
	
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		_int iWeapon = ((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Get_Weapon_Type();
		_bool IsFire = ((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Get_Is_Fire();

		if (IsFire)
		{
			switch (iWeapon)
			{
			case WEAPON_LAZER:

				CameraShakingStart(fDeltaTime, 5.f);
				break;
			case WEAPON_MACHINEGUN:
				CameraShakingStart(fDeltaTime, 4.f);
				break;
			case WEAPON_MISSILE:
				CameraShakingStart(fDeltaTime, 9.f);
				break;
			}
		}
	}


	return _uint();
}

void CMainCam::Check_SoloMoveMode(_float fDeltaTime)
{
	switch (m_eSoloMoveMode)
	{
	case ESoloMoveMode::Stage1_Ring:
		Solo_Stage1_Ring(fDeltaTime);
		break;
	case ESoloMoveMode::Lock:
		Solo_Lock(fDeltaTime);
		break;
	case ESoloMoveMode::Stage2_Asteroid:
		Solo_Stage2_Asteroid(fDeltaTime);
		break;
	case ESoloMoveMode::Stage2_FinishAsteroid:
		Solo_Stage2FinishAsteroid(fDeltaTime);
		break;
	default:
		m_eSoloMoveMode = ESoloMoveMode::End;
		break;
	}
}

_uint CMainCam::Solo_Lock(_float fDeltaTime)
{
	_float3 vCameraDir = { 0.f,0.f, -1.f };
	D3DXQuaternionSlerp(&m_qCameraRot, &m_qCameraRot, &m_pPlayerTransform->Get_TransformDesc().qRot, 0.05f);
	_float4x4 matCameraRot;
	D3DXMatrixRotationQuaternion(&matCameraRot, &m_qCameraRot);
	_float4x4 matInitCameraRot;
	D3DXMatrixRotationX(&matInitCameraRot, D3DXToRadian(m_fCamAngle));
	D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matInitCameraRot);
	D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matCameraRot);

	// 내가 있어야 할 위치
	_float3 vCameraPos = m_pPlayerTransform->Get_State(EState::Position) + vCameraDir * 30.f;
	_float3 vNowPos = m_CameraDesc.vEye;
	_float3 vDir = vCameraPos - vNowPos;

	if (D3DXVec3Length(&vDir) >= 1.f)
	{
		D3DXVec3Normalize(&vDir, &vDir);
		_float fSpeedPerSec = 10.f;

		vNowPos += vDir * fSpeedPerSec * fDeltaTime;

		m_CameraDesc.vEye = vNowPos;	
	}



	return _uint();
}

_uint CMainCam::Solo_Stage1_Ring(_float fDeletaTime)
{
	if (m_IsMoveCountCheck == false)
	{
		m_byMoveCount = 0;
		m_IsMoveCountCheck = true;

		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Planet", L"Com_Transform");
		Safe_AddRef(m_pTargetTransform);
		if (m_pTargetTransform == nullptr)
		{
			PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
			return UPDATE_ERROR;
		}
	}

	_float fSpeedPerSec = 120.f;
	// eye, up, at

	switch (m_byMoveCount)
	{
	case 0: // at을 플레이어 > 행성
	{
		_float3 vCameraDir = { 0.f,0.f, -1.f };

		// 0.05f 민감도
		D3DXQuaternionSlerp(&m_qCameraRot, &m_qCameraRot, &m_pPlayerTransform->Get_TransformDesc().qRot, 0.05f);

		_float4x4 matCameraRot;
		D3DXMatrixRotationQuaternion(&matCameraRot, &m_qCameraRot);

		// Camera Pos
		_float4x4 matInitCameraRot;
		D3DXMatrixRotationX(&matInitCameraRot, D3DXToRadian(m_fCamAngle));

		D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matInitCameraRot);
		D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matCameraRot);
		_float3 vCameraPos = m_pPlayerTransform->Get_State(EState::Position) + vCameraDir * m_fDistanceFromTarget;

		// Up
		_float3 vCameraUp = { 0.f,1.f,0.f };
		D3DXVec3TransformNormal(&vCameraUp, &vCameraUp, &matCameraRot);

		// LookAt
		_float3 vCameraLookAt = _float3(0.f, 10.f, 0.f);
		memcpy(&matCameraRot._41, &m_pTargetTransform->Get_State(EState::Position), sizeof(_float3));
		D3DXVec3TransformCoord(&vCameraLookAt, &vCameraLookAt, &matCameraRot);

		vCameraPos.x += 20.f;
		vCameraPos.y += 20.f;
		vCameraPos.z += 20.f;

		_float3 vDir = (m_pTargetTransform->Get_State(EState::Position) - m_pPlayerTransform->Get_State(EState::Position));
		_float3 vLen = (m_pPlayerTransform->Get_State(EState::Position) - m_CameraDesc.vEye);

		//if (D3DXVec3Length(&vLen) <= 10.f)
		//{
			m_CameraDesc.vEye = vCameraPos;
			m_CameraDesc.vUp = vCameraUp;
			m_CameraDesc.vAt = vCameraLookAt;
			++m_byMoveCount;
		//}

		D3DXVec3Normalize(&vDir, &vDir);
		//_float fSpeedPerSec = 50.f;
		m_CameraDesc.vEye += fSpeedPerSec * fDeletaTime * vDir;
		m_CameraDesc.vAt += fSpeedPerSec * fDeletaTime * vDir;
	}
		break;
	case 1:
	{
		//_float3 vTargetPos = ((CTransform*)(m_pManagement->Get_GameObject(L"Layer_Ring", 4)->Get_Component(L"Com_Transform")))->Get_State(EState::Position);
		_float3 vTargetPos = { -200.f, 10.f, 500.f };
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeletaTime;
		
		if(vLength <= 20.f)
			++m_byMoveCount;
	}
		break;

	case 2:
	{
		_float3 vTargetPos = { 0.f, 35.f, 1400.f };
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeletaTime;

		if (vLength <= 20.f)
			++m_byMoveCount;
	}
		break;

	case 3:
	{
		_float3 vTargetPos = { 400.f, 60.f, 1850.f };
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeletaTime;
		
		if (vLength <= 20.f)
			++m_byMoveCount;
	}
	break;
	
	case 4:
	{
		_float3 vTargetPos = { 650.f, 35.f, 1800.f };
		_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeletaTime;

		if (vLength <= 50.f)
		{
			vDir = vPlayerPos - vTargetPos;
			D3DXVec3Normalize(&vDir, &vDir);
			m_CameraDesc.vAt += vDir * fSpeedPerSec * fDeletaTime;
		}

		if (vLength <= 20.f)
		{
			m_CameraDesc.vAt = m_pPlayerTransform->Get_State(EState::Position);
			++m_byMoveCount;
		}

	}
	break;

	case 5:
	{
		_float3 vTargetPos = { 800.f, 10.f, 1400.f };
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeletaTime;

		if (vLength <= 20.f)
			++m_byMoveCount;
	}
	break;

	case 6:
	{
		_float3 vTargetPos = { 505.f, 5.f, 550.f };
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeletaTime;

		if (vLength <= 20.f)
			++m_byMoveCount;
	}
	break;

	case 7:
	{
		_float3 vTargetPos = { 200.f, 0.f, 100.f };
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeletaTime;

		if (vLength <= 2.f)
			++m_byMoveCount;
	}
	break;

	case 8:
	{
		_float3 vTargetPos = m_pPlayerTransform->Get_State(EState::Position);
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeletaTime;

		if (vLength <= 10.f)
			++m_byMoveCount;
	}
	break;

	default:
		Safe_Release(m_pTargetTransform);
		m_IsMoveCountCheck = false;
		// 플레이어 무빙 허용
		((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsCameraMove(false);
		m_eSoloMoveMode = ESoloMoveMode::End;
		break;
	}

	return _uint();
}

_uint CMainCam::Solo_Stage2_Asteroid(_float fDeltaTime)
{
	if (m_IsMoveCountCheck == false)
	{
		m_byMoveCount = 0;
		m_IsMoveCountCheck = true;
		_uint iCount = m_pManagement->Get_GameObjectList(L"Layer_Asteroid")->size();
		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Asteroid", L"Com_Transform",iCount-1);
		Safe_AddRef(m_pTargetTransform);
		if (m_pTargetTransform == nullptr)
		{
			PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
			return UPDATE_ERROR;
		}
	}
	_float fSpeedPerSec = 120.f;
	switch (m_byMoveCount)
	{
	case 0:
		{// at을 플레이어 > 행성
		_float3 vCameraDir = { 0.f,0.f, -1.f };

		// 0.05f 민감도
		D3DXQuaternionSlerp(&m_qCameraRot, &m_qCameraRot, &m_pPlayerTransform->Get_TransformDesc().qRot, 0.05f);

		_float4x4 matCameraRot;
		D3DXMatrixRotationQuaternion(&matCameraRot, &m_qCameraRot);

		// Camera Pos
		_float4x4 matInitCameraRot;
		D3DXMatrixRotationX(&matInitCameraRot, D3DXToRadian(m_fCamAngle));

		D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matInitCameraRot);
		D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matCameraRot);
		_float3 vCameraPos = m_pPlayerTransform->Get_State(EState::Position) + vCameraDir * m_fDistanceFromTarget;

		// Up
		_float3 vCameraUp = { 0.f,1.f,0.f };
		D3DXVec3TransformNormal(&vCameraUp, &vCameraUp, &matCameraRot);

		// LookAt
		_float3 vCameraLookAt = _float3(0.f, 10.f, 0.f);
		memcpy(&matCameraRot._41, &m_pTargetTransform->Get_State(EState::Position), sizeof(_float3));
		D3DXVec3TransformCoord(&vCameraLookAt, &vCameraLookAt, &matCameraRot);

		vCameraPos.x += 20.f;
		vCameraPos.y += 20.f;
		vCameraPos.z += 20.f;

		_float3 vDir = (m_pTargetTransform->Get_State(EState::Position) - m_pPlayerTransform->Get_State(EState::Position));
		_float3 vLen = (m_pPlayerTransform->Get_State(EState::Position) - m_CameraDesc.vEye);

		m_CameraDesc.vEye = vCameraPos;
		m_CameraDesc.vUp = vCameraUp;
		m_CameraDesc.vAt = vCameraLookAt;
		++m_byMoveCount;

		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += fSpeedPerSec * fDeltaTime * vDir;
		m_CameraDesc.vAt += fSpeedPerSec * fDeltaTime * vDir;
		}
	break;
	case 1:
	{
		//_float3 vTargetPos = ((CTransform*)(m_pManagement->Get_GameObject(L"Layer_Ring", 4)->Get_Component(L"Com_Transform")))->Get_State(EState::Position);
		_float3 vTargetPos =m_pTargetTransform->Get_TransformDesc().vPosition;
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float vLength = D3DXVec3Length(&vDir);
		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeltaTime;

		if (vLength <= 40.f)
			++m_byMoveCount;
	}
	break;
	case 2:
	{
		Safe_Release(m_pTargetTransform);
		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
		Safe_AddRef(m_pTargetTransform);
		_float3 vTargetPos = m_pTargetTransform->Get_TransformDesc().vPosition;
		vTargetPos -= m_pTargetTransform->Get_State(EState::Look) * 20;
		m_CameraDesc.vEye = vTargetPos;
			++m_byMoveCount;
	}
		break;
	default:
		Safe_Release(m_pTargetTransform);
		m_IsMoveCountCheck = false;
		// 플레이어 무빙 허용
		((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsCameraMove(false);
		m_eSoloMoveMode = ESoloMoveMode::End;
		break;
	}

	return _uint();
}

_uint CMainCam::Solo_Stage2FinishAsteroid(_float fDeltaTime)
{

	static _uint iCount = 0;

	if (m_IsMoveCountCheck == false)
	{
		m_byMoveCount = 0;
		m_IsMoveCountCheck = true;
		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_NaviArrow", L"Com_Transform");
		if (m_pTargetTransform == nullptr)
		{
			PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
			return UPDATE_ERROR;
		}
		Safe_AddRef(m_pTargetTransform);
	}
	_float fSpeedPerSec = 120.f;
	switch (m_byMoveCount)
	{
	case 0:
	{// at을 플레이어 > 행성
		_float3 vCameraDir = { 0.f,0.f, -1.f };

		// 0.05f 민감도
		D3DXQuaternionSlerp(&m_qCameraRot, &m_qCameraRot, &m_pPlayerTransform->Get_TransformDesc().qRot, 0.05f);

		_float4x4 matCameraRot;
		D3DXMatrixRotationQuaternion(&matCameraRot, &m_qCameraRot);

		// Camera Pos
		_float4x4 matInitCameraRot;
		D3DXMatrixRotationX(&matInitCameraRot, D3DXToRadian(m_fCamAngle));

		D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matInitCameraRot);
		D3DXVec3TransformNormal(&vCameraDir, &vCameraDir, &matCameraRot);
		_float3 vCameraPos = m_pPlayerTransform->Get_State(EState::Position) + vCameraDir * m_fDistanceFromTarget;

		// Up
		_float3 vCameraUp = { 0.f,1.f,0.f };
		D3DXVec3TransformNormal(&vCameraUp, &vCameraUp, &matCameraRot);

		// LookAt
		_float3 vCameraLookAt = _float3(0.f, 10.f, 0.f);
		memcpy(&matCameraRot._41, &m_pTargetTransform->Get_State(EState::Position), sizeof(_float3));
		D3DXVec3TransformCoord(&vCameraLookAt, &vCameraLookAt, &matCameraRot);

		vCameraPos.x += 20.f;
		vCameraPos.y += 20.f;
		vCameraPos.z += 20.f;

		_float3 vDir = (m_pTargetTransform->Get_State(EState::Position) - m_pPlayerTransform->Get_State(EState::Position));
		_float3 vLen = (m_pPlayerTransform->Get_State(EState::Position) - m_CameraDesc.vEye);

		m_CameraDesc.vEye = vCameraPos;
		m_CameraDesc.vUp = vCameraUp;
		m_CameraDesc.vAt = vCameraLookAt;
		++m_byMoveCount;

		D3DXVec3Normalize(&vDir, &vDir);

		m_CameraDesc.vEye += fSpeedPerSec * fDeltaTime * vDir;
		m_CameraDesc.vAt += fSpeedPerSec * fDeltaTime * vDir;
	}
	break;
	case 1:
	{
		_float3 vTargetPos = m_pTargetTransform->Get_TransformDesc().vPosition;
		_float3 vDir = vTargetPos - m_CameraDesc.vEye;
		_float fLength = D3DXVec3Length(&vDir);

		if (fLength <= 20.f)
		{
			if (nullptr == m_pManagement->Get_GameObjectList(L"Layer_NaviArrow"))
			{
				PRINT_LOG(L"Err", L"Layer_NaviArrow is nullptr");
				return UPDATE_ERROR;
			}
			_uint iSize = m_pManagement->Get_GameObjectList(L"Layer_NaviArrow")->size();
			if (iCount < iSize - 1)
			{
				iCount++;
				Safe_Release(m_pTargetTransform);
				m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_NaviArrow", L"Com_Transform", iCount);
				Safe_AddRef(m_pTargetTransform);
			}
			else if (iCount == iSize)
			{
				iCount++;
				Safe_Release(m_pTargetTransform);
				m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Broken_Plane", L"Com_Transform", iCount);
				if (nullptr == m_pTargetTransform)
				{
					PRINT_LOG(L"Err", L"Layer_Broken_Plane is nullptr");
					return UPDATE_ERROR;
				}
				Safe_AddRef(m_pTargetTransform);
			}
			else
				m_byMoveCount++;
		}
		D3DXVec3Normalize(&vDir, &vDir);
		m_CameraDesc.vEye += vDir * fSpeedPerSec * fDeltaTime;
		_float3 vCurAt = m_CameraDesc.vAt;
		_float3 vNextAt =m_pTargetTransform->Get_State(EState::Look);
		D3DXVec3Normalize(&vCurAt, &vCurAt);
		D3DXVec3Normalize(&vNextAt, &vNextAt);
		_float fAngel = acosf(D3DXVec3Dot(&vCurAt, &vNextAt));
		_float3 vAxis;
		D3DXVec3Cross(&vAxis, &vCurAt, &vNextAt);
		_float4x4 matRot;
		D3DXMatrixRotationAxis(&matRot, &vAxis,D3DXToRadian(fAngel));
		_float3 vRot;
		D3DXVec3TransformNormal(&vRot, &vRot, &matRot);
		m_CameraDesc.vAt = m_CameraDesc.vEye + vRot*20.f;
	}
	break;
	case 2:
	{
		Safe_Release(m_pTargetTransform);
		m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
		if (nullptr == m_pTargetTransform)
		{
			PRINT_LOG(L"Err", L"PlayerTransform is nullptr");
			return UPDATE_ERROR;
		}
		Safe_AddRef(m_pTargetTransform);
		_float3 vTargetPos = m_pTargetTransform->Get_TransformDesc().vPosition;
		vTargetPos -= m_pTargetTransform->Get_State(EState::Look) * 20;
		m_CameraDesc.vEye = vTargetPos;
		++m_byMoveCount;
	}
	break;
	default:
		Safe_Release(m_pTargetTransform);
		m_IsMoveCountCheck = false;
		if (m_pManagement->Get_GameObjectList(L"Layer_Player"))
		{
			((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsCameraMove(false);
		}
		else
		{
			PRINT_LOG(L"Err", L"Layer_Player is null");
			return UPDATE_ERROR;
		}

		m_eSoloMoveMode = ESoloMoveMode::End;
		break;
	}

	return NO_EVENT;

}


CMainCam * CMainCam::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CMainCam* pInstance = new CMainCam(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create MainCam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMainCam::Clone(void * pArg/* = nullptr*/)
{
	CMainCam* pClone = new CMainCam(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone MainCam");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMainCam::Free()
{
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pTargetTransform);

	CCamera::Free();
}

_uint CMainCam::FPSMovement(_float fDeltaTime)
{
	return _uint();
}

_uint CMainCam::SetCameraFPS(_float fDeltaTime)
{
	return _uint();
}

void CMainCam::Set_IsSoloMove(ESoloMoveMode eMove)
{
	if (eMove >= ESoloMoveMode::End)
		eMove = ESoloMoveMode::End;

	m_eSoloMoveMode = eMove;
}

const ESoloMoveMode CMainCam::Get_SoloMoveMode()
{
	return m_eSoloMoveMode;
}

_uint CMainCam::CameraShakingStart(_float fDeltaTime, _float Range)
{
	_float3 Shake = { (_float)(rand() % 3), (_float)(rand() % 3), 0.f };

	m_CameraDesc.vEye += Shake * fDeltaTime * Range;

	return _uint();
}

