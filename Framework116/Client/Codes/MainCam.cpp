#include "stdafx.h"
#include "..\Headers\MainCam.h"

CMainCam::CMainCam(LPDIRECT3DDEVICE9 pDevice)
	: CCamera(pDevice)
{
}

CMainCam::CMainCam(const CMainCam & other)
	: CCamera(other)
	, m_fDistanceFromTarget(other.m_fDistanceFromTarget)
	, m_fCamAngle(other.m_fCamAngle)
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
	Movement(fDeltaTime);
	KeyInput(fDeltaTime);

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
	/* 빨간 벡터 */
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);

	/* 파란 벡터 */
	_float3 vPlayerLook = m_pPlayerTransform->Get_State(EState::Look);
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

	/* 보라색 벡터 */
	//vPlayerLook *= -m_fDistanceFromTarget;
	//////////////////////////////////////////

	_float3 vPlayerRight = m_pPlayerTransform->Get_State(EState::Right);
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	_float4x4 matRot;
	D3DXMatrixRotationAxis(&matRot, &vPlayerRight, m_fCamAngle);
	D3DXVec3TransformNormal(&vPlayerLook, &vPlayerLook, &matRot);

	/* 초록 벡터 */
	//m_CameraDesc.vEye = vPlayerPos + vPlayerLook;
	m_CameraDesc.vEye = vPlayerPos + (vPlayerLook) * -m_fDistanceFromTarget * 2.f;

	/* 바라볼 위치 */
	//한 프레임전의 에임
	_float3 vPreAim = m_CameraDesc.vAt;
	//이번에 바뀔 에임 
	_float3 vCurAim = vPlayerPos - (vPlayerLook) * -m_fDistanceFromTarget * 2.f;

	D3DXVec3Normalize(&vPreAim, &vPreAim);
	D3DXVec3Normalize(&vCurAim, &vCurAim);

	//이동이 있을시만 계산
	if (vPreAim != vCurAim)
	{
		D3DXQUATERNION QuatP = { vPreAim.x,vPreAim.y ,vPreAim.z , 0.f }
			//w는 스칼라.축이니 0으로 주자
		, QuatQ = { vCurAim.x ,vCurAim.y,vCurAim.z,0.f };
		D3DXQuaternionSlerp(&QuatQ, &QuatP, &QuatQ, 0.001f/*민감도*/);
		//QuatQ에 두 사원수의 구면 선형보간,민감도가 커지면 카메라 회전이 많이 됨.
		vCurAim = { QuatQ.x,QuatQ.y,
			QuatQ.z };
	}

	//위치
	vCurAim += vPlayerPos;
	m_CameraDesc.vAt = vCurAim;

	m_fCamAngle = 0.f;
	//카메라 위치를 조금 위로
	_float3 vCamDist;
	D3DXVec3Cross(&vCamDist, &vPlayerLook, &vPlayerRight);
	m_CameraDesc.vEye += vCamDist * m_fDistanceFromTarget;

	/* 바라볼 위치 */
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_float3 vMouse = { (_float)pt.x, (_float)pt.y, 0.f };
	_float3 vScreenCenter = { WINCX / 2.f, WINCY / 2.f, 0.f };
	_float3 vGap = vMouse - vScreenCenter;

	if (fabs(vGap.x) >= 150.f)
	{
		if(m_fDistanceFromTarget >= 4.f)
			m_fDistanceFromTarget -= 0.02f;
	}
	else if (fabs(vGap.x) < 150.f)
	{
		if (m_fDistanceFromTarget < 10.f)
		m_fDistanceFromTarget += 0.02f;
	}

	vPlayerPos.y += 10.f;
	m_CameraDesc.vAt = vPlayerPos - (vPlayerLook) * -m_fDistanceFromTarget * 2.f * fDeltaTime;
	


	return _uint();
}

_uint CMainCam::KeyInput(_float fDeltaTime)
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
		if(m_fDistanceFromTarget < 11.f)
			m_fDistanceFromTarget += 9.f * fDeltaTime;
	}

	// 부스터 카메라 연출
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		_float OriginDis = 10.f;

		if (!(m_fDistanceFromTarget > OriginDis + 2.f))
			m_fDistanceFromTarget += 8.f * fDeltaTime;	
	}
	if (!(GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		_float OriginDis = 10.f;
		if(!(m_fDistanceFromTarget < 10.f))
		m_fDistanceFromTarget -= 5.f * fDeltaTime;
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
