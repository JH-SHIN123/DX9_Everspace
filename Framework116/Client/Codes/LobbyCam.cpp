#include "stdafx.h"
#include "..\Headers\LobbyCam.h"

CLobbyCam::CLobbyCam(LPDIRECT3DDEVICE9 pDevice)
	: CCamera(pDevice)
{
}

CLobbyCam::CLobbyCam(const CLobbyCam & other)
	: CCamera(other)
	, m_fDistanceFromTarget(other.m_fDistanceFromTarget)
	, m_fCamAngle(other.m_fCamAngle)
{
}

HRESULT CLobbyCam::Ready_GameObject_Prototype()
{
	CCamera::Ready_GameObject_Prototype();

	return S_OK;
}
 
HRESULT CLobbyCam::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CCamera::Ready_GameObject(pArg);

	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Lobby_Model", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);
	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

_uint CLobbyCam::Update_GameObject(_float fDeltaTime)
{
	CCamera::Update_GameObject(fDeltaTime);
	OffSet(fDeltaTime);
	return NO_EVENT;
}

_uint CLobbyCam::LateUpdate_GameObject(_float fDeltaTime)
{
	CCamera::LateUpdate_GameObject(fDeltaTime);

	return _uint();
}

_uint CLobbyCam::Render_GameObject()
{
	CCamera::Render_GameObject();

	return _uint();
}




_uint CLobbyCam::OffSet(_float fDeltaTime)
{
	static _bool bIsStart = false;
	if (bIsStart)
		return TRUE;

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
	
	bIsStart = true;
	return TRUE;
}

CLobbyCam * CLobbyCam::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLobbyCam* pInstance = new CLobbyCam(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create MainCam");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLobbyCam::Clone(void * pArg/* = nullptr*/)
{
	CLobbyCam* pClone = new CLobbyCam(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone MainCam");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLobbyCam::Free()
{
	Safe_Release(m_pPlayerTransform);

	CCamera::Free();
}
