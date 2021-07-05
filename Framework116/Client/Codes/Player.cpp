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
	wstring meshTag = L"Component_Mesh_BigShip";
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		meshTag,
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
		return E_FAIL;
	}

	// For.Com_Transform Test
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 45.f;
	TransformDesc.vPosition = _float3(50.f, 0.f, 0.f);
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

	// 최초 무기(기관총) HUD 생성.
	UI_DESC MachinegunHUD;
	MachinegunHUD.tTransformDesc.vPosition = { -400.f, 435.f, 0.f };
	MachinegunHUD.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
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
	// For.Com_Collide
	PASSDATA_COLLIDE tCollide;
	CStreamHandler::Load_PassData_Collide(L"BigShip", meshTag, tCollide);

	m_Collides.reserve(tCollide.vecBoundingSphere.size());
	int i = 0;
	for (auto& bounds : tCollide.vecBoundingSphere) {
		if (FAILED(CGameObject::Add_Component(
			EResourceType::Static,
			L"Component_CollideSphere",
			L"Com_CollideSphere" + to_wstring(i++),
			nullptr,
			&bounds,
			true)))
		{
			PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
			return E_FAIL;
		}
	}

	return S_OK;
}

_uint CPlayer::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	KeyProcess(fDeltaTime);

	// 
	Movement(fDeltaTime);

	// 월드행렬 업데이트
	m_pTransform->Update_Transform_Quaternion();

	// 충돌박스 업데이트
	for (auto& collide : m_Collides) 
		collide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
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
	//for (auto& collide : m_Collides)
	//	collide->Render_Collide();
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

	if (GetAsyncKeyState('Q') & 0x8000)
		m_pTransform->RotateZ(fDeltaTime);
	if (GetAsyncKeyState('E') & 0x8000)
		m_pTransform->RotateZ(-fDeltaTime);

	// Weapon Change / Skills (OverDrive, Shield)
	if (m_pController->Key_Down(KEY_1))
	{
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_MACHINEGUN;
		UI_DESC MachinegunHUD;
		MachinegunHUD.tTransformDesc.vPosition = { -400.f, 435.f, 0.f };
		MachinegunHUD.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
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
	else if (m_pController->Key_Down(KEY_2))
	{
		// 이전 무기 HUD 삭제
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_LAZER;
		UI_DESC LaserHUD;
		LaserHUD.tTransformDesc.vPosition = { -400.f, 435.f, 0.f };
		LaserHUD.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
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
	else if (m_pController->Key_Down(KEY_3))
	{
		// 이전 무기 HUD 삭제
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_MISSILE;
		UI_DESC MissileHUD;
		MissileHUD.tTransformDesc.vPosition = { -400.f, 435.f, 0.f };
		MissileHUD.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
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

	if (m_pController->Key_Down(KEY_F2))
	{
		//반짝이게 하는 Effect How?
		m_fOverDrive = 2.f;
		m_bOverDrive = true;
	}
	if (m_pController->Key_Down(KEY_F3))
	{
		// 실드활성화.
	}

	//공격
	if (m_pController->Key_Pressing(KEY_LBUTTON))
	{
		if (m_iWeapon == WEAPON_MACHINEGUN)
		{
			m_fMachinegunFireDelay += fDeltaTime * m_fOverDrive;
			if (m_fMachinegunFireDelay > 0.1f)
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
				m_fMachinegunFireDelay = 0.f;
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

	if (m_pController->Key_Pressing(KEY_LBUTTON))
	{
		float fDist_Monster = 0.f;

		if (CCollision::PickingObject(fDist_Monster, g_hWnd, WINCX, WINCY, m_pDevice,
			m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster")))
		{
			wstring wstrDist = to_wstring(fDist_Monster);
			PRINT_LOG(wstrDist.c_str(), L"Pick!");
		}
	}

	// 마우스 고정시켜서 끄기 불편해서.. ESC키 쓰세용
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		DestroyWindow(g_hWnd);

	//오버드라이브 타이머
	if (m_bOverDrive)
	{
		m_fOverDriveTime -= fDeltaTime;
		// 타이머가 0초가 되면 오버드라이브 꺼지고 연사속도 다시 정상으로.
		if (m_fOverDriveTime <= 0)
		{
			m_bOverDrive = false;
			m_fOverDrive = 1.f;
		}
	}
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

	p1.x = rc.left + 600;
	p1.y = rc.top + 300;
	p2.x = rc.right - 600;
	p2.y = rc.bottom - 300;

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

	// 0.05f 플레이어가 얼만큼 더 회전할건지
	_float fSpeed = D3DXVec3Length(&vGap) * 0.15f;
	D3DXVec3Normalize(&vGap, &vGap);

	m_pTransform->RotateX(D3DXToRadian(vGap.y) * fDeltaTime * fSpeed * 2.5f);
	m_pTransform->RotateY(D3DXToRadian(vGap.x) * fDeltaTime * fSpeed * 0.5f);
	m_pTransform->RotateZ(D3DXToRadian(vGap.x) * -fDeltaTime * fSpeed * 1.f);

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
	Safe_Release(m_pController);

	CGameObject::Free();
}
