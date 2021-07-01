#include "stdafx.h"
#include "..\Headers\Player.h"
#include "Collision.h"

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
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
		return E_FAIL;
	}

	// For.Com_Transform Test
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 45.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vScale = { 0.01f,0.01f,0.01f };

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
	
	// For.Com_Collide
	BOUNDING_SPHERE BoundingSphere;
	BoundingSphere.fRadius = 5.f;

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_CollideSphere",
		L"Com_CollideSphere",
		(CComponent**)&m_pCollide,
		&BoundingSphere,
		true)))
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

_uint CPlayer::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	KeyProcess(fDeltaTime);
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().vPosition);
	return NO_EVENT;
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
	
#ifdef _DEBUG // Render Collide
	/*m_pCollide->Render_Collide();*/
#endif

	return _uint();
}

void CPlayer::KeyProcess(_float fDeltaTime)
{
	if (nullptr == m_pController) return;
	m_pController->Update_Controller();

	// Move
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pTransform->Go_Straight(fDeltaTime);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pTransform->Go_Straight(-fDeltaTime);
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
		m_pTransform->RotateZ(fDeltaTime);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pTransform->RotateZ(-fDeltaTime);
	}

	if (m_pController->Key_Down(KEY_LBUTTON))
	{
		//float fDist_Monster = 0.f;
		//if (CCollision::PickingObject(fDist_Monster, g_hWnd, WINCX, WINCY, m_pDevice,
		//	m_pManagement->Get_GameObjectList(L"Layer_Monster"))) {
		//	PRINT_LOG(L"", L"Pick!");
		//}
	}
}

_uint CPlayer::Movement(_float fDeltaTime)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	RECT rc;
	POINT p1, p2;

	GetClientRect(g_hWnd, &rc);

	p1.x = rc.left + 100.f;
	p1.y = rc.top + 100.f;
	p2.x = rc.right - 100.f;
	p2.y = rc.bottom - 100.f;

	ClientToScreen(g_hWnd, &p1);
	ClientToScreen(g_hWnd, &p2);

	rc.left = p1.x;
	rc.top = p1.y;
	rc.right = p2.x;
	rc.bottom = p2.y;

	ClipCursor(&rc);
	


	_float3 vMouse = { (_float)pt.x, (_float)pt.y, 0.f };
	_float3 vScreenCenter = { WINCX / 2.f, WINCY / 2.f, 0.f };
	_float3 vGap = vMouse - vScreenCenter;

	_float fSpeed = D3DXVec3Length(&vGap) * 1.f;
	D3DXVec3Normalize(&vGap, &vGap);
	_float fNewRotX = vGap.y;
	_bool bRotYDir = false; // true면 위쪽 회전, false면 밑에 회전
	if (fNewRotX < 0.f)
		bRotYDir = false;
	else if (fNewRotX > 0.f)
		bRotYDir = true;

	_float fRotX = m_pTransform->Get_TransformDesc().vRotate.x;

	if (fRotX >= -D3DXToRadian(90.f) && !bRotYDir)
		m_pTransform->RotateX(D3DXToRadian(vGap.y) * fDeltaTime * fSpeed);
	else if (fRotX < D3DXToRadian(65.f) && bRotYDir)
	{
		m_pTransform->RotateX(D3DXToRadian(vGap.y) * fDeltaTime * fSpeed);
	}

	m_pTransform->RotateY(D3DXToRadian(vGap.x) * fDeltaTime * fSpeed);

	
	//POINT ptMouse = { WINCX >> 1, WINCY >> 1 }; // 400 / 300
	//ClientToScreen(g_hWnd, &ptMouse);
	//	SetCursorPos(ptMouse.x, ptMouse.y);

	// 범위 정해줘야함. 100 ~ WINCX - 100 , 100 ~ WINCY;

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
	Safe_Release(m_pCollide);
	Safe_Release(m_pController);

	CGameObject::Free();
}
