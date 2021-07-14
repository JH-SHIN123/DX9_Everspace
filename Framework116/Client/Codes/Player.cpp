#include "stdafx.h"
#include "..\Headers\Player.h"
#include "Collision.h"
#include "Pipeline.h"
#include "EngineEffectSystem.h"
#include "WingBoost_System.h"
#include "HP_Bar.h"
#include "Stamina_Bar.h"
#include "CollisionHandler.h"
#include "ScriptUI.h"
#include "MainCam.h"
#include "LockOnAlert.h"
#include "HUD_Effect_Boost.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CPlayer::CPlayer(const CPlayer & other)
	: CGameObject(other)
{
}

void CPlayer::Update_Effect()
{
	// Engine-Boost Effect
	if (m_pLeftEngineEffect) {
		_float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
		vEnginePos += m_pTransform->Get_State(EState::Right) * m_vLeftEngineOffset.x;
		vEnginePos += m_pTransform->Get_State(EState::Up) * m_vLeftEngineOffset.y;
		vEnginePos += m_pTransform->Get_State(EState::Look) * m_vLeftEngineOffset.z;
		m_pLeftEngineEffect->Set_EngineOffset(vEnginePos);
		m_pLeftEngineEffect->Set_IsBoost(m_IsBoost);
	}
	if (m_pRightEngineEffect) {
		_float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
		vEnginePos += m_pTransform->Get_State(EState::Right) * m_vRightEngineOffset.x;
		vEnginePos += m_pTransform->Get_State(EState::Up) * m_vRightEngineOffset.y;
		vEnginePos += m_pTransform->Get_State(EState::Look) * m_vRightEngineOffset.z;
		m_pRightEngineEffect->Set_EngineOffset(vEnginePos);
		m_pRightEngineEffect->Set_IsBoost(m_IsBoost);
	}

	// Wing-Boost Effect
	if (m_fStamina > 1.f)
	{
		if (m_pLeftWingBoost) {
			_float3 vWingPos = m_pTransform->Get_TransformDesc().vPosition;
			vWingPos += m_pTransform->Get_State(EState::Right) * m_vLeftWingOffset.x;
			vWingPos += m_pTransform->Get_State(EState::Up) * m_vLeftWingOffset.y;
			vWingPos += m_pTransform->Get_State(EState::Look) * m_vLeftWingOffset.z;
			m_pLeftWingBoost->Set_WingOffset(vWingPos);
			m_pLeftWingBoost->Set_IsBoost(m_IsBoost);
		}
		if (m_pRightWingBoost) {
			_float3 vWingPos = m_pTransform->Get_TransformDesc().vPosition;
			vWingPos += m_pTransform->Get_State(EState::Right) * m_vRightWingOffset.x;
			vWingPos += m_pTransform->Get_State(EState::Up) * m_vRightWingOffset.y;
			vWingPos += m_pTransform->Get_State(EState::Look) * m_vRightWingOffset.z;
			m_pRightWingBoost->Set_WingOffset(vWingPos);
			m_pRightWingBoost->Set_IsBoost(m_IsBoost);
		}
	}
}

