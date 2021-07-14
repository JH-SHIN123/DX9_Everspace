#include "stdafx.h"
#include "Drone.h"
#include "HP_Bar.h"
#include "HP_Bar_Border.h"
#include "Pipeline.h"

CDrone::CDrone(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CDrone::CDrone(const CDrone& other)
	: CGameObject(other)
	, m_fDis(other.m_fDis)
	, m_fTurnSec(other.m_fTurnSec)
{
}

HRESULT CDrone::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CDrone::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);

	GAMEOBJECT_DESC* pDesc = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (pDesc = dynamic_cast<GAMEOBJECT_DESC*>(ptr))
		{
		}
		else
		{
			PRINT_LOG(L"Error", L"ASEROID_DESC is nullptr");
			return E_FAIL;
		}
	}

	// For.Com_VIBuffer
	wstring meshTag = L"Component_Mesh_Drone";
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		meshTag,
		L"Com_Mesh",
		(CComponent**)&m_pModelMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc = pDesc->tTransformDesc;
	TransformDesc.vScale = _float3(2.f, 2.f, 2.f);
	m_vSpawnPos = pDesc->tTransformDesc.vPosition;

	//float randRotateSpeed = rand() % 5 + 10.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(30.f);
	TransformDesc.fSpeedPerSec = 7.f;

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
	PASSDATA_COLLIDE tCollide;
	CStreamHandler::Load_PassData_Collide(L"drone", meshTag, tCollide);
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

	m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pTargetTransform);
	if (nullptr == m_pTargetTransform)
	{
		PRINT_LOG(L"Error", L"m_pTargetTransform is nullptr");
		return E_FAIL;
	}

	// Init
	//m_eNextState = State::Research;
	//m_vCreatePosition = TransformDesc.vPosition;
	//m_vResearchRange = { 50.f,50.f,50.f };

	m_fAngle = CPipeline::GetRandomFloat(0.1f, 1.f);
	_float Check = CPipeline::GetRandomFloat(0.f, 1.f);
	
	// 상하운동 
	if (Check > 0.5f)
	{
		m_eNextState = Idle;
		Check = CPipeline::GetRandomFloat(0.f, 1.f);

		m_bAnglePlus = false; // 아래부터? 위부터?
		if (Check > 0.5f)
			m_bAnglePlus = true;
	}
	
	else // 랜덤 이동
	{
		m_eNextState = Turn;
	}

	// HP 세팅
	m_fHp = 100.f;
	m_fFullHp = m_fHp;

	return S_OK;
}

_uint CDrone::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	StateCheck();
	Movement(fDeltaTime);
	
	_float3 vDir = m_pTargetTransform->Get_State(EState::Position) - m_pTransform->Get_State(EState::Position);
	_float fDist = D3DXVec3Length(&vDir);
	
	if (fDist < 400.f)
	{
		if (!m_IsHPBar)
		{
			Add_Hp_Bar(fDeltaTime);
			m_IsHPBar = true;
		}
	}
	if (m_pHp_Bar != nullptr && m_pHP_Bar_Border != nullptr)
	{

		Set_Hp_Pos();
		Check_Degree();
	}

	m_pTransform->Update_Transform();
	for (auto& p : m_Collides)
	{
		if(p) p->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	}
	return NO_EVENT;
}

_uint CDrone::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	if (m_fHp <= 0.f)
	{
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);
		m_IsDead = true;
		if(m_pHp_Bar)
			m_pHp_Bar->Set_IsDead(TRUE);
		if (m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_IsDead(TRUE);
		m_pManagement->PlaySound(L"Ship_Explosion.ogg", CSoundMgr::SHIP_EXPLOSION);
		return DEAD_OBJECT;
	}
	if (m_IsCollide) {
		// Bullet 데미지 만큼.
		if (m_pHp_Bar)
			m_pHp_Bar->Set_ScaleX(-30.f / m_fFullHp * m_fHpLength);
		m_fHp -= 30.f;
		m_IsCollide = false;
	}


	return _uint();
}

_uint CDrone::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pModelMesh->Render_Mesh();
	// Test

#ifdef _DEBUG // Render Collide
	for (auto& p : m_Collides)
		if (p) p->Render_Collide();
#endif

	return _uint();
}

_uint CDrone::State_Idle(_float fDelataTime)
{
	_float3 vUp = m_pTransform->Get_State(EState::Up);
	_float3 vPos = m_pTransform->Get_State(EState::Position);
	D3DXVec3Normalize(&vUp, &vUp);
	m_fAngle -= 2.f;
	if (m_bAnglePlus == true)
		m_fAngle += 4.f;

	_float fSin = (sinf(D3DXToRadian(m_fAngle)) * 0.1f);
	vUp *= fSin;
	vPos += vUp;

	m_pTransform->Set_Position(vPos);

	return _uint();
}

