#include "stdafx.h"
#include "Delivery.h"

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

    return S_OK;
}

_uint CDelivery::Update_GameObject(_float fDeltaTime)
{
    if (m_IsDead) return DEAD_OBJECT;
    CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsStart == true)
		Movement(fDeltaTime);



    m_pTransform->Update_Transform_Quaternion();
    for (auto& p : m_Collides)
    {
        if (p) p->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
    }
    return NO_EVENT;
}

_uint CDelivery::LateUpdate_GameObject(_float fDeltaTime)
{
    if (m_IsDead) return DEAD_OBJECT;

    CGameObject::LateUpdate_GameObject(fDeltaTime);

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
    Safe_Release(m_pTransform);
    Safe_Release(m_pMesh);

    CGameObject::Free();
}
