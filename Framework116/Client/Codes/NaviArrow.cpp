#include "stdafx.h"
#include "NaviArrow.h"
#include "MaterialHandler.h"

CNaviArrow::CNaviArrow(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CNaviArrow::CNaviArrow(const CNaviArrow& other)
	: CGameObject(other)
{
}

HRESULT CNaviArrow::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CNaviArrow::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);

	NAVI_ARROW_DESC* pDesc = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (pDesc = dynamic_cast<NAVI_ARROW_DESC*>(ptr))
		{
		}
		else
		{
			PRINT_LOG(L"Error", L"NAVI_ARROW_DESC is nullptr");
			return E_FAIL;
		}
	}

	// For.Com_Mesh
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_NaviArrow",
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = pDesc->vPos;
	TransformDesc.vPosition.y -= 5.f;
	TransformDesc.vScale = { 6.f, 6.f, 6.f };
	
	_float3 right = { 1.f,0.f,0.f };
	_float3 up = { 0.f,1.f,0.f };
	_float3 look = { 0.f,0.f,1.f };
	_float3 dir = pDesc->vDir;

	//D3DXVec3Cross(&up, &look, &dir);
	//D3DXVec3Normalize(&up, &up);

	D3DXVec3Cross(&right, &up, &dir);
	D3DXVec3Normalize(&right, &right);

	TransformDesc.vAxis[(_uint)EState::Right] = right;
	TransformDesc.vAxis[(_uint)EState::Up] = up;
	TransformDesc.vAxis[(_uint)EState::Look] = pDesc->vDir;

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

	CMaterialHandler::Set_RGBA(0.f, 0.9f, 0.f, 0.7f, &m_tMaterial);
	m_tMaterial.Power = 10.f;

	return S_OK;
}

_uint CNaviArrow::Update_GameObject(_float fDeltaTime)
{
	if (m_IsDead) return DEAD_OBJECT;
	CGameObject::Update_GameObject(fDeltaTime);
	Movement(fDeltaTime);

	m_pTransform->Update_Transform_Quaternion();
	return NO_EVENT;
}

_uint CNaviArrow::LateUpdate_GameObject(_float fDeltaTime)
{
	if (m_IsDead) return DEAD_OBJECT;

	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CNaviArrow::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pDevice->SetMaterial(&m_tMaterial);
	m_pMesh->Render_Mesh();

	return _uint();
}

_uint CNaviArrow::Movement(_float fDeltaTime)
{
	return _uint();
}

CNaviArrow* CNaviArrow::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CNaviArrow* pInstance = new CNaviArrow(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNaviArrow::Clone(void* pArg)
{
	CNaviArrow* pClone = new CNaviArrow(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CNaviArrow::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh);

	CGameObject::Free();
}
