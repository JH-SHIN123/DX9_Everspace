#include "stdafx.h"
#include "..\Headers\LobbyModel.h"
#include "Collision.h"
#include"Player.h"
#include"Lobby.h"
CLobbyModel::CLobbyModel(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	
	
}

CLobbyModel::CLobbyModel(const CLobbyModel & other)
	: CGameObject(other)
{
}

HRESULT CLobbyModel::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLobbyModel::Ready_GameObject(void * pArg/* = nullptr*/)
{
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_BigShip",
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
		return E_FAIL;
	}

	// For.Com_Transform Test
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 45.f;
	TransformDesc.vPosition = _float3(15.f, 0.f, 15.f);
	TransformDesc.fSpeedPerSec = 25.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vScale = { 1.f,1.f,1.f };

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		&TransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	// For.Com_Controller
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Controller",
		L"Com_Controller",
		(CComponent**)&m_pController)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Controller");
		return E_FAIL;
	}

	return S_OK;
}

_uint CLobbyModel::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);
	if (!m_bGotoNextScene)
	{
		KeyProcess(fDeltaTime);
		Movement(fDeltaTime);
		m_pTransform->Update_Transform();
	}
	else
	{
		StartSceneChange(fDeltaTime);

	}
	return m_pTransform->Update_Transform();;
}

_uint CLobbyModel::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;
	if (m_bGo_Straight)
	{
		m_fDelaySceneChange += fDeltaTime;
		if (m_fDelaySceneChange >= 5.f)
			m_pLobby->Set_SceneChange(TRUE);
	}
	return _uint();
}

_uint CLobbyModel::Render_GameObject()
{
	if (m_pLobby->Get_SceneSelect() && !m_pLobby->Get_GotoNextScene())
		return 0;
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();

#ifdef _DEBUG // Render Collide
	/*m_pCollide->Render_Collide();*/
#endif

	return _uint();
}

void CLobbyModel::KeyProcess(_float fDeltaTime)
{
	if (nullptr == m_pController) return;
	m_pController->Update_Controller();
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_pTransform->RotateZ(D3DXToRadian(45.f)*fDeltaTime);
	}
	else if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pTransform->RotateZ(-D3DXToRadian(45.f)*fDeltaTime);
	}
	
	

}

_uint CLobbyModel::Movement(_float fDeltaTime)
{

	m_pTransform->RotateY(-D3DXToRadian(45.f)*fDeltaTime);
	return _uint();
}

void CLobbyModel::StartSceneChange(_float fDeltaTime)
{
	static _float fTime = 0;
	fTime += fDeltaTime;
	if (m_bGo_Straight)
	{
		m_pTransform->Go_Straight(fDeltaTime* 10.f);
		return;
	}
	if (fTime < 5.f)
	{
		m_pTransform->Go_Up(0.01f*fDeltaTime);
	}
	else
	{
		_float3 vPlayerPos = m_pTransform->Get_State(EState::Position);
		_float3 vTargetDir = _float3( -1.f,0.f,1.f );
		_float3 vDir = m_pTransform->Get_State(EState::Look);
		vDir.y = 0.f;
		D3DXVec3Normalize(&vDir, &vDir);
		D3DXVec3Normalize(&vTargetDir,&vTargetDir);
		_float fAngle = acosf(D3DXVec3Dot(&vTargetDir, &vDir));
		m_pTransform->RotateY(fDeltaTime*D3DXToRadian(fAngle)*20.f);
		
		if (0.05>= fAngle)
		{
			vTargetDir = _float3(0.f, 1.f, 1.f);
			vDir = m_pTransform->Get_State(EState::Look);
			vDir.x = 0.f;
			D3DXVec3Normalize(&vTargetDir, &vTargetDir);
			
			fAngle = D3DXVec3Dot(&vDir, &vTargetDir);
			m_pTransform->RotateX(fDeltaTime*fAngle);
			if (0.005 >= fAngle)
			{
				m_bGo_Straight = TRUE;
			}
		}
	}
}


CLobbyModel * CLobbyModel::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLobbyModel* pInstance = new CLobbyModel(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLobbyModel::Clone(void * pArg/* = nullptr*/)
{
	CLobbyModel* pClone = new CLobbyModel(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLobbyModel::Free()
{
	Safe_Release(m_pLobby);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pController);

	CGameObject::Free();
}
