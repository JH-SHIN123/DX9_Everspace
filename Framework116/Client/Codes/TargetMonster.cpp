#include "stdafx.h"
#include "..\Headers\TargetMonster.h"
#include "MaterialHandler.h"

CTargetMonster::CTargetMonster(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	ZeroMemory(&m_tMaterial, sizeof(D3DMATERIAL9));

	CMaterialHandler::Set_RGBA(1.f, 0.0f, 1.f, 0.f, &m_tMaterial);

	m_tMaterial.Power = 1.f;
}

CTargetMonster::CTargetMonster(const CTargetMonster & other)
	: CGameObject(other)
	, m_fReviveTime(other.m_fReviveTime)
	, m_fColTime(other.m_fColTime)
	, m_tMaterial(other.m_tMaterial)
	, vColorRGBA(other.vColorRGBA) 
{
}

HRESULT CTargetMonster::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CTargetMonster::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	wstring wstrMeshTag = L"Component_GeoMesh_Cylinder";
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		wstrMeshTag,
		L"Com_GeoMesh",
		(CComponent**)&m_pGeoMesh)))
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
	TransformDesc.vPosition = { 50.f, 0.f, 50.f };
	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(80.f);
	TransformDesc.vScale = { 2.f, 2.f, 2.f };
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
	PASSDATA_COLLIDE tCollide;
	CStreamHandler::Load_PassData_Collide(L"Target_Cylinder", wstrMeshTag, tCollide);

	m_Collides.reserve(tCollide.vecBoundingSphere.size());
	_int i = 0;
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

	return S_OK;
}

_uint CTargetMonster::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);

	m_pTransform->Update_Transform();

	for (auto& collide : m_Collides)
		collide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CTargetMonster::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	Collide_Check(fDeltaTime);

	return _uint();
}

_uint CTargetMonster::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pDevice->SetMaterial(&m_tMaterial);
	m_pTexture->Set_Texture(0);
	m_pGeoMesh->Render_Mesh();
	//m_pDevice->SetRenderState()

	// Test

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CTargetMonster::Movement(_float fDeltaTime)
{
	// m_pTransform->Go_Straight(fDeltaTime);

	return _uint();
}

_bool CTargetMonster::Collide_Check(_float fDeltaTime)
{
	if (m_IsCollide == true)
	{
		if (m_fColTime == 0.f)
		{
			vColorRGBA.y += 0.1f;
			CMaterialHandler::Set_RGBA(vColorRGBA, &m_tMaterial);
		}

		m_fColTime += fDeltaTime; 

		if (m_fColTime >= 0.5f)
		{
			m_fColTime = 0.f;
			m_IsCollide = false;
		}

	}
	return _bool();
}

CTargetMonster * CTargetMonster::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CTargetMonster* pInstance = new CTargetMonster(pDevice);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTargetMonster::Clone(void * pArg/* = nullptr*/)
{
	CTargetMonster* pClone = new CTargetMonster(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CTargetMonster::Free()
{
	Safe_Release(m_pGeoMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pCollide);

	CGameObject::Free();
}