_uint CDrone::State_Turn(_float fDelataTime)
{
	if (m_fTurnSec == 0.f)
		m_fTurnSec = CPipeline::GetRandomFloat(0.2f, 2.f);

	m_fTurnSec -= fDelataTime;
	m_pTransform->RotateY(fDelataTime);

	if (m_fTurnSec <= 0.f)
	{
		m_fTurnSec = 0.f;
		m_eNextState = Move;
	}

	return _uint();
}

_uint CDrone::State_Move(_float fDelataTime)
{
	_float3 vPos = m_pTransform->Get_State(EState::Position);
	m_pTransform->Go_Straight(fDelataTime);

	_float fDis = D3DXVec3Length(&(m_vSpawnPos - vPos));

	if (fDis >= m_fDis)
	{
		m_eNextState = BackTurn;
	}

	return _uint();
}

_uint CDrone::State_BackTurn(_float fDelataTime)
{
	m_pTransform->Go_Straight(fDelataTime);

	// 각을 구함
	_float3 vMyPos = m_pTransform->Get_State(EState::Position);

	_float3 vTargetDir = m_vSpawnPos - vMyPos;
	D3DXVec3Normalize(&vTargetDir, &vTargetDir);

	_float3 vMyLook = m_pTransform->Get_State(EState::Look);
	_float3 vMyUp = m_pTransform->Get_State(EState::Up);
	D3DXVec3Normalize(&vMyLook, &vMyLook);

	_float fCeta = D3DXVec3Dot(&vTargetDir, &vMyLook);
	_float fRadianMax = D3DXToRadian(95.f);
	_float fRadianMin = D3DXToRadian(85.f);

	_float3 vMyRight, vMyLeft;
	D3DXVec3Cross(&vMyRight, &vMyUp, &vMyLook);
	D3DXVec3Cross(&vMyLeft, &vMyLook, &vMyUp);

	D3DXVec3Normalize(&vMyRight, &vMyRight);
	D3DXVec3Normalize(&vMyLeft, &vMyLeft);

	_float fRight = D3DXVec3Dot(&vTargetDir, &vMyRight);
	_float fLeft = D3DXVec3Dot(&vTargetDir, &vMyLeft);

	//if (fCeta < fRadianMin)
	//{
	if (fRight < fLeft)
		m_pTransform->RotateY(-fDelataTime);

	else
		m_pTransform->RotateY(fDelataTime);
	//}



	if (D3DXVec3Length(&vTargetDir) <= 0.5f)
	{
		m_pTransform->Set_Position(m_vSpawnPos);
		m_eNextState = Turn;
	}

	return _uint();
}

_uint CDrone::State_BackMove(_float fDelataTime)
{
	return _uint();
}

_uint CDrone::Movement(_float fDeltaTime)
{
	switch (m_eCurState)
	{
	case CDrone::Idle:
		State_Idle(fDeltaTime);
		break;
	case CDrone::Turn:
		State_Turn(fDeltaTime);
		break;
	case CDrone::Move:
		State_Move(fDeltaTime);
		break;
	case CDrone::BackTurn:
		State_BackTurn(fDeltaTime);
		break;
	case CDrone::BackMove:
		State_BackMove(fDeltaTime);
		break;

	default:
		break;
	}


	return _uint();
}

_uint CDrone::Researching(_float fDeltaTime)
{
	// if 범위보다 벗어났다. -> Create Pos로 돌아가기


	return _uint();
}

void CDrone::StateCheck()
{
	if (m_eCurState != m_eNextState)
	{
		switch (m_eNextState)
		{
		//case State::Research:
		//	break;
		case State::Die:
			break;
		}
		m_eCurState = m_eNextState;
	}
}

