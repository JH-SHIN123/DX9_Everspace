#include "stdafx.h"
#include "..\Headers\Player.h"
#include "Collision.h"
#include "Pipeline.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pPassData)
	: CGameObject(pDevice)
{
	m_pPassData = pPassData;
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
		m_pPassData->vecPrototypeTag_Mesh[0],
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
		return E_FAIL;
	}

	// For.Com_Transform Test
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 45.f;
	TransformDesc.vPosition = _float3(0.f, 0.f, 0.f);
	TransformDesc.fSpeedPerSec = 25.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vScale = { 1.f,1.f,1.f };

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		m_pPassData->vecPrototypeTag_Mesh[1],
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
		m_pPassData->vecPrototypeTag_Mesh[2],
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
		m_pPassData->vecPrototypeTag_Mesh[3],
		L"Com_Controller",
		(CComponent**)&m_pController)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Controller");
		return E_FAIL;
	}

	// 최초 무기(기관총) HUD 생성.
	UI_DESC MachinegunHUD;
	MachinegunHUD.tTransformDesc.vPosition = { -140.f, 250.f, 0.f };
	MachinegunHUD.tTransformDesc.vScale = { 50.f, 45.f, 0.f };
	MachinegunHUD.wstrTexturePrototypeTag = L"Component_Texture_Machinegun_HUD";
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_UI",
		L"Layer_HUD_Weapon",
		(void*)&MachinegunHUD)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
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

		// Booster
		if (m_pController->Key_Pressing(KEY_SPACE))
			m_pTransform->Go_Straight(fDeltaTime * 0.8f);
	}

	if (GetAsyncKeyState('S') & 0x8000)
		m_pTransform->Go_Straight(-fDeltaTime);

	if (GetAsyncKeyState('D') & 0x8000)
		m_pTransform->Go_Side(fDeltaTime);

	if (GetAsyncKeyState('A') & 0x8000)
		m_pTransform->Go_Side(-fDeltaTime);

	// Weapon Change
	if (m_pController->Key_Down(KEY_1))
	{
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_MACHINEGUN;
		UI_DESC MachinegunHUD;
		MachinegunHUD.tTransformDesc.vPosition = { -140.f, 250.f, 0.f };
		MachinegunHUD.tTransformDesc.vScale = { 50.f, 45.f, 0.f };
		MachinegunHUD.wstrTexturePrototypeTag = L"Component_Texture_Machinegun_HUD";
		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::Static,
			L"GameObject_UI",
			L"Layer_HUD_Weapon",
			(void*)&MachinegunHUD)))
		{
			PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
			return;
		}
	}
	if (m_pController->Key_Down(KEY_2))
	{
		// 이전 무기 HUD 삭제
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_LAZER;
		UI_DESC LaserHUD;
		LaserHUD.tTransformDesc.vPosition = { -140.f, 250.f, 0.f };
		LaserHUD.tTransformDesc.vScale = { 50.f, 45.f, 0.f };
		LaserHUD.wstrTexturePrototypeTag = L"Component_Texture_Laser_HUD";
		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::Static,
			L"GameObject_UI",
			L"Layer_HUD_Weapon",
			(void*)&LaserHUD)))
		{
			PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
			return;
		}
	}
	if (m_pController->Key_Down(KEY_3))
	{
		// 이전 무기 HUD 삭제
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_MISSILE;
		UI_DESC MissileHUD;
		MissileHUD.tTransformDesc.vPosition = { -140.f, 250.f, 0.f };
		MissileHUD.tTransformDesc.vScale = { 50.f, 45.f, 0.f };
		MissileHUD.wstrTexturePrototypeTag = L"Component_Texture_Missile_HUD";
		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::Static,
			L"GameObject_UI",
			L"Layer_HUD_Weapon",
			(void*)&MissileHUD)))
		{
			PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
			return;
		}
	}

	if (m_pController->Key_Down(KEY_F3))
	{

	}

	// 마우스 고정시켜서 끄기 불편해서.. ESC키 쓰세용
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		DestroyWindow(g_hWnd);


	if (m_pController->Key_Pressing(KEY_LBUTTON))
	{
		if (m_iWeapon == WEAPON_MACHINEGUN)
		{
			m_fMachinegunDelay += fDeltaTime;
			if (m_fMachinegunDelay > 0.1f)
			{
				if (m_IsLeft)
					m_IsLeft = false;
				else
					m_IsLeft = true;

				if (FAILED(m_pManagement->Add_GameObject_InLayer(
					EResourceType::Static,
					L"GameObject_Player_Bullet",
					L"Layer_Player_Bullet",
					(void*)m_IsLeft)))
				{
					PRINT_LOG(L"Error", L"Failed To Add Player_Bullet In Layer");
					return;
				}
				m_fMachinegunDelay = 0.f;
			}
		}
		else if (m_iWeapon == WEAPON_LAZER)
		{
			if (!m_IsLazer)
			{
				if (FAILED(m_pManagement->Add_GameObject_InLayer(
					EResourceType::Static,
					L"GameObject_Player_Lazer",
					L"Layer_Player_Lazer",
					(void*)true)))
				{
					PRINT_LOG(L"Error", L"Failed To Add Player_Lazer In Layer");
					return;
				}
				if (FAILED(m_pManagement->Add_GameObject_InLayer(
					EResourceType::Static,
					L"GameObject_Player_Lazer",
					L"Layer_Player_Lazer",
					(void*)false)))
				{
					PRINT_LOG(L"Error", L"Failed To Add Player_Lazer In Layer");
					return;
				}
			}
			if (m_pManagement->Get_GameObjectList(L"Layer_Player_Lazer")->size() == 2)
				m_IsLazer = true;
			else
				m_IsLazer = false;
			
		}
		else if (m_iWeapon == WEAPON_MISSILE)
		{
			if (!m_IsMissile)
			{
				if (FAILED(m_pManagement->Add_GameObject_InLayer(
					EResourceType::Static,
					L"GameObject_Player_Missile",
					L"Layer_Player_Missile")))
				{
					PRINT_LOG(L"Error", L"Failed To Add Player_Lazer In Layer");
					return;
				}
				
			}
			if (m_pManagement->Get_GameObjectList(L"Layer_Player_Missile")->size() == 1)
				m_IsMissile = true;
			else
				m_IsMissile = false;
		}
	}


	// TEST //
	//if (m_pController->Key_Down(KEY_LBUTTON))
	//{
	//	float fDist_Monster = 0.f;
	//	if (CCollision::PickingObject(fDist_Monster, g_hWnd, WINCX, WINCY, m_pDevice,
	//		m_pManagement->Get_GameObjectList(L"Layer_Monster"))) {
	//		PRINT_LOG(L"", L"Pick!");
	//	}
	//}

}

