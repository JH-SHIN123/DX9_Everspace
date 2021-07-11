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

	//float randRotateSpeed = rand() % 5 + 10.f;
	//TransformDesc.fRotatePerSec = D3DXToRadian(randRotateSpeed);
	//TransformDesc.fSpeedPerSec = 4.f;

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
	m_eNextState = State::Research;
	m_vCreatePosition = TransformDesc.vPosition;
	m_vResearchRange = { 50.f,50.f,50.f };

	m_fHp = 900.f;
	m_fFullHp = m_fHp; 

	m_fAngle = CPipeline::GetRandomFloat(0.1f, 1.f);
	_float Check = CPipeline::GetRandomFloat(0.f, 1.f);
	m_bAnglePlus = false;
	if (Check > 0.5f)
		m_bAnglePlus = true;
	

	return S_OK;
}

_uint CDrone::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	Movement_Common(fDeltaTime);
	Movement(fDeltaTime);
	StateCheck();

	//Add_Hp_Bar(fDeltaTime);

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
		//m_pHp_Bar->Set_IsDead(TRUE);
		//m_pHP_Bar_Border->Set_IsDead(TRUE);
		m_pManagement->PlaySound(L"Ship_Explosion.ogg", CSoundMgr::SHIP_EXPLOSION);
		return DEAD_OBJECT;
	}
	if (m_IsCollide) {
		// Bullet 데미지 만큼.
		//m_pHp_Bar->Set_ScaleX(-100.f / m_fFullHp * m_fHpLength);
		m_fHp -= 100.f;
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
	return _uint();
}

_uint CDrone::Movement(_float fDeltaTime)
{

	switch (m_eCurState)
	{
	case State::Idle:
		break;
	case State::Move:
		break;
	case State::Research:
		Researching(fDeltaTime);
		break;
	case State::Die:
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

_uint CDrone::Movement_Common(_float fDeltaTime)
{
	_float3 vUp = m_pTransform->Get_State(EState::Up);
	_float3 vPos = m_pTransform->Get_State(EState::Position);

	m_fAngle -= 2.f;
	if(m_bAnglePlus == true)
		m_fAngle += 4.f;

	_float fSin = (sinf(D3DXToRadian(m_fAngle)) * 0.5f);
	vUp *= fSin;
	vPos += vUp;

	m_pTransform->Set_Position(vPos);

	return _uint();
}

void CDrone::StateCheck()
{
	if (m_eCurState != m_eNextState)
	{
		switch (m_eNextState)
		{
		case State::Research:
			break;
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
	//Safe_Release(m_pHP_Bar_Border);
	//Safe_Release(m_pHp_Bar);
	Safe_Release(m_pTargetTransform);

	Safe_Release(m_pModelMesh);
	Safe_Release(m_pTransform);

	CGameObject::Free();
}
