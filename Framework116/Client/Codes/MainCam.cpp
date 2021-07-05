#include "stdafx.h"
#include "..\Headers\MainCam.h"
#include "Pipeline.h"

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
	return NO_EVENT;
}

_uint CMainCam::LateUpdate_GameObject(_float fDeltaTime)
{
	CCamera::LateUpdate_GameObject(fDeltaTime);

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
	
	// 카메라 쉐이킹
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		CameraShakingStart(fDeltaTime);
	}


	return _uint();
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

_uint CMainCam::CameraShakingStart(_float fDeltaTime)
{
	_float3 Shake = { (_float)(rand() % 6), (_float)(rand() % 6), 0.f };

	m_CameraDesc.vEye += Shake * fDeltaTime * 5.f;

	return _uint();
}

