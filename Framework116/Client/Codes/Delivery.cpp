#include "stdafx.h"
#include "Delivery.h"
#include "EngineEffectSystem.h"
#include "HP_Bar.h"
#include "HP_Bar_Border.h"
#include "Player.h"
#include "Collision.h"
#include "LockOnAlert.h"

CDelivery::CDelivery(LPDIRECT3DDEVICE9 pDevice) :
    CGameObject(pDevice)
{
}

CDelivery::CDelivery(const CDelivery& other)
    : CGameObject(other)
	, m_IsStart(other.m_IsStart)
{
}

HRESULT CDelivery::Ready_GameObject_Prototype()
{
    CGameObject::Ready_GameObject_Prototype();

    return S_OK;
}

HRESULT CDelivery::Ready_GameObject(void* pArg)
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
            PRINT_LOG(L"Error", L"CBrokenPlane is nullptr");
            return E_FAIL;
        }
    }

    // For.Com_Mesh
    if (FAILED(CGameObject::Add_Component(
        EResourceType::Static,
        L"Component_Mesh_Delivery",
        L"Com_Mesh",
        (CComponent**)&m_pMesh)))
    {
        PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
        return E_FAIL;
    }

    // For.Com_Transform
    TRANSFORM_DESC TransformDesc = pDesc->tTransformDesc;
	TransformDesc.fSpeedPerSec = 60.f;//30.f;
    TransformDesc.fSpeedPerSec = 30.f;//30.f;

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
	 
	// 플레이어 트랜스폼
	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);

	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"Player Transform is nullptr");
		return E_FAIL;
	}

    // For.Com_Collide
    PASSDATA_COLLIDE tCollide;
    CStreamHandler::Load_PassData_Collide(L"delivery", L"Component_Mesh_Delivery", tCollide);
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

    // 네비 경로 지정
    CStreamHandler::Get_PassData_Navi(m_vecNaviRoute, L"../../Resources/Data/Navi/stage3.navi");

    // 네비 첫번째 위치와 방향 세팅
    FindNextRoute();

    // Add Engine-Boost Effect
    CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pLeftEngineEffect);
    m_vLeftEngineOffset = { -3.7f, 0.f, -5.f };
    CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pRightEngineEffect);
    m_vRightEngineOffset = { 3.7f, 0.f, -5.f };

    m_pLeftEngineEffect->Set_SizeRate(9.f);
    m_pRightEngineEffect->Set_SizeRate(9.f);

	// HP 세팅
	//m_fHp = 1000.f;
	//m_fFullHp = m_fHp;
	STAT_INFO tStatus;
	tStatus.iMaxHp = 5000;
	tStatus.iHp = tStatus.iMaxHp;
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Status_Info",
		L"Com_StatInfo",
		(CComponent**)&m_pInfo,
		&tStatus)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}
//#ifdef _DEBUG
//    WritePrivateProfileString(L"Section_1", L"Key_1", L"1.4f", L"../test.ini");
//    WritePrivateProfileString(L"Section_1", L"Key_2", L"0.9f", L"../test.ini");
//    WritePrivateProfileString(L"Section_1", L"Key_3", L"-6.7f", L"../test.ini");
//#endif // _DEBUG
	
    return S_OK;
}

_uint CDelivery::Update_GameObject(_float fDeltaTime)
{
    if (m_IsDead) return DEAD_OBJECT;
    CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsStart == true)
		Movement(fDeltaTime);

	if (!m_IsHPBar)
	{
		Add_Hp_Bar(fDeltaTime);
		m_IsHPBar = true;
	}
	if (m_pHp_Bar != nullptr && m_pHP_Bar_Border != nullptr)
	{
		Set_Hp_Pos();
		Check_Degree();
	}

	Make_LockOn_Alert(fDeltaTime);

    m_pTransform->Update_Transform_Quaternion();
    for (auto& p : m_Collides)
    {
        if (p) p->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
    }

    Update_Effect();
    return NO_EVENT;
}

_uint CDelivery::LateUpdate_GameObject(_float fDeltaTime)
{
    if (m_IsDead) return DEAD_OBJECT;

    CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (m_pInfo->Get_Hp() <= 0 || m_IsDead == true)
	{
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);
		m_IsDead = true;
		if (m_pHp_Bar)
			m_pHp_Bar->Set_IsDead(TRUE);
		if (m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_IsDead(TRUE);
		m_pManagement->PlaySound(L"Ship_Explosion.ogg", CSoundMgr::SHIP_EXPLOSION);
		return DEAD_OBJECT;
	}
	if (m_IsCollide) {
		// Bullet 데미지 만큼.
		CEffectHandler::Add_Layer_Effect_Explosion(m_pTransform->Get_State(EState::Position), 1.f);
		_float fDamage = _float(m_pInfo->Get_HittedDamage());
		_float fMaxHp = _float(m_pInfo->Get_MaxHp());
		m_pHp_Bar->Set_ScaleX((-fDamage / fMaxHp) * m_fHpLength);
		m_IsCollide = false;
	}

    if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
        return UPDATE_ERROR;

    return _uint();
}

