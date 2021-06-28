#include "stdafx.h"
#include "..\Headers\Dummy_Mon.h"
#include "Collision.h"
#include "Load_Prototype.h"

CDummy_Mon::CDummy_Mon(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	if (CLoad_Prototype::Load_PassData_Object(
		L"../../Data/PrototypeData/DUMMY.object", m_tPassData) == false)
	{
		PRINT_LOG(L"Load Error", L"Wrong PrototypeData Path");
	}
}

CDummy_Mon::CDummy_Mon(const CDummy_Mon & other)
	: CGameObject(other)
	, m_tPassData(other.m_tPassData)
{
}

HRESULT CDummy_Mon::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CDummy_Mon::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	//// For.Com_VIBuffer
	//if (FAILED(CGameObject::Add_Component(
	//	EResourceType::Static,
	//	m_tPassData.wstrPrototypeTag_Mesh,
	//	L"Com_Mesh",
	//	(CComponent**)&m_pMesh)))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
	//	return E_FAIL;
	//}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vPosition = { 10.f,0.f,10.f };
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
		&BoundingSphere)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}
	//

	// Setting Prev Cursor 
	GetCursorPos(&m_tPrevCursorPos);
	ScreenToClient(g_hWnd, &m_tPrevCursorPos);

	return S_OK;
}

_uint CDummy_Mon::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().vPosition);
	return NO_EVENT;
}

_uint CDummy_Mon::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CDummy_Mon::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();
	
#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CDummy_Mon::Movement(_float fDeltaTime)
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

	// Rotate

	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		RAY ray;
		CCollision::CreatePickingRay(ray, g_hWnd, WINCX, WINCY, m_pDevice);

		D3DXMATRIX view;
		m_pDevice->GetTransform(D3DTS_VIEW, &view);
		CCollision::TransformRay(ray, view);

		// Layer에 모든 오브젝트들의 바운딩박스 검사
		if (CCollision::IntersectRayToSphere(ray, m_pCollide->Get_BoundingSphere())) {
			PRINT_LOG(L"Hit!", L"Hit!");
		}
	}

	return _uint();
}

CDummy_Mon * CDummy_Mon::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CDummy_Mon* pInstance = new CDummy_Mon(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDummy_Mon::Clone(void * pArg/* = nullptr*/)
{
	CDummy_Mon* pClone = new CDummy_Mon(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CDummy_Mon::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