//_uint CDrone::Add_Hp_Bar(_float fDeltaTime)
//{
//	if (m_IsHPBar) return -1;
//
//	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
//	_float3 vPlayerPos = m_pTargetTransform->Get_State(EState::Position);
//
//	_float3 vDir = vMonsterPos - vPlayerPos;
//	_float fDist = D3DXVec3Length(&vDir);
//
//	if (fDist < 300.f)
//	{
//		if (m_IsHPBar == false)
//		{
//			//////////////////3d좌표를 2d좌표로////////////////////////////
//			D3DVIEWPORT9 vp2;
//			m_pDevice->GetViewport(&vp2);
//			_float4x4 TestView2, TestProj2;
//			m_pDevice->GetTransform(D3DTS_VIEW, &TestView2);
//			m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj2);
//			_float4x4 matCombine2 = TestView2 * TestProj2;
//			D3DXVec3TransformCoord(&vMonsterPos, &vMonsterPos, &matCombine2);
//			vMonsterPos.x += 1.f;
//			vMonsterPos.y += 1.f;
//
//			vMonsterPos.x = (vp2.Width * (vMonsterPos.x)) / 2.f + vp2.X;
//			vMonsterPos.y = (vp2.Height * (2.f - vMonsterPos.y) / 2.f + vp2.Y);
//
//			_float3 ptBoss;
//			ptBoss.x = -1.f * (WINCX / 2) + vMonsterPos.x;
//			ptBoss.y = 1.f * (WINCY / 2) + vMonsterPos.y;
//			ptBoss.z = 0.f;
//			//////////////////////////////////////////////////////////////////
//			// 감지범위에 들어오게 되면 HP_Bar 생성!
//			_float2 offset = { -64.f ,-50.f };
//
//			CGameObject* pGameObject = nullptr;
//			UI_DESC HUD_Hp_Bar;
//			HUD_Hp_Bar.tTransformDesc.vPosition = { ptBoss.x + offset.x, ptBoss.y + offset.y , 0.f };
//			HUD_Hp_Bar.tTransformDesc.vScale = { m_fHp * (m_fHpLength / m_fFullHp), 8.f, 0.f };
//			HUD_Hp_Bar.wstrTexturePrototypeTag = L"Component_Texture_HP_Bar";
//			if (FAILED(m_pManagement->Add_GameObject_InLayer(
//				EResourceType::NonStatic,
//				L"GameObject_HP_Bar",
//				L"Layer_HP_Bar",
//				&HUD_Hp_Bar, &pGameObject)))
//			{
//				PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
//				return E_FAIL;
//			}
//
//			CGameObject* pGameObjectBorder = nullptr;
//			UI_DESC HUD_Hp_Bar_Border;
//			HUD_Hp_Bar_Border.tTransformDesc.vPosition = { ptBoss.x + offset.x , ptBoss.y + offset.y, 0.f };
//			HUD_Hp_Bar_Border.tTransformDesc.vScale = { m_fHp * (m_fHpLength / m_fFullHp) + 2.5f, 12.f, 0.f };
//			HUD_Hp_Bar_Border.wstrTexturePrototypeTag = L"Component_Texture_HP_Border";
//			if (FAILED(m_pManagement->Add_GameObject_InLayer(
//				EResourceType::NonStatic,
//				L"GameObject_HP_Bar_Border",
//				L"Layer_HP_Bar_Border",
//				&HUD_Hp_Bar_Border, &pGameObjectBorder)))
//			{
//				PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
//				return E_FAIL;
//			}
//			m_IsHPBar = true;
//
//			m_pHP_Bar_Border = static_cast<CHP_Bar_Border*>(pGameObjectBorder);
//			m_pHP_Bar_Border->Who_Make_Me(m_pHP_Bar_Border->MAKER_DRONE);
//
//			m_pHp_Bar = static_cast<CHP_Bar*>(pGameObject);
//			m_pHp_Bar->Who_Make_Me(m_pHp_Bar->MAKER_DRONE);
//		}
//
//	}
//	return _uint();
//}

CDrone* CDrone::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDrone* pInstance = new CDrone(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDrone::Clone(void* pArg)
{
	CDrone* pClone = new CDrone(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CDrone::Free()
{
	Safe_Release(m_pHP_Bar_Border);
	Safe_Release(m_pHp_Bar);
	Safe_Release(m_pTargetTransform);

	Safe_Release(m_pModelMesh);
	Safe_Release(m_pTransform);

	CGameObject::Free();
}

_uint CDrone::Add_Hp_Bar(_float fDeltaTime)
{
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	_float3 vPlayerPos = m_pTargetTransform->Get_State(EState::Position);

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

void CDrone::Set_Hp_Pos()
{
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	_float3 vPlayerPos = m_pTargetTransform->Get_State(EState::Position);

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

	_float3 vPosition = { ptBoss.x - (WINCX / 2.f) - 29.f, -ptBoss.y + (WINCY / 2.f) + 30.f, 0.f };
	//_float3 vPosition = { 0.f, 0.f, 0.f };

	m_pHp_Bar->Set_Pos(vPosition);
	m_pHP_Bar_Border->Set_Pos(vPosition);
}

_uint CDrone::Check_Degree()
{
	_float3 vPlayerLook = m_pTargetTransform->Get_State(EState::Look);
	_float3 v1 = vPlayerLook;
	_float3 v2 = m_pTransform->Get_State(EState::Position) - m_pTargetTransform->Get_State(EState::Position);
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