HRESULT CPlayer::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPlayer::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	GAMEOBJECT_DESC* pDesc = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (pDesc = dynamic_cast<GAMEOBJECT_DESC*>(ptr))
		{}
		else
		{
			PRINT_LOG(L"Error", L"GAMEOBJECT_DESC is nullptr");
			return E_FAIL;
		}
	}

	// For.Com_VIBuffer
	wstring meshTag = pDesc->wstrMeshName;
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
	TRANSFORM_DESC TransformDesc = pDesc->tTransformDesc;
	TransformDesc.fSpeedPerSec = 35.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(120.f);

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

	// 최초 무기(펄스) HUD 생성.
	UI_DESC LaserHUD;
	LaserHUD.tTransformDesc.vPosition = { -300.f, 435.f, 0.f };
	LaserHUD.tTransformDesc.vScale = { 130.f, 90.f, 0.f };
	LaserHUD.wstrTexturePrototypeTag = L"Component_Texture_Laser_HUD";
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_UI",
		L"Layer_HUD_Weapon",
		(void*)&LaserHUD)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}

	// HP 바.
	m_fHp = 100.f;
	m_fFullHp = m_fHp;

	CGameObject* pGameObject = nullptr;
	UI_DESC HUD_Hp_Bar;
	HUD_Hp_Bar.tTransformDesc.vPosition = { -828.5f, 455.f, 0.f };
	HUD_Hp_Bar.tTransformDesc.vScale = { m_fHp * (m_fHpLength / m_fFullHp), 8.f, 0.f };
	HUD_Hp_Bar.wstrTexturePrototypeTag = L"Component_Texture_HP_Bar";
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_HP_Bar",
		L"Layer_HP_Bar",
		&HUD_Hp_Bar, &pGameObject)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}
	m_pHp_Bar = static_cast<CHP_Bar*>(pGameObject);
	m_pHp_Bar->Who_Make_Me(m_pHp_Bar->MAKER_PLAYER);

	// 스태미너 바
	CGameObject* pGameObjectStamina = nullptr;
	UI_DESC HUD_Stamina_Bar;
	HUD_Stamina_Bar.tTransformDesc.vPosition = { 0.f, 0.f, 0.f };
	HUD_Stamina_Bar.tTransformDesc.vScale = { m_fStamina * (m_fStaminaLength / m_fFullStamina), 8.f, 0.f };
	HUD_Stamina_Bar.wstrTexturePrototypeTag = L"Component_Texture_Stamina_Bar";
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Stamina_Bar",
		L"Layer_Stamina_Bar",
		&HUD_Stamina_Bar, &pGameObjectStamina)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Stamina UI In Layer");
		return E_FAIL;
	}
	m_pStamina_Bar = static_cast<CStamina_Bar*>(pGameObjectStamina);
	m_pStamina_Bar->Who_Make_Me(m_pStamina_Bar->MAKER_PLAYER);

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

	// Add Engine-Boost Effect
	CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pLeftEngineEffect);
	m_vLeftEngineOffset = { -1.4f, 0.9f, -6.7f };
	CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pRightEngineEffect);
	m_vRightEngineOffset = { 1.4f, 0.9f, -6.7f };
	
	// Add Wing-Boost Effect
	CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pLeftWingBoost);
	m_vLeftWingOffset = { -8.2f, -1.5f, -2.f };
	CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pRightWingBoost);
	m_vRightWingOffset = { 8.2f, -1.5f, -2.f };
	

	// Add HUD Boost Effect
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_HUD_Effect_Boost",
		L"Layer_HUD_Effect",
		nullptr,
		(CGameObject**)&m_pHUD_Effect_Boost)))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_HUD_Effect_Damage In Layer");
		return E_FAIL;
	}
	m_pHUD_Effect_Boost->Release();

	return S_OK;
}

_uint CPlayer::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	KeyProcess(fDeltaTime);

	
	Movement(fDeltaTime);

	TimeOperation(fDeltaTime);
	
	Make_Arrow();

	Make_LockOn_Alert(fDeltaTime);


	// 월드행렬 업데이트
	m_pTransform->Update_Transform_Quaternion();

	if (!m_IsDead)
	{
		Movement(fDeltaTime);
		Increase_Stamina(fDeltaTime);
		TimeOperation(fDeltaTime);

		Make_Arrow();

		// 월드행렬 업데이트
		m_pTransform->Update_Transform_Quaternion();

		Collide_Planet_Or_Astroid(fDeltaTime);
		// 충돌박스 업데이트
		for (auto& collide : m_Collides)
			collide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);

		// (순서 중요!) 이펙트 업데이트
		Update_Effect();
	}
	return NO_EVENT;
}

