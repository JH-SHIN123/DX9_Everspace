#include "stdafx.h"
#include "..\Headers\Sniper.h"
#include "HP_Bar.h"
#include "HP_Bar_Border.h"
#include "Player.h"

CSniper::CSniper(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CSniper::CSniper(const CSniper & other)
	: CGameObject(other)
{
}

HRESULT CSniper::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CSniper::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_Enemy2",
		L"Com_Mesh",
		(CComponent**)&m_pModelMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = _float3(600.5f, 0.f, 600.5f);	
	TransformDesc.vScale = _float3(5.f, 5.f, 5.f);
	TransformDesc.fSpeedPerSec = 40.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(60.f);

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
	BoundingSphere.fRadius = 4.f;

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


	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);

	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"Player Transform is nullptr");
		return E_FAIL;
	}

	// HP 세팅
	m_fHp = 1000.f;
	m_fFullHp = m_fHp;

	return S_OK;
}

_uint CSniper::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	
	//배틀모드가 아닐때는 그냥 돌아다니렴.
	if (!m_IsBattle)
		Movement(fDeltaTime);
	else
		Sniper_Battle(fDeltaTime);

	if (m_pHp_Bar != nullptr && m_pHP_Bar_Border != nullptr)
	{
		Set_Hp_Pos();
		Check_Degree();
	}

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CSniper::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_fHp <= 0.f)
	{
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);
		m_IsDead = true;
		m_pHp_Bar->Set_IsDead(TRUE);
		m_pHP_Bar_Border->Set_IsDead(TRUE);
		m_pManagement->PlaySound(L"Ship_Explosion.ogg", CSoundMgr::SHIP_EXPLOSION);
		((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Someone_Try_To_Kill_Me(false);
		return DEAD_OBJECT;
	}
	if (m_IsCollide) {
		// Bullet 데미지 만큼.
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);
		m_pHp_Bar->Set_ScaleX(-100.f / m_fFullHp * m_fHpLength);
		m_fHp -= 100.f;
		m_IsCollide = false;
	}

	return _uint();
}

_uint CSniper::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pModelMesh->Render_Mesh(); 

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CSniper::Movement(_float fDeltaTime)
{
	//최초에 가만히 있을래 아니면 돌아다닐래? -> 돌아다니자 ㅋㅋ
	m_pTransform->Go_Straight(fDeltaTime);
	m_pTransform->RotateY(fDeltaTime);

	// 플레이어와의 거리를 계속 계산해서 감지할지 말지 여부를 정함!
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_float3 vSniperPos = m_pTransform->Get_State(EState::Position);

	_float3 vDir = (vPlayerPos - vSniperPos);
	_float vDist = D3DXVec3Length(&vDir);

	// 배틀 상태 On
	if (vDist <= 400.f && vDist != 0.f)
	{
		m_IsBattle = true;
		Add_Hp_Bar(fDeltaTime);
	}
	return _uint();
}


_uint CSniper::Sniper_Battle(_float fDeltaTime)
{
	// 플레이어 쪽으로 회전해! -> OK!
	RotateToPlayer(fDeltaTime);

	// 플레이어쪽을 바라보고 있으면 락온시작! 그게아니면 계속 회전이나 하렴
	
	if (m_IsLookingPlayer)
	{
		Lock_On(fDeltaTime);
	}
	else
		return _uint();


	return _uint();
}

