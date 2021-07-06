#include "stdafx.h"
#include "..\Headers\Meteor.h"

CMeteor::CMeteor(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CMeteor::CMeteor(const CMeteor & other)
	: CGameObject(other)
{
}

HRESULT CMeteor::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CMeteor::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_CubeTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Monster",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = {100.f, -50.f, -100.f};
	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(80.f);
	TransformDesc.vScale = { 2.f, 2.f, 2.f };
	//TransformDesc.vRotate = ;

	if (pArg != nullptr)
	{
		TransformDesc.vPosition = ((TRANSFORM_DESC*)pArg)->vPosition;
		TransformDesc.vRotate = ((TRANSFORM_DESC*)pArg)->vRotate;
	}


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

	//m_pTerrainBuffer = (CVIBuffer_TerrainTexture*)m_pManagement->Get_Component(L"Layer_Terrain", L"Com_VIBuffer");
	//Safe_AddRef(m_pTerrainBuffer);
	//if (nullptr == m_pTerrainBuffer)
	//{
	//	PRINT_LOG(L"Error", L"m_pTerrainBuffer is nullptr");
	//	return E_FAIL;
	//}

	// For.Com_Collide
	BOUNDING_SPHERE BoundingSphere;
	BoundingSphere.fRadius = 1.f;

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

	return S_OK;
}

_uint CMeteor::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CMeteor::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CMeteor::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pMesh->Render_Mesh();
	// Test

#ifdef _DEBUG // Render Collide
	m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CMeteor::Movement(_float fDeltaTime)
{
	//_float3 vOutPos = m_pTransform->Get_State(EState::Position);
	//if (true == m_pTerrainBuffer->Is_OnTerrain(&vOutPos))
	//{
	//	vOutPos.y += 0.5f;
	//	m_pTransform->Set_Position(vOutPos);
	//}	


	m_pTransform->Go_Straight(fDeltaTime);


	return _uint();
}

CMeteor * CMeteor::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CMeteor* pInstance = new CMeteor(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMeteor::Clone(void * pArg/* = nullptr*/)
{
	CMeteor* pClone = new CMeteor(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMeteor::Free()
{
	//Safe_Release(m_pTerrainBuffer);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