_uint CPlayer::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);
	
	if (m_fHp <= 0.f && !m_IsDead)
	{
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);

		// 기존 이펙트 모두 off

		m_IsDead = true;
	}
	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CPlayer::Render_GameObject()
{
	CGameObject::Render_GameObject();

	if (!m_IsDead)
	{
		m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
		m_pMesh->Render_Mesh();


	wstring str = L"궁서";
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	m_pManagement->Get_Font()->DrawText(NULL
		, str.c_str(), -1
		, &rc, DT_CENTER, D3DXCOLOR(255, 0, 0, 255));

	// 카메라 스킵
	if (m_IsCameraMove)
	{
		ESoloMoveMode eCheck = ((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Get_SoloMoveMode();
		if (eCheck < ESoloMoveMode::Lock)
		{
			wstring mesage = L"C 키를 눌러 스킵";
			RECT tUIBounds;
			GetClientRect(g_hWnd, &tUIBounds);
			//tUIBounds.top += 700;
			tUIBounds.left += 1700;
			m_pManagement->Get_Font()->DrawText(NULL
				, mesage.c_str(), -1
				, &tUIBounds, DT_CENTER, D3DXCOLOR(100, 100, 100, 255));
		}
	}


#ifdef _DEBUG // Render Collide
		//for (auto& collide : m_Collides)
			//collide->Render_Collide();
#endif
	}
	return _uint();
}

_uint CPlayer::Set_IsScript(_bool IsScript)
{
	m_IsScript = IsScript;
	return _uint();
}

_uint CPlayer::Set_IsCameraMove(_bool IsCameraMove)
{
	m_IsCameraMove = IsCameraMove;

	return _uint();
}

void CPlayer::KeyProcess(_float fDeltaTime)
{
	if (nullptr == m_pController) return;
	m_pController->Update_Controller();

	// 대화
	if (m_IsScript == true)
	{
		if (m_pController->Key_Down(KEY_F))
		{
			static_cast<CScriptUI*>(m_pManagement->Get_GameObjectList(L"Layer_ScriptUI")
				->front())->Set_NextScript();
		}
		return;
	}

	if (m_IsCameraMove == true)
		return;
		
	// Move
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pTransform->Go_Straight(fDeltaTime);
		m_pManagement->PlaySound(L"Player_Move.ogg", CSoundMgr::PLAYER_MOVE);
	}

	
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pTransform->Go_Straight(-fDeltaTime);
		m_pManagement->PlaySound(L"Player_Move.ogg", CSoundMgr::PLAYER_MOVE);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pTransform->Go_Side(fDeltaTime);
		m_pManagement->PlaySound(L"Player_Move.ogg", CSoundMgr::PLAYER_MOVE);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_pTransform->Go_Side(-fDeltaTime);
		m_pManagement->PlaySound(L"Player_Move.ogg", CSoundMgr::PLAYER_MOVE);
	}

	if ((!GetAsyncKeyState('W') && 0x8000)
		&& (!GetAsyncKeyState('A') && 0x8000)
		&& (!GetAsyncKeyState('S') && 0x8000)
		&& (!GetAsyncKeyState('D') && 0x8000))
	{
		m_pManagement->StopSound(CSoundMgr::PLAYER_MOVE);
	}

	// Booster
	if (m_pController->Key_Pressing(KEY_SPACE))
	{
		if (m_fStamina > m_fMinStamina)
		{
			m_IsBoost = true;
			m_pTransform->Go_Straight(fDeltaTime * 1.5f);
			//Stamina
			m_IsStaminaShrink = true;
			m_fStamina -= 0.2f;
			m_pStamina_Bar->Set_ScaleX(-0.2f / m_fFullStamina * m_fStaminaLength);

			if (m_pHUD_Effect_Boost && (m_fStamina > m_fMinStamina * 2.f)) {
				m_pManagement->PlaySound(L"Player_Boost_Loop.ogg", CSoundMgr::PLAYER_BOOST);
				m_pHUD_Effect_Boost->Set_Operate(m_IsBoost);
			}
		}
		else
		{
			m_IsBoost = false;
			m_IsStaminaShrink = false;

			if (m_pHUD_Effect_Boost)
				m_pHUD_Effect_Boost->Set_Operate(m_IsBoost);
		}
	}
	if (m_pController->Key_Up(KEY_SPACE))
	{
		m_pManagement->StopSound(CSoundMgr::PLAYER_BOOST);
		m_IsBoost = false;
		//Stamina
		m_IsStaminaShrink = false;

		if (m_pHUD_Effect_Boost)
			m_pHUD_Effect_Boost->Set_Operate(m_IsBoost);
	}
	// Rotate
	if (GetAsyncKeyState('Q') & 0x8000)
		m_pTransform->RotateZ(fDeltaTime);
	if (GetAsyncKeyState('E') & 0x8000)
		m_pTransform->RotateZ(-fDeltaTime);

	// Weapon Change / Skills (OverDrive, Shield)
	if (m_pController->Key_Down(KEY_1))
	{
		// 이전 무기 HUD 삭제
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_LAZER;
		UI_DESC LaserHUD;
		LaserHUD.tTransformDesc.vPosition = { -300.f, 435.f, 0.f };
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
	else if (m_pController->Key_Down(KEY_2))
	{
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_MACHINEGUN;
		UI_DESC MachinegunHUD;
		MachinegunHUD.tTransformDesc.vPosition = { -300.f, 435.f, 0.f };
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
	else if (m_pController->Key_Down(KEY_3))
	{
		// 이전 무기 HUD 삭제
		m_pManagement->Get_GameObjectList(L"Layer_HUD_Weapon")->front()->Set_IsDead(TRUE);
		m_iWeapon = WEAPON_MISSILE;
		UI_DESC MissileHUD;
		MissileHUD.tTransformDesc.vPosition = { -300.f, 435.f, 0.f };
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
	// 피깎는 !TEST!!!!!!!!!!!!!
	if (m_pController->Key_Down(KEY_F1))
	{
		m_fHp -= 10.f;
		m_pHp_Bar->Set_ScaleX(-10.f / m_fFullHp * m_fHpLength);

		// HIT Effect
		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::Static,
			L"GameObject_HUD_Effect_Damage",
			L"Layer_HUD_Effect")))
		{
			PRINT_LOG(L"Error", L"Failed To Add GameObject_HUD_Effect_Damage In Layer");
			return;
		}
	}
	if (m_pController->Key_Down(KEY_F2))
	{
		//반짝이게 하는 Effect How?
		m_pManagement->PlaySound(L"Overdrive.ogg", CSoundMgr::PLAYER_SKILL);
		m_fOverDrive = 2.f;
		m_bOverDrive = true;
	}
	if (m_pController->Key_Down(KEY_F3))
	{
		if (!m_IsShield)
		{
			m_pManagement->PlaySound(L"Shield_Boost.ogg", CSoundMgr::PLAYER_SKILL);
			// 실드활성화.
			if (FAILED(m_pManagement->Add_GameObject_InLayer(
				EResourceType::Static,
				L"GameObject_Shield_Battery",
				L"Layer_Player_Shield")))
			{
				PRINT_LOG(L"Error", L"Failed To Add Shield_Battery In Layer");
				return;
			}
			m_IsShield = true;
		}
	}

	//공격
	if (m_pController->Key_Pressing(KEY_LBUTTON))
	{
		if (m_iWeapon == WEAPON_LAZER)
		{
			m_fMachinegunFireDelay += fDeltaTime * m_fOverDrive;
			if (m_fMachinegunFireDelay > 0.25f)
			{
				m_IsFire = true;

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
				m_pManagement->StopSound(CSoundMgr::PLAYER_WEAPON);
				m_pManagement->PlaySound(L"Pulse_Laser.ogg", CSoundMgr::PLAYER_WEAPON);
				m_fMachinegunFireDelay = 0.f;
			}
			else
			{
				m_IsFire = false;
			}
		}
		else if (m_iWeapon == WEAPON_MACHINEGUN)
		{
			//총열돌리는시간
			if (m_IsShooting == false)
			{
				m_IsFire = false;
				m_fMachinegunDelay += fDeltaTime;
				m_pManagement->PlaySound(L"Gatling_StartUp.ogg", CSoundMgr::PLAYER_GATLING);
			}
			if (m_fMachinegunDelay > 0.4f)
			{
				m_IsShooting = true;
				m_fMachinegunFireDelay += fDeltaTime * m_fOverDrive;
				if (m_fMachinegunFireDelay > 0.10f)
				{
					m_IsFire = true;
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
					m_pManagement->StopSound(CSoundMgr::PLAYER_WEAPON);
					m_pManagement->PlaySound(L"Gatling_Fire_Loop.ogg", CSoundMgr::PLAYER_WEAPON);
					m_fMachinegunFireDelay = 0.f;
				}
			}
			else
			{
				m_IsFire = false;
			}
		}
		else if (m_iWeapon == WEAPON_MISSILE)
		{
			if (!m_IsMissile)
			{
				for (int i = 0; i < 4; ++i)
				{
					switch (i)
					{
					case 0:
						vMissileDir1 = {m_pTransform->Get_State(EState::Right)};
						if (FAILED(m_pManagement->Add_GameObject_InLayer(
							EResourceType::Static,
							L"GameObject_Player_Missile",
							L"Layer_Player_Missile", (void*)&vMissileDir1)))
						{
							PRINT_LOG(L"Error", L"Failed To Add Player_Lazer In Layer");
							return;
						}
						break;
					case 1:
						vMissileDir2 = { m_pTransform->Get_State(EState::Right) * -1.f };
						if (FAILED(m_pManagement->Add_GameObject_InLayer(
							EResourceType::Static,
							L"GameObject_Player_Missile",
							L"Layer_Player_Missile", (void*)&vMissileDir2)))
						{
							PRINT_LOG(L"Error", L"Failed To Add Player_Lazer In Layer");
							return;
						}
						break;
					case 2:
						vMissileDir3 = { m_pTransform->Get_State(EState::Up) };
						if (FAILED(m_pManagement->Add_GameObject_InLayer(
							EResourceType::Static,
							L"GameObject_Player_Missile",
							L"Layer_Player_Missile", (void*)&vMissileDir3)))
						{
							PRINT_LOG(L"Error", L"Failed To Add Player_Lazer In Layer");
							return;
						}
						break;
					case 3:
						vMissileDir4 = { m_pTransform->Get_State(EState::Up) * -1.f };
						if (FAILED(m_pManagement->Add_GameObject_InLayer(
							EResourceType::Static,
							L"GameObject_Player_Missile",
							L"Layer_Player_Missile", (void*)&vMissileDir4)))
						{
							PRINT_LOG(L"Error", L"Failed To Add Player_Lazer In Layer");
							return;
						}
						break;
					}

				}
					m_pManagement->StopSound(CSoundMgr::PLAYER_WEAPON);
					m_pManagement->PlaySound(L"Launch_Missile.ogg", CSoundMgr::PLAYER_WEAPON);
			}
			if (m_pManagement->Get_GameObjectList(L"Layer_Player_Missile")->size() == 4)
				m_IsMissile = true;
			else
				m_IsMissile = false;
		}
	}
	if(m_pController->Key_Up(KEY_LBUTTON))
	{
		m_IsShooting = false;
		m_fMachinegunDelay = 0.f;
		if (m_iWeapon == WEAPON_MACHINEGUN)
		{
			m_pManagement->StopSound(CSoundMgr::PLAYER_WEAPON);
			m_pManagement->PlaySound(L"Gatling_Stop.ogg", CSoundMgr::PLAYER_WEAPON);
		}
	}

	// 마우스 고정시켜서 끄기 불편해서.. ESC키 쓰세용
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		DestroyWindow(g_hWnd);
}