_uint CSniper::Lock_On(_float fDeltaTime)
{
	// 락온 시작하면 락온 되었다는걸 알리기 위해서 플레이어로 향하는 레이저를 발사해야 할까?..
	// 아니면 플레이어 HUD에서 퉁쳐야하나?
	m_IsLockOn = true;
	m_fLockOnDelay += fDeltaTime;
	if (m_fLockOnDelay >= 2.f)
	{
		((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Someone_Try_To_Kill_Me(true);

		m_fSniperShootDelay += fDeltaTime;
		// 락온을 4초동안 한다음에 투사체 하나 발사하자
		if (m_fSniperShootDelay >= 4.f)
		{
			TRANSFORM_DESC* pArg = new TRANSFORM_DESC;

			pArg->vPosition = m_pTransform->Get_State(EState::Position);
			pArg->vRotate = m_pTransform->Get_TransformDesc().vRotate;

			if (FAILED(m_pManagement->Add_GameObject_InLayer(
				EResourceType::NonStatic,
				L"GameObject_Sniper_Bullet",
				L"Layer_Sniper_Bullet", pArg)))
			{
				PRINT_LOG(L"Error", L"Failed To Add GameObject_Sniper_Bullet In Layer");
				return E_FAIL;
			}
			m_fSniperShootDelay = 0.f;
			m_fLockOnDelay = 0.f;
			((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Someone_Try_To_Kill_Me(false);
		}
	}

	return _uint();
}

_uint CSniper::Add_Hp_Bar(_float fDeltaTime)
{
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);

	_float3 vDir = vMonsterPos - vPlayerPos;
	_float fDist = D3DXVec3Length(&vDir);


	if (m_IsHPBar == false)
	{
		//////////////////3d좌표를 2d좌표로////////////////////////////
		D3DVIEWPORT9 vp2;
		m_pDevice->GetViewport(&vp2);
		_float4x4 TestView2, TestProj2;
		m_pDevice->GetTransform(D3DTS_VIEW, &TestView2);
		m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj2);
		_float4x4 matCombine2 = TestView2 * TestProj2;
		D3DXVec3TransformCoord(&vMonsterPos, &vMonsterPos, &matCombine2);
		vMonsterPos.x += 1.f;
		vMonsterPos.y += 1.f;

		vMonsterPos.x = (vp2.Width * (vMonsterPos.x)) / 2.f + vp2.X;
		vMonsterPos.y = (vp2.Height * (2.f - vMonsterPos.y) / 2.f + vp2.Y);

		_float3 ptBoss;
		ptBoss.x = -1.f * (WINCX / 2) + vMonsterPos.x;
		ptBoss.y = 1.f * (WINCY / 2) + vMonsterPos.y;
		ptBoss.z = 0.f;
		//////////////////////////////////////////////////////////////////
		// 감지범위에 들어오게 되면 HP_Bar 생성!

		CGameObject* pGameObject = nullptr;
		UI_DESC HUD_Hp_Bar;
		HUD_Hp_Bar.tTransformDesc.vPosition = { ptBoss.x - 64.f, ptBoss.y - 50.f, 0.f };
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

		CGameObject* pGameObjectBorder = nullptr;
		UI_DESC HUD_Hp_Bar_Border;
		HUD_Hp_Bar_Border.tTransformDesc.vPosition = { ptBoss.x - 64.f, ptBoss.y - 50.f, 0.f };
		HUD_Hp_Bar_Border.tTransformDesc.vScale = { m_fHp * (m_fHpLength / m_fFullHp) + 2.5f, 12.f, 0.f };
		HUD_Hp_Bar_Border.wstrTexturePrototypeTag = L"Component_Texture_HP_Border";
		if (FAILED(m_pManagement->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_HP_Bar_Border",
			L"Layer_HP_Bar_Border",
			&HUD_Hp_Bar_Border, &pGameObjectBorder)))
		{
			PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
			return E_FAIL;
		}
		m_IsHPBar = true;

		m_pHP_Bar_Border = static_cast<CHP_Bar_Border*>(pGameObjectBorder);
		m_pHP_Bar_Border->Who_Make_Me(m_pHP_Bar_Border->MAKER_SNIPER);

		m_pHp_Bar = static_cast<CHP_Bar*>(pGameObject);
		m_pHp_Bar->Who_Make_Me(m_pHp_Bar->MAKER_SNIPER);

	}
	return _uint();
}

void CSniper::Set_Hp_Pos()
{
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);

	_float3 vDir = vMonsterPos - vPlayerPos;
	_float fDist = D3DXVec3Length(&vDir);

	D3DVIEWPORT9 vp2;
	m_pDevice->GetViewport(&vp2);
	_float4x4 TestView2, TestProj2;
	m_pDevice->GetTransform(D3DTS_VIEW, &TestView2);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj2);
	_float4x4 matCombine2 = TestView2 * TestProj2;
	D3DXVec3TransformCoord(&vMonsterPos, &vMonsterPos, &matCombine2);
	vMonsterPos.x += 1.f;
	vMonsterPos.y += 1.f;

	vMonsterPos.x = (vp2.Width * (vMonsterPos.x)) / 2.f + vp2.X;
	vMonsterPos.y = (vp2.Height * (2.f - vMonsterPos.y) / 2.f + vp2.Y);

	_float3 ptBoss;
	ptBoss.x = vMonsterPos.x;
	ptBoss.y = vMonsterPos.y;
	ptBoss.z = 0.f;
	//////////////////////////////////////////////////////////////////

	_float3 vPosition = { ptBoss.x - (WINCX / 2.f) - 30.f, -ptBoss.y + (WINCY / 2.f) + 30.f, 0.f };
	//_float3 vPosition = { 0.f, 0.f, 0.f };

	m_pHp_Bar->Set_Pos(vPosition);
	m_pHP_Bar_Border->Set_Pos(vPosition);
}


