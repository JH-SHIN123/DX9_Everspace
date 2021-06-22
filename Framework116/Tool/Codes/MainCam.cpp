#include "pch.h"
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
	vPlayerLook *= -m_fDistanceFromTarget;

	_float3 vPlayerRight = m_pPlayerTransform->Get_State(EState::Right);
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	_float4x4 matRot;
	D3DXMatrixRotationAxis(&matRot, &vPlayerRight, m_fCamAngle);
	D3DXVec3TransformNormal(&vPlayerLook, &vPlayerLook, &matRot);

	/* 초록 벡터 */
	m_CameraDesc.vEye = vPlayerPos + vPlayerLook;

	/* 바라볼 위치 */
	m_CameraDesc.vAt = vPlayerPos;

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