_uint CPlayer::Movement(_float fDeltaTime)
{
	if (m_IsScript == true || m_IsCameraMove) // 대화중이거나 카메라가 움직이는중!
		return 0;

	// 화면 가둬줄 가상의 네모
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	RECT rc;
	POINT p1, p2;

	GetClientRect(g_hWnd, &rc);

	p1.x = rc.left + 300;
	p1.y = rc.top + 300;
	p2.x = rc.right - 300;
	p2.y = rc.bottom - 300;

	ClientToScreen(g_hWnd, &p1);
	ClientToScreen(g_hWnd, &p2);

	rc.left = p1.x;
	rc.top = p1.y;
	rc.right = p2.x;
	rc.bottom = p2.y;

	//ClipCursor(&rc);
	
	_float3 vMouse = { (_float)pt.x, (_float)pt.y, 0.f };
	_float3 vScreenCenter = { WINCX / 2.f, WINCY / 2.f, 0.f };
	_float3 vGap = vMouse - vScreenCenter;

	// 0.05f 플레이어가 얼만큼 더 회전할건지
	_float fSpeed = D3DXVec3Length(&vGap) * 0.15f;
	D3DXVec3Normalize(&vGap, &vGap);

	m_pTransform->RotateX(D3DXToRadian(vGap.y) * fDeltaTime * fSpeed * 0.5f);
	m_pTransform->RotateY(D3DXToRadian(vGap.x) * fDeltaTime * fSpeed * 0.3f);
	return _uint();
}

