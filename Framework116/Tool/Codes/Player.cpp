#include "pch.h"
#include "..\Headers\Player.h"
#include <MainFrm.h>
#include "MainView.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CPlayer::CPlayer(const CPlayer & other)
	: CGameObject(other)
{
}

HRESULT CPlayer::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPlayer::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_BigShip",
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Component_Mesh_Axis");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vScale = { 0.005f,0.005f,0.005f };

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

	//// Setting Prev Cursor 
	//GetCursorPos(&m_tPrevCursorPos);
	//ScreenToClient(g_hWnd, &m_tPrevCursorPos);

	return S_OK;
}

_uint CPlayer::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CPlayer::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CPlayer::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();

	return _uint();
}

_uint CPlayer::Movement(_float fDeltaTime)
{
	//// Mouse Rotate
	//GetCursorPos(&m_tCurCursorPos);
	//ScreenToClient(g_hWnd, &m_tCurCursorPos);

	//// 이전 프레임과 현재프레임의 마우스 이동거리 구하기
	//_float2 vGap = { float(m_tCurCursorPos.x - m_tPrevCursorPos.x) ,
	//	float(m_tCurCursorPos.y - m_tPrevCursorPos.y) };

	//m_tPrevCursorPos = m_tCurCursorPos;

	//float dps = 100.f;
	//m_pTransform->RotateX(D3DXToRadian(vGap.y) * fDeltaTime * dps);
	//m_pTransform->RotateY(D3DXToRadian(vGap.x) * fDeltaTime * dps);

	//// 마우스 중앙 고정
	//POINT ptMouse = { WINCX >> 1, WINCY >> 1 };
	//ClientToScreen(g_hWnd, &ptMouse);
	//SetCursorPos(ptMouse.x, ptMouse.y);

	// Move
	_float3 vOutPos = m_pTransform->Get_State(EState::Position);
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pTransform->Go_Straight(fDeltaTime);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pTransform->Go_Straight(-fDeltaTime);
		m_pTransform->Go_Straight(fDeltaTime);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pTransform->Go_Side(fDeltaTime);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_pTransform->Go_Side(-fDeltaTime);
	}

	// Rotate
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_pTransform->RotateY(-fDeltaTime);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pTransform->RotateY(fDeltaTime);
	}	

	return _uint();
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CPlayer* pInstance = new CPlayer(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg/* = nullptr*/)
{
	CPlayer* pClone = new CPlayer(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlayer::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);

	CGameObject::Free();
}
