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

	m_pPlayerTransform = (CTransform*)m_pManagement->
		Get_Component(L"Layer_Lobby_Model", L"Com_Transform");
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
		if (m_pLobby->Get_GotoNextScene())
		{
			m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Lobby_Model"
				, L"Com_Transform");
			m_bDir = false;
		}
		if (m_pLobby->Get_IsGatcha())
		{
			m_pPlayerTransform = (CTransform*)m_pManagement->Get_GameObject(L"Layer_GatchaBox")
				->Get_Component(L"Com_Transform");
			m_bDir = true;
		}
		else if (m_pLobby->Get_IsSetPlayerModel())
		{
			m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Lobby_Model"
				, L"Com_Transform");
			m_bDir = false;
		}
	}
	OffSet(fDeltaTime);
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
	if (!m_pPlayerTransform)
		return 0;
	if (m_bStartUnPacking)
	{
		if (m_bUnPacked)
		{
			m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Product"
				, L"Com_Transform");
			m_CameraDesc.vAt = m_pPlayerTransform->Get_State(EState::Position);
			return 0;
		}
		else
			return 0;
	}
	
	if (!m_bStart)
	{
		m_CameraDesc.vAt = m_pPlayerTransform->Get_TransformDesc().vPosition;
		m_CameraDesc.vEye.y += m_fDistanceFromTarget / 2.f;
		m_bStart = true;
		return 0;
	}
	if (m_pLobby->Get_GotoNextScene())
	{
		m_CameraDesc.vAt = m_pPlayerTransform->Get_State(EState::Position);
		return 0;
	}
	if (m_pLobby->Get_GotoNextScene())
	{
		m_CameraDesc.vAt = m_pPlayerTransform->Get_State(EState::Position);
		return 0;
	}
	_float3 vTargetPos = m_pPlayerTransform->Get_TransformDesc().vPosition;
	_float3 vAt = m_CameraDesc.vAt;

	vTargetPos.y = 0.f;
	vAt.y = 0.f;
	if (vTargetPos != vAt)
	{
		D3DXVec3Normalize(&vTargetPos, &vTargetPos);
		D3DXVec3Normalize(&vAt, &vAt);
		_float fAngle = 0.f;
		if (m_bDir)
		{
			fAngle = acosf(D3DXVec3Dot(&vAt,&vTargetPos));
			if (fAngle <= D3DXToRadian(1.f))
				return 0;
		}
		else
		{
			fAngle = -acosf(D3DXVec3Dot(&vAt, &vTargetPos));
			if (fAngle >= -D3DXToRadian(1.f))
				return 0;
		}
			
		
<<<<<<< HEAD

=======
>>>>>>> main
		
		fAngle *= fDeltaTime;
		vAt = m_CameraDesc.vAt;
		_float3 vNextAim;
		vNextAim.y = 0.f;
		vNextAim.x = vAt.x* cosf(fAngle) - vAt.z *sinf(fAngle);
		vNextAim.z = vAt.x *sinf(fAngle) + vAt.z*cosf(fAngle);

		m_CameraDesc.vAt = vNextAim;
	}
	
	return 0;
}

void CLobbyCam::StartChangeScene(_float fDeltaTime)
{
	if (m_pLobby)
	{
		if (m_pLobby->Get_GotoNextScene())
		{

		}
	}
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
	
	CCamera::Free();
}