void CPlayer::TimeOperation(const _float fDeltaTime)
{
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

void CPlayer::Increase_Stamina(const _float fDeltaTime)
{
	if (!m_IsStaminaShrink)
	{
		m_fStaminaIncreaseDelay += fDeltaTime;
		if (m_fStaminaIncreaseDelay >= 1.2f)
		{
			if (m_fStamina < 100.f)
			{
				m_fStamina += 0.2f;
				m_pStamina_Bar->Set_ScaleX(0.2f / m_fFullStamina * m_fStaminaLength);
			}
		}
	}
	else
	{
		m_fStaminaIncreaseDelay = 0.f;
	}
}

void CPlayer::Make_LockOn_Alert(_float fDeltaTime)
{
	if (m_bLockOn)
	{
		if (!m_bFirstLocked)
		{
			CGameObject* pGameObject = nullptr;
			//알림생성
   			UI_DESC LockOnAlert;
			LockOnAlert.tTransformDesc.vPosition = { 800.f, 300.f, 0.f };
			LockOnAlert.tTransformDesc.vScale = { 100.f, 100.f, 0.f };
			LockOnAlert.wstrTexturePrototypeTag = L"Component_Texture_LockOnAlert";
			if (FAILED(m_pManagement->Add_GameObject_InLayer(
				EResourceType::NonStatic,
				L"GameObject_LockOnAlert",
				L"Layer_LockOnAlert",
				&LockOnAlert, &pGameObject)))
			{
				PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
				return;
			}

			m_pLockOnAlert = static_cast<CLockOnAlert*>(pGameObject);
			m_bFirstLocked = true;
		}
	}
	else
	{
		if (m_bFirstLocked && !m_bLockOn)
		{
			m_pManagement->Get_GameObjectList(L"Layer_LockOnAlert")->front()->Set_IsDead(true);
			m_bFirstLocked = false;
		}
	}



}

_uint CPlayer::Collide_Planet_Or_Astroid(const _float fDeltaTime)
{
	// 1.Planet
	CCollisionHandler::Collision_PlayerToObstacle(L"Layer_Player", L"Layer_Planet");
	CCollisionHandler::Collision_PlayerToObstacle(L"Layer_Player", L"Layer_Asteroid");

	// 일반이동 충돌
	if (m_IsBoost == false && m_IsAstroidCollide == true)
	{
		//정면
		if (GetAsyncKeyState(L'W') & 0x8000)
		{
			m_pTransform->Go_Dir(m_pTransform->Get_State(EState::Look), -fDeltaTime * m_fSpeed);
			m_fAfterCollisionDist += fDeltaTime;
			if(m_fAfterCollisionDist > 1.f)
				m_IsAstroidCollide = false;
		}
		// 후진
		if (GetAsyncKeyState(L'S') & 0x8000)
		{
			m_pTransform->Go_Dir(m_pTransform->Get_State(EState::Look), fDeltaTime * m_fSpeed);
			m_fAfterCollisionDist += fDeltaTime;
			if (m_fAfterCollisionDist > 1.f)
				m_IsAstroidCollide = false;
		}
		// 좌측, 우측
		if (GetAsyncKeyState(L'A') & 0x8000)
		{
			m_pTransform->Go_Side(fDeltaTime * 1.2f * m_fSpeed);
			m_fAfterCollisionDist += fDeltaTime;
			if (m_fAfterCollisionDist > 1.f)
				m_IsAstroidCollide = false;
		}
		if (GetAsyncKeyState(L'D') & 0x8000)
		{
			m_pTransform->Go_Side(-fDeltaTime * 1.2f * m_fSpeed);
			m_fAfterCollisionDist += fDeltaTime;
			if (m_fAfterCollisionDist > 1.f)
				m_IsAstroidCollide = false;
		}
	}
	// 부스터 시 충돌
	else if (m_IsBoost == true && m_IsAstroidCollide == true)
	{
		if (GetAsyncKeyState(L'W') & 0x8000)
		{
			m_pTransform->Go_Dir(m_pTransform->Get_State(EState::Look), -fDeltaTime * 2.5f * m_fSpeed);
			m_fAfterCollisionDist += fDeltaTime;
			if (m_fAfterCollisionDist > 1.f)
				m_IsAstroidCollide = false;
		}
		// 후진
		if (GetAsyncKeyState(L'S') & 0x8000)
		{
			m_pTransform->Go_Dir(m_pTransform->Get_State(EState::Look), fDeltaTime * 1.f * m_fSpeed);
			m_fAfterCollisionDist += fDeltaTime;
			if (m_fAfterCollisionDist > 1.f)
				m_IsAstroidCollide = false;
		}
		if (GetAsyncKeyState(L'A') & 0x8000)
		{
			m_pTransform->Go_Side(fDeltaTime * 1.2f * m_fSpeed);
			m_fAfterCollisionDist += fDeltaTime;
			if (m_fAfterCollisionDist > 1.f)
				m_IsAstroidCollide = false;
		}
		if (GetAsyncKeyState(L'D') & 0x8000)
		{
			m_pTransform->Go_Side(-fDeltaTime * 1.2f * m_fSpeed);
			m_fAfterCollisionDist += fDeltaTime;
			if (m_fAfterCollisionDist > 1.f)
				m_IsAstroidCollide = false;
		}
	
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
	if (m_pLeftEngineEffect) {
		m_pLeftEngineEffect->Set_IsDead(true);
		m_pLeftEngineEffect = nullptr;
	}
	if (m_pRightEngineEffect) {
		m_pRightEngineEffect->Set_IsDead(true);
		m_pRightEngineEffect = nullptr;
	}

	if (m_pLeftWingBoost) {
		m_pLeftWingBoost->Set_IsDead(true);
		m_pLeftWingBoost = nullptr;
	}
	if (m_pRightWingBoost) {
		m_pRightWingBoost->Set_IsDead(true);
		m_pLeftWingBoost = nullptr;
	}

	Safe_Release(m_pLockOnAlert);
	if (m_pHUD_Effect_Boost)
	{
		m_pHUD_Effect_Boost->Set_IsDead(true);
		Safe_Release(m_pHUD_Effect_Boost);
	}
	Safe_Release(m_pStamina_Bar);
	Safe_Release(m_pHp_Bar);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pController);

	CGameObject::Free();
}

_uint CPlayer::Make_Arrow()
{
	if (nullptr == m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster") ||
		nullptr == m_pManagement->Get_GameObjectList(L"Layer_Sniper") ||
		nullptr == m_pManagement->Get_GameObjectList(L"Layer_AlertArrow"))
		return NO_EVENT;
	
	// 나중에는 모든 몬스터 순회하면서 검사해야함.
	// 몬스터 pos - 플레이어 pos. => 
	// 각도로 비교하자~


	if (m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster")->size() == 0 
		||m_pManagement->Get_GameObjectList(L"Layer_Sniper")->size() == 0)
		return NO_EVENT;

	//Test
	m_pTransform->Get_TransformDesc().matWorld;
	_float3 vPlayerLook = m_pTransform->Get_State(EState::Look);

	m_listCheckMonsters = m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster");
	m_listCheckSnipers = m_pManagement->Get_GameObjectList(L"Layer_Sniper");
	if (nullptr == m_listCheckMonsters 
		|| nullptr == m_listCheckSnipers 
		|| m_listCheckMonsters->size() == 0 
		|| m_listCheckSnipers->size() == 0) return NO_EVENT;

	// Boss_Monster
	auto& iter = m_listCheckMonsters->begin();

	for (; iter != m_listCheckMonsters->end(); ++iter)
	{
		_float3 v1 = vPlayerLook; 
		_float3 v2 = (*iter)->Get_Collides()->front()->Get_BoundingSphere().Get_Position() - m_pTransform->Get_State(EState::Position); 
		_float fCeta;
		D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
		_float v1v2 = D3DXVec3Dot(&v1, &v2);
		_float v1Length = D3DXVec3Length(&v1);
		_float v2Length = D3DXVec3Length(&v2);
		fCeta = acosf(v1v2 / (v1Length * v2Length));
		
		_float fDegree = D3DXToDegree(fCeta);

		if (fabs(fDegree) > 90.f)
		{
			if (IsArrow == false)
			{
				//wstring abc = to_wstring(fDegree);
				//PRINT_LOG(L"", abc.c_str());
				m_pManagement->Add_GameObject_InLayer(EResourceType::Static, L"GameObject_AlertArrow", L"Layer_AlertArrow", (void*)(*iter));
				IsArrow = true;
			}
		}
		else if (fabs(fDegree) < 70.f)
		{
			if (IsArrow && m_pManagement->Get_GameObjectList(L"Layer_AlertArrow")->size() != 0)
			{
				m_pManagement->Get_GameObjectList(L"Layer_AlertArrow")->front()->Set_IsDead(TRUE);
				IsArrow = false;
			}
		}
	}
	// Snipers 
	auto& iter2 = m_listCheckSnipers->begin();

	for (; iter2 != m_listCheckSnipers->end(); ++iter2)
	{
		_float3 v1 = vPlayerLook; 
		_float3 v2 = (*iter2)->Get_Collides()->front()->Get_BoundingSphere().Get_Position() - m_pTransform->Get_State(EState::Position);
		_float fCeta;
		D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
		_float v1v2 = D3DXVec3Dot(&v1, &v2);
		_float v1Length = D3DXVec3Length(&v1);
		_float v2Length = D3DXVec3Length(&v2);
		fCeta = acosf(v1v2 / (v1Length * v2Length));

		_float fDegree = D3DXToDegree(fCeta);

		// 각도가 이만큼 넘으면 화면밖에있음
		if (fabs(fDegree) > 90.f)
		{
			if (IsArrow2 == false)
			{
				//wstring abc = to_wstring(fDegree);
				//PRINT_LOG(L"", abc.c_str());
				m_pManagement->Add_GameObject_InLayer(EResourceType::Static, L"GameObject_AlertArrow", L"Layer_AlertArrow", (void*)(*iter2));
				IsArrow2 = true;
			}
		}
		else if (fabs(fDegree) < 70.f)
		{
			if (IsArrow2 && m_pManagement->Get_GameObjectList(L"Layer_AlertArrow")->size() != 0)
			{
				m_pManagement->Get_GameObjectList(L"Layer_AlertArrow")->front()->Set_IsDead(TRUE);
				IsArrow2 = false;
			}
		}
	}

	


	return _uint();
}