_uint CDelivery::Render_GameObject()
{
    CGameObject::Render_GameObject();

    m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
    m_pMesh->Render_Mesh();

#ifdef _DEBUG // Render Collide
    for (auto& p : m_Collides)
        if (p) p->Render_Collide();
#endif

    return _uint();
}

void CDelivery::Set_MoveStart(_bool bMove)
{
	m_IsStart = bMove;
}

_bool CDelivery::Get_IsArrive()
{
	return m_bArrive;
}

void CDelivery::Update_Effect()
{

//#ifdef _DEBUG
//    _float3 vOffset;
//
//    TCHAR szBuff[256] = L"";
//    GetPrivateProfileString(L"Section_1", L"Key_1", L"1.4f", szBuff, 256, L"../test.ini");
//    float x = _ttof(szBuff);
//    vOffset.x = x;
//
//    GetPrivateProfileString(L"Section_1", L"Key_2", L"0.9f", szBuff, 256, L"../test.ini");
//    float y = _ttof(szBuff);
//    vOffset.y = y;
//
//    GetPrivateProfileString(L"Section_1", L"Key_3", L"-6.7f", szBuff, 256, L"../test.ini");
//    float z = _ttof(szBuff);
//    vOffset.z = z;
//#endif // _DEBUG
//    m_vRightEngineOffset = vOffset;

    // Engine-Boost Effect
    if (m_pLeftEngineEffect) {
        _float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
        vEnginePos += m_pTransform->Get_State(EState::Right) * m_vLeftEngineOffset.x;
        vEnginePos += m_pTransform->Get_State(EState::Up) * m_vLeftEngineOffset.y;
        vEnginePos += m_pTransform->Get_State(EState::Look) * m_vLeftEngineOffset.z;
        m_pLeftEngineEffect->Set_EngineOffset(vEnginePos);
        m_pLeftEngineEffect->Set_IsBoost(true);
    }
    if (m_pRightEngineEffect) {
        _float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
        vEnginePos += m_pTransform->Get_State(EState::Right) * m_vRightEngineOffset.x;
        vEnginePos += m_pTransform->Get_State(EState::Up) * m_vRightEngineOffset.y;
        vEnginePos += m_pTransform->Get_State(EState::Look) * m_vRightEngineOffset.z;
        m_pRightEngineEffect->Set_EngineOffset(vEnginePos);
        m_pRightEngineEffect->Set_IsBoost(true);
    }
}

