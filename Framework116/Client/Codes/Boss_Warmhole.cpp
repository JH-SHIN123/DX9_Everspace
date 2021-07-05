#include "stdafx.h"
#include "..\Headers\Boss_Warmhole.h"

CBoss_Warmhole::CBoss_Warmhole(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CBoss_Warmhole::CBoss_Warmhole(const CBoss_Warmhole & other)
	: CGameObject(other)
	, m_fOpenTime(other.m_fOpenTime)
	, m_fSpawnTime(other.m_fSpawnTime)
	, m_bSpawn(other.m_bSpawn)
	, m_iSpawnCount(other.m_iSpawnCount)
	, m_fSpawnTime_Reset(other.m_fSpawnTime_Reset)
{
}

HRESULT CBoss_Warmhole::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CBoss_Warmhole::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Billboard_Warmhole",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Billboard_Warmhole");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = _float3(0.5f, 5.f, 70.f);
	TransformDesc.fRotatePerSec = D3DXToRadian(-50.f);
	TransformDesc.vScale = m_vScale;

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
	//BOUNDING_SPHERE BoundingSphere;
	//BoundingSphere.fRadius = 1.f;

	//if (FAILED(CGameObject::Add_Component(
	//	EResourceType::Static,
	//	L"Component_CollideSphere",
	//	L"Com_CollideSphere",
	//	(CComponent**)&m_pCollide,
	//	&BoundingSphere,
	//	true)))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
	//	return E_FAIL;
	//}

	return S_OK;
}

_uint CBoss_Warmhole::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	//Movement(fDeltaTime);

	Spawn_Monster(fDeltaTime);

	m_pTransform->Update_Transform();
	//m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CBoss_Warmhole::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;

	Scale(fDeltaTime);
	Spawn_Monster(fDeltaTime);
	Billboard(fDeltaTime);


	return _uint();
}

_uint CBoss_Warmhole::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);

	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer(); 
	// Test

#ifdef _DEBUG // Render Collide
	//m_pCollide->Render_Collide();
#endif

	return _uint();
}

_uint CBoss_Warmhole::Movement(_float fDeltaTime)
{
	//_float3 vOutPos = m_pTransform->Get_State(EState::Position);
	//if (true == m_pTerrainBuffer->Is_OnTerrain(&vOutPos))
	//{
	//	vOutPos.y += 0.5f;
	//	m_pTransform->Set_Position(vOutPos);
	//}	

	return _uint();
}

_uint CBoss_Warmhole::Billboard(_float fDeltaTime)
{
	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	ZeroMemory(&matView._41, sizeof(_float3));
	D3DXMatrixInverse(&matView,0 ,&matView);

	_float3 vBillPos = m_pTransform->Get_State(EState::Position);

	_float fScale[3];
	fScale[0] = m_pTransform->Get_State(EState::Right).x;
	fScale[1] = m_pTransform->Get_State(EState::Up).y;
	fScale[2] = m_pTransform->Get_State(EState::Look).z;

	memcpy(&matView._41, &vBillPos, sizeof(_float3));

	for (_uint i = 0; i < 3; ++i)
	{
		for (_uint j = 0; j < 4; ++j)
			matView(i, j) *= fScale[i];
	}

	m_pTransform->Set_WorldMatrix(matView);

	return _uint();
}

_uint CBoss_Warmhole::Spin(_float fDeltaTime)
{
	//m_pTransform->RotateZ(-fDeltaTime);

	return _uint();
}

_uint CBoss_Warmhole::Scale(_float fDeltaTime)
{

	if (m_fOpenTime >= m_fSpawnTime && m_bSpawn == false)
	{
		m_fOpenTime -= fDeltaTime;

		//m_vScale *= 1.006f;
		//m_pTransform->Set_Scale(m_vScale);
	}

	else
		m_bSpawn = true;


	return _uint();
}

_uint CBoss_Warmhole::Spawn_Monster(_float fDeltaTime)
{
	if (m_bSpawn == true)
	{
		m_fSpawnTime -= fDeltaTime;

		if (m_fSpawnTime <= 0.f)
		{
			if (m_iSpawnCount)
			{
				m_fSpawnTime = m_fSpawnTime_Reset;
				--m_iSpawnCount;

				TRANSFORM_DESC* pArg = new TRANSFORM_DESC;
				_float3 vPos = m_pTransform->Get_State(EState::Position);
				_float3 vLook = m_pTransform->Get_State(EState::Look);

				D3DXVec3Normalize(&vLook, &vLook);
				vPos += vLook * 8.f;

				_float4x4 matWorld;
				_float3 vScale, vTrans;
				D3DXQUATERNION	qRotate;
				D3DXMatrixDecompose(&vScale, &qRotate, &vTrans, &m_pTransform->Get_TransformDesc().matWorld);

				//pArg->matWorld = m_pTransform->Get_TransformDesc().matWorld;
				pArg->vRotate = { qRotate.x, qRotate.y, qRotate.z };
				pArg->vPosition = vPos;

				if (FAILED(m_pManagement->Add_GameObject_InLayer(
					EResourceType::NonStatic,
					L"GameObject_Boss_Spawn_Monster",
					L"Layer_Boss_Spawn_Monster", pArg)))
				{
					PRINT_LOG(L"Error", L"Failed To Add Boss_Spawn_Monster In Layer");
					return E_FAIL;
				}


			}
		}


	}

	return _uint();
}

CBoss_Warmhole * CBoss_Warmhole::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CBoss_Warmhole* pInstance = new CBoss_Warmhole(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss_Warmhole::Clone(void * pArg/* = nullptr*/)
{
	CBoss_Warmhole* pClone = new CBoss_Warmhole(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CBoss_Warmhole::Free()
{
	//Safe_Release(m_pTerrainBuffer);
	//Safe_Release(m_pCollide);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
