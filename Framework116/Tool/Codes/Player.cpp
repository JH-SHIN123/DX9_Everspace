#include "pch.h"
#include "..\Headers\Player.h"
#include "MeshTool.h"
#include "Mesh.h"

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CPlayer::CPlayer(const CPlayer & other)
	: CGameObject(other)
{
}

HRESULT CPlayer::ChangeMesh(const wstring& wstrMeshPrototypeTag)
{
	// 기존꺼 m_pMesh 레퍼런스 카운팅 반납
	Safe_Release(m_pMesh);

	// 메시 교체
	auto iter_find = m_Components.find(L"Com_Mesh");
	if (m_Components.end() != iter_find)
	{
		// 실질적 메모리 해제 (클론삭제)
		Safe_Release(iter_find->second);

		// 새로운거로 변경
		CComponent* pClone = (CComponent*)m_pManagement->Clone_Component(EResourceType::Static, wstrMeshPrototypeTag);
		if (nullptr == pClone)
		{
			PRINT_LOG(L"Error", L"Failed to Change Mesh");
			return E_FAIL;
		}

		// 원본데이터(m_Components) 포인터지정
		iter_find->second = pClone;

		m_pMesh = (CMesh*)pClone;
		Safe_AddRef(pClone);

		m_wstrMeshPrototypeTag = wstrMeshPrototypeTag;
	}

	return S_OK;
}

HRESULT CPlayer::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CPlayer::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// Default Player Mesh
	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_GeoMesh_Cube",
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Component_Mesh_Axis");
		return E_FAIL;
	}
	m_wstrMeshPrototypeTag = L"Component_GeoMesh_Cube";

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
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


	return S_OK;
}

_uint CPlayer::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CPlayer::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	// Mesh Tool Info Update
	if (CMeshTool::s_pInstance)
	{
		const _float3 vPos = m_pTransform->Get_State(EState::Position);
		CMeshTool::s_pInstance->m_Edit_PosX.SetWindowTextW(to_wstring(vPos.x).c_str());
		CMeshTool::s_pInstance->m_Edit_PosY.SetWindowTextW(to_wstring(vPos.y).c_str());
		CMeshTool::s_pInstance->m_Edit_PosZ.SetWindowTextW(to_wstring(vPos.z).c_str());

		const _float3 vRotate = m_pTransform->Get_TransformDesc().vRotate;
		CMeshTool::s_pInstance->m_Edit_RotateX.SetWindowTextW(to_wstring(vRotate.x).c_str());
		CMeshTool::s_pInstance->m_Edit_RotateY.SetWindowTextW(to_wstring(vRotate.y).c_str());
		CMeshTool::s_pInstance->m_Edit_RotateZ.SetWindowTextW(to_wstring(vRotate.z).c_str());
	}

	return _uint();
}

_uint CPlayer::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);

	if(m_pMesh)
		m_pMesh->Render_Mesh();

	return _uint();
}

_uint CPlayer::Movement(_float fDeltaTime)
{
	// Move
	_float3 vOutPos = m_pTransform->Get_State(EState::Position);
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

	// y로 이동하는 코드 작성해야함
	//if (GetAsyncKeyState('D') & 0x8000)
	//{
	//	m_pTransform->Go_Side(fDeltaTime);
	//}

	//if (GetAsyncKeyState('A') & 0x8000)
	//{
	//	m_pTransform->Go_Side(-fDeltaTime);
	//}

	// Rotate
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_pTransform->RotateY(-fDeltaTime);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pTransform->RotateY(fDeltaTime);
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
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);

	CGameObject::Free();
}
