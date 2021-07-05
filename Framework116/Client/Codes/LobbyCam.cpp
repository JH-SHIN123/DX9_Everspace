#include "stdafx.h"
#include "..\Headers\LobbyCam.h"
#include"Pipeline.h"
#include"Lobby.h"
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
	if (m_pLobby)
	{
		if (m_pLobby->Get_IsGatcha())
		{
			m_pPlayerTransform = (CTransform*)m_pManagement->Get_GameObject(L"Layer_GatchaBox")
				->Get_Component(L"Com_Transform");
		}
	}
	if (!m_bGotoNextScene)
		OffSet(fDeltaTime);
	else
		StartChangeScene(fDeltaTime);
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
	
	static _bool bStart = false;
	if (!bStart)
	{
		m_CameraDesc.vAt = m_pPlayerTransform->Get_State(EState::Position);
		
		
		m_CameraDesc.vEye.y += m_fDistanceFromTarget / 2.f;
		bStart = true;
		return 0;
	}
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	
	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	_float3 vGoalDir = vPlayerPos - m_CameraDesc.vEye;
	_float3 vDir = m_CameraDesc.vAt- m_CameraDesc.vEye;
	
	D3DXVec3Normalize(&vGoalDir, &vGoalDir);
	D3DXVec3Normalize(&vDir, &vDir);

	_float3 vAxis;
	D3DXVec3Cross(&vAxis, &vGoalDir,&vDir);

	_float fAngle = D3DXVec3Dot(&vGoalDir,&vDir);
	D3DXMatrixRotationAxis(&matWorld, &vAxis, acosf(fAngle)*fDeltaTime);
	_float3 vTemp = { 1.f,1.f,1.f };
	_float4x4 matTrans, matRot;
	D3DXMatrixTranslation(&matTrans, vPlayerPos.x, vPlayerPos.y, vPlayerPos.z);
	matWorld = matWorld* matTrans;
	D3DXVec3TransformCoord(&m_CameraDesc.vAt, &vTemp, &matWorld);
	return 0;
}

void CLobbyCam::StartChangeScene(_float fDeltaTime)
{
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
	Safe_Release(m_pLobby);
	CCamera::Free();
}