_uint CPlayer::Movement(_float fDeltaTime)
{

	// 화면 가둬줄 가상의 네모

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	RECT rc;
	POINT p1, p2;

	GetClientRect(g_hWnd, &rc);

	p1.x = rc.left + 100;
	p1.y = rc.top + 100;
	p2.x = rc.right - 100;
	p2.y = rc.bottom - 100;

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

	_float fSpeed = D3DXVec3Length(&vGap) * 0.2f;
	D3DXVec3Normalize(&vGap, &vGap);
	_float fNewRotX = vGap.y;
	_bool bRotYDir = false; // true면 위쪽 회전, false면 밑에 회전
	if (fNewRotX < 0.f)
		bRotYDir = false;
	else if (fNewRotX > 0.f)
		bRotYDir = true;

	_float fRotX = m_pTransform->Get_TransformDesc().vRotate.x;
	
	//90도 넘을때 반전되는 문제 ->

	if (fRotX >= -D3DXToRadian(90.f) && !bRotYDir)
		m_pTransform->RotateX(D3DXToRadian(vGap.y) * fDeltaTime * fSpeed);
	else if (fRotX < D3DXToRadian(55.f) && bRotYDir)
	{
		m_pTransform->RotateX(D3DXToRadian(vGap.y) * fDeltaTime * fSpeed);
	}

	m_pTransform->RotateY(D3DXToRadian(vGap.x) * fDeltaTime * fSpeed);
	

	return _uint();
}

CPlayer * CPlayer::Create(LPDIRECT3DDEVICE9 pDevice, PASSDATA_OBJECT* pPassData)
{
	CPlayer* pInstance = new CPlayer(pDevice, pPassData);
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