void CDelivery::Make_LockOn_Alert(_float fDeltaTime)
{
	if (m_bLockOn)
	{
		if (!m_bFirstLocked)
		{
			CGameObject* pGameObject = nullptr;
			//알림생성
			UI_DESC LockOnAlert;
			LockOnAlert.tTransformDesc.vPosition = { 900.f, 300.f, 0.f };
			LockOnAlert.tTransformDesc.vScale = { 0.f, 0.f, 0.f };
			LockOnAlert.wstrTexturePrototypeTag = L"GameObject_LockOnAlert_Delivery";
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

_uint CDelivery::Movement(_float fDeltaTime)
{
    if (nullptr == m_pTransform) return NO_EVENT;
    
    // 현재위치와 다음 경로의 위치의 거리가 1일경우 다음 노드로 이동

    // 마지막 노드 
    if (m_iCurRouteIndex == m_vecNaviRoute.size() - 1) {
        // 도착 파티클 
        // 도착 퀘스트 완료
        // 호위선 무적상태
        m_bArrive = true; /* 도착 */
    }

    if(!(m_iCurRouteIndex >= m_vecNaviRoute.size() - 1))
    {
        _float3 vCurPos = m_pTransform->Get_State(EState::Position);
        _float3 vNextPos = m_vecNaviRoute[m_iCurRouteIndex].vNodePos;

        // 거리비교
        FLOAT vLength = D3DXVec3Length(&(vNextPos - vCurPos));
        if (vLength <= 3.f) {
            m_pTransform->Set_Position(m_vecNaviRoute[m_iCurRouteIndex].vNodePos);
            ++m_iCurRouteIndex;
        }

        FindNextRoute();
    }

    if(false == m_bArrive)
        m_pTransform->Go_Straight(fDeltaTime);

    return _uint();
}

void CDelivery::FindNextRoute()
{
    if (m_iCurRouteIndex >= m_vecNaviRoute.size()) return;
    if (m_pTransform == nullptr) return;

    if (m_iCurRouteIndex == 0) {
        m_pTransform->Set_Position(m_vecNaviRoute[m_iCurRouteIndex].vNodePos);
        ++m_iCurRouteIndex;
    }

    _float3 right = { 1.f,0.f,0.f };
    _float3 up = { 0.f,1.f,0.f };
    _float3 look = { 0.f,0.f,1.f };
    _float3 dir = m_vecNaviRoute[m_iCurRouteIndex].vNodePos - m_pTransform->Get_TransformDesc().vPosition;
    D3DXVec3Normalize(&dir, &dir);

    //D3DXVec3Cross(&up, &look, &dir);
    //D3DXVec3Normalize(&up, &up);

    D3DXVec3Cross(&right, &up, &dir);
    //D3DXVec3Cross(&right, &dir, &up);
    D3DXVec3Normalize(&right, &right);

    m_pTransform->Set_State(EState::Right, right);
    m_pTransform->Set_State(EState::Up, up);
    m_pTransform->Set_State(EState::Look, dir);
}

CDelivery* CDelivery::Create(LPDIRECT3DDEVICE9 pDevice)
{
    CDelivery* pInstance = new CDelivery(pDevice);
    if (FAILED(pInstance->Ready_GameObject_Prototype()))
    {
        PRINT_LOG(L"Error", L"Failed To Create Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDelivery::Clone(void* pArg)
{
    CDelivery* pClone = new CDelivery(*this); /* 복사 생성자 호출 */
    if (FAILED(pClone->Ready_GameObject(pArg)))
    {
        PRINT_LOG(L"Error", L"Failed To Clone Player");
        Safe_Release(pClone);
    }

    return pClone;
}

void CDelivery::Free()
{
    if (m_pLeftEngineEffect) {
        m_pLeftEngineEffect->Set_IsDead(true);
        m_pLeftEngineEffect = nullptr;
    }
    if (m_pRightEngineEffect) {
        m_pRightEngineEffect->Set_IsDead(true);
        m_pRightEngineEffect = nullptr;
    }
	Safe_Release(m_pHp_Bar);
	Safe_Release(m_pHP_Bar_Border);
	Safe_Release(m_pInfo);
	Safe_Release(m_pPlayerTransform);
    Safe_Release(m_pTransform);
    Safe_Release(m_pMesh);

    CGameObject::Free();
}

_uint CDelivery::Check_Degree()
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
			if (m_pHp_Bar)
				m_pHp_Bar->Set_IsBack(true);
			if (m_pHP_Bar_Border)
				m_pHP_Bar_Border->Set_IsBack(true);
		}
		else if (fDegree <= 90.f)
		{
			if (m_pHp_Bar)
				m_pHp_Bar->Set_IsBack(false);
			if (m_pHP_Bar_Border)
				m_pHP_Bar_Border->Set_IsBack(false);
		}
	}
	else
	{
		if (fDegree > 90.f)
		{
			if (m_pHp_Bar)
				m_pHp_Bar->Set_IsBack(true);
			if (m_pHP_Bar_Border)
				m_pHP_Bar_Border->Set_IsBack(true);
		}
		else if (fDegree <= 90.f)
		{
			if (m_pHp_Bar)
				m_pHp_Bar->Set_IsBack(false);
			if (m_pHP_Bar_Border)
				m_pHP_Bar_Border->Set_IsBack(false);
		}
	}
	return _uint();
}

_uint CDelivery::Add_Hp_Bar(_float fDeltaTime)
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
		HUD_Hp_Bar.tTransformDesc.vScale = { m_pInfo->Get_Hp() * (m_fHpLength / m_pInfo->Get_MaxHp()), 8.f, 0.f };
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
		HUD_Hp_Bar_Border.tTransformDesc.vScale = { m_pInfo->Get_Hp() * (m_fHpLength / m_pInfo->Get_MaxHp()) + 2.5f, 12.f, 0.f };
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

		//Safe_Release(pGameObjectBorder);
		//Safe_Release(pGameObject);

	}
	return _uint();
}

void CDelivery::Set_Hp_Pos()
{
	_float3 vMonsterPos = m_pTransform->Get_State(EState::Position);
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);

	_float3 vDir = vMonsterPos - vPlayerPos;
	_float fDist = D3DXVec3Length(&vDir);

	// 너무 멀면 렌더하지마
	if (fDist > 600.f)
	{
		if (m_pHp_Bar)
			m_pHp_Bar->Set_Is_Far(true);
		if (m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_Is_Far(true);
	}
	else
	{
		if (m_pHp_Bar)
			m_pHp_Bar->Set_Is_Far(false);
		if (m_pHP_Bar_Border)
			m_pHP_Bar_Border->Set_Is_Far(false);
	}

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
	_float3 vLockOnPos = { ptBoss.x - (WINCX / 2.f), -ptBoss.y + (WINCY / 2.f), 0.f };

	if (m_pHp_Bar)
		m_pHp_Bar->Set_Pos(vPosition);
	if (m_pHP_Bar_Border)
		m_pHP_Bar_Border->Set_Pos(vPosition);
	//if (m_pLockOn)
	//{
	//	if (m_pLockOn->Get_IsDead() == false)
	//	{
	//		if (GetAsyncKeyState(L'R') & 0x8000)
	//		{
	//			POINT pt = { (LONG)ptBoss.x, (LONG)ptBoss.y };
	//			//ScreenToClient(g_hWnd, &pt);
	//			SetCursorPos((_int)pt.x + 7, (_int)pt.y + 13);
	//		}
	//		m_pLockOn->Set_Pos(vLockOnPos);
	//	}
	//}
}