CSniper * CSniper::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CSniper* pInstance = new CSniper(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create CSniper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSniper::Clone(void * pArg/* = nullptr*/)
{
	CSniper* pClone = new CSniper(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CSniper");
		Safe_Release(pClone);
	}

	return pClone;
}

void CSniper::Free()
{
	Safe_Release(m_pHp_Bar);
	Safe_Release(m_pHP_Bar_Border);
	Safe_Release(m_pModelMesh);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}

_uint CSniper::Check_Degree()
{
	_float3 vPlayerLook = m_pPlayerTransform->Get_State(EState::Look);
	_float3 v1 = vPlayerLook;
	_float3 v2 = m_pTransform->Get_State(EState::Position) - m_pPlayerTransform->Get_State(EState::Position);
	_float fCeta;
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
	_float v1v2 = D3DXVec3Dot(&v1, &v2);
	_float v1Length = D3DXVec3Length(&v1);
	_float v2Length = D3DXVec3Length(&v2);
	fCeta = acosf(v1v2 / (v1Length * v2Length));

	_float fDegree = D3DXToDegree(fCeta);

	if (v2.x < 0)
	{
		if (fDegree > 90.f)
		{
			m_pHp_Bar->Set_IsBack(true);
			m_pHP_Bar_Border->Set_IsBack(true);
		}
		else if (fDegree <= 90.f)
		{
			m_pHp_Bar->Set_IsBack(false);
			m_pHP_Bar_Border->Set_IsBack(false);
		}
	}
	else
	{
		if (fDegree > 90.f)
		{
			m_pHp_Bar->Set_IsBack(true);
			m_pHP_Bar_Border->Set_IsBack(true);
		}
		else if (fDegree <= 90.f)
		{
			m_pHp_Bar->Set_IsBack(false);
			m_pHP_Bar_Border->Set_IsBack(false);
		}
	}
	return _uint();
}

_bool CSniper::RotateToPlayer(_float fDeltaTime)
{
	_float3 vTargetPos = m_pPlayerTransform->Get_State(EState::Position);
	_float3 vSniperPos = m_pTransform->Get_State(EState::Position);

	_float3 vTargetDir = vTargetPos - vSniperPos;
	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float3 vSniperLook = m_pTransform->Get_State(EState::Look);
	_float3 vSniperUp = m_pTransform->Get_State(EState::Up);
	D3DXVec3Normalize(&vSniperLook, &vSniperLook);

	_float fCeta = D3DXVec3Dot(&vTargetDir, &vSniperLook);
	_float fRadianMax = D3DXToRadian(95.f);
	_float fRadianMin = D3DXToRadian(85.f);

	_float3 vMyRight, vMyLeft, vMissileUp, vMissileDown;
	D3DXVec3Cross(&vMyRight, &vSniperUp, &vSniperLook);
	D3DXVec3Cross(&vMyLeft, &vSniperLook, &vSniperUp);
	D3DXVec3Cross(&vMissileUp, &vMyRight, &vSniperLook);
	D3DXVec3Cross(&vMissileDown, &vSniperLook, &vMyRight);

	D3DXVec3Normalize(&vMyRight, &vMyRight);
	D3DXVec3Normalize(&vMyLeft, &vMyLeft);
	D3DXVec3Normalize(&vMissileUp, &vMissileUp);
	D3DXVec3Normalize(&vMissileDown, &vMissileDown);

	_float fRight = D3DXVec3Dot(&vTargetDir, &vMyRight);
	_float fLeft = D3DXVec3Dot(&vTargetDir, &vMyLeft);
	_float fUp = D3DXVec3Dot(&vTargetDir, &vMissileUp);
	_float fDown = D3DXVec3Dot(&vTargetDir, &vMissileDown);

	
	if (fRight < fLeft)
		m_pTransform->RotateY(-fDeltaTime);
	else
		m_pTransform->RotateY(fDeltaTime);

	if (fUp < fDown)
		m_pTransform->RotateX(-fDeltaTime);
	else
		m_pTransform->RotateX(fDeltaTime);

	if (fabs(fRight - fLeft) < 10.f || fabs(fUp - fDown) < 10.f)
	{
		m_IsLookingPlayer = true;
	}
	else
		m_IsLookingPlayer = false;

	return m_IsLookingPlayer;
}
