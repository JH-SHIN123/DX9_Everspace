#include "stdafx.h"
#include "..\Headers\Product.h"
#include "MaterialHandler.h"
#include"Pipeline.h"
#include"LobbyCam.h"
#include"Lobby.h"

CProduct::CProduct(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	ZeroMemory(&m_tPurpleMaterial, sizeof(D3DMATERIAL9));
	CMaterialHandler::Set_RGBA(0.4f, 0.2f, 0.5f, 0.f, &m_tPurpleMaterial);
	m_tPurpleMaterial.Power = 1.f;

	ZeroMemory(&m_tGoldMaterial, sizeof(D3DMATERIAL9));
	CMaterialHandler::Set_RGBA(255, 215, 0.f, 0.f, &m_tGoldMaterial);
	m_tGoldMaterial.Power = 1.f;

	ZeroMemory(&m_tRedMaterial, sizeof(D3DMATERIAL9));
	CMaterialHandler::Set_RGBA(1.f, 0.f, 0.f, 0.f, &m_tRedMaterial);
	m_tRedMaterial.Power = 1.f;


}

CProduct::CProduct(const CProduct & other)
	: CGameObject(other)
	, m_bHitRing(other.m_bHitRing)
	, m_tPurpleMaterial(other.m_tPurpleMaterial)
	,m_tRedMaterial(other.m_tRedMaterial)
	, m_tGoldMaterial(other.m_tGoldMaterial)
	, vColorRGBA(other.vColorRGBA)
{

}

HRESULT CProduct::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CProduct::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_GeoMesh_Ring",
		L"Com_GeoMesh",
		(CComponent**)&m_pGeoMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}


	
	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	if (pArg != nullptr)
	{
		TransformDesc.vPosition = ((TRANSFORM_DESC*)pArg)->vPosition;
		TransformDesc.vRotate = ((TRANSFORM_DESC*)pArg)->vRotate;
	}

	//TransformDesc.vPosition = { 0.f,3.f,-30.f };

	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(80.f);
	TransformDesc.vScale = { 1.f, 1.f, 1.f };

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

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pProductVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}
	
	_uint iProduct =(_uint)CPipeline::GetRandomFloat(0,9);
	m_eProduct = (EProduct)iProduct;
	_uint iRank = (_uint)CPipeline::GetRandomFloat(0, 2);
	m_eRank = (ERank)iRank;
	if ((_uint)m_eProduct < 6)
	{
		if (FAILED(CGameObject::Add_Component(
			EResourceType::NonStatic,
			L"Component_Texture_Product",
			L"Com_Product_Texture",
			(CComponent**)&m_pProductTex)))
		{
			PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(CGameObject::Add_Component(
			EResourceType::NonStatic,
			L"Component_Texture_PlaneTemplete",
			L"Com_Product_Texture",
			(CComponent**)&m_pProductTex)))
		{
			PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
			return E_FAIL;
		}
	}
	return S_OK;
}

_uint CProduct::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	m_fCountTime += fDeltaTime;
	if (m_bCancel)
	{
		m_fCancelCount += fDeltaTime;
		if(m_fCancelCount >= 2.f)
		{
			CLobbyCam* pCam = (CLobbyCam*)m_pManagement->Get_GameObject(L"Layer_Cam");
			pCam->Set_StartUnPacking(TRUE);
			pCam->Set_UnPacked(FALSE);
			Get_Product();
			m_pLobby->Set_Money(-1000);
			return DEAD_OBJECT;
		}
	}
	else if (m_fCountTime >= 5.f)
	{
		CLobbyCam* pCam = (CLobbyCam*)m_pManagement->Get_GameObject(L"Layer_Cam");
		pCam->Set_StartUnPacking(TRUE);
		pCam->Set_UnPacked(FALSE);
		Get_Product();
		m_pLobby->Set_Money(-1000);
		return DEAD_OBJECT;
	}
	Movement(fDeltaTime);
	UpdateProduct(fDeltaTime);
	m_pTransform->Update_Transform();
	return NO_EVENT;
}

_uint CProduct::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;
	return _uint();
}

_uint CProduct::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	switch (m_eRank)
	{
	case ERank::PurpleRank:
		m_pDevice->SetMaterial(&m_tPurpleMaterial);
		break;
	case ERank::RedRank:
		m_pDevice->SetMaterial(&m_tRedMaterial);
		break;
	case ERank::GoldRank:
		m_pDevice->SetMaterial(&m_tGoldMaterial);
		break;
	}
	m_pGeoMesh->Render_Mesh();
	if (m_bShowProduct)
		Render_Product();


#ifdef _DEBUG // Render Collide
	
#endif

	return _uint();
}

_uint CProduct::Movement(_float fDeltaTime)
{
	if (m_bShowProduct)
	{
		_float3 vPos = m_pTransform->Get_State(EState::Position);
		vPos.y = -5.f;
		m_pTransform->Set_Position(vPos);
		m_pTransform->Set_Rotate(_float3(D3DXToRadian(90.f), 0.f, 0.f));
		return 0;
	}
	if (m_fFlyTime >= 70.f && !m_bFall)
		m_bFall = TRUE;
	
	if (!m_bFall)
		m_fFlyTime += fDeltaTime*100.f;
	else
		m_fFlyTime -= fDeltaTime*100.f;

	
	_float3 vPos = m_pTransform->Get_State(EState::Position);

	if (m_fFlyTime > 0.f)
		vPos.y = m_fFlyTime;
	else
		vPos.y = -5.f;
	m_pTransform->Set_Position(vPos);

	if (m_fFlyTime > 0.f)
		m_pTransform->RotateX(fDeltaTime*10.f);
	else
	{
		m_bShowProduct = TRUE;
		m_pTransform->Set_Rotate(_float3(D3DXToRadian(90.f), 0.f, 0.f));
	}

	return _uint();
}

void CProduct::UpdateProduct(_float fDeltaTime)
{
	if (!m_bShowProduct)
		return;
	m_fRotProductY += D3DXToRadian(30.f)*fDeltaTime*10.f;

	_float3 vScale, vRot, vPos;
	vScale = { 5.f,5.f,1.f };
	vRot = m_pTransform->Get_TransformDesc().vRotate;
	vRot.x += D3DXToRadian(90.f);
	vRot.y += m_fRotProductY;
	vPos = m_pTransform->Get_State(EState::Position);
	vPos.y += 2.f;
	CPipeline::Setup_WorldMatrix(
		&m_matProduct, &vScale, &vRot, &vPos);
}

void CProduct::Render_Product()
{
	
	DWORD dwRenderState;
	
	m_pDevice->GetRenderState(D3DRS_CULLMODE,&dwRenderState);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_matProduct);
	if((_uint)m_eProduct < 6)
		m_pProductTex->Set_Texture((_uint)m_eProduct);
	else
	{
		m_pProductTex->Set_Texture((_uint)m_eProduct - 4);
	}
	m_pProductVIBuffer->Render_VIBuffer();
	m_pDevice->SetRenderState(D3DRS_CULLMODE, dwRenderState);
	if (m_bShowProduct)
		Set_Text();
}

void CProduct::Get_Product()
{
	UNIT_INFO tCurUnitInfo = *m_pLobby->Get_UnitInfo();
	switch (m_eProduct)
	{
	case EProduct::ATK_UP:
		tCurUnitInfo.iAtk += 10*((_uint)m_eRank+1);
		break;
	case EProduct::DEF_UP:
		tCurUnitInfo.iDef += 10 * ((_uint)m_eRank + 1);
		break;
	case EProduct::MAX_HP_UP:
		tCurUnitInfo.iMaxHp += 10 * ((_uint)m_eRank + 1);
		break;
	case EProduct::MAX_SHIELD_UP:
		tCurUnitInfo.iMaxShield += 10 * ((_uint)m_eRank + 1);
		break;
	case EProduct::MONEY:
		m_pLobby->Set_Money(3000 * ((_uint)m_eRank + 1));
		break;
	case EProduct::Atk_Buff:
		m_pLobby->SetAtkBuffItemCount((_uint)m_eRank + 1);
		break;
	case EProduct::Def_Buff:
		m_pLobby->SetDefBuffItemCount((_uint)m_eRank + 1);
		break;
	case EProduct::Hp_Buff:
		m_pLobby->SetHpBuffItemCount((_uint)m_eRank + 1);
		break;
	case EProduct::Energy_Buff:
		m_pLobby->SetEnergyBuffItemCount((_uint)m_eRank + 1);
		break;
	}
	m_pLobby->Set_UnitInfo(tCurUnitInfo);
}

void CProduct::Set_Text()
{
	wstring str;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	switch (m_eProduct)
	{
	case EProduct::ATK_UP:
		str = L"공격력 증가!";
		break;
	case EProduct::DEF_UP:
		str = L"방어력 증가!";
		break;
	case EProduct::MAX_HP_UP:
		str = L"최대체력 증가!";
		break;
	case EProduct::MAX_SHIELD_UP:
		str = L"쉴드량 증가!";
		break;
	case EProduct::MONEY:
		str = L"돈 획득!";
		break;
	case EProduct::Atk_Buff:
		str = L"공격력 버프 아이템!!";
		break;
	case EProduct::Def_Buff:
		str = L"방어력 버프 아이템!!";
		break;
	case EProduct::Hp_Buff:
		str = L"체력 버프 아이템!!";
		break;
	case EProduct::Energy_Buff:
		str = L"에너지 버프 아이템!!";
		break;
	}
		rc.left = (WINCX >> 1) - 100;
		rc.top = (WINCY >> 1) - 400;
		m_pManagement->Get_Font()->DrawText(NULL
			, str.c_str(), -1
			, &rc, DT_LEFT | DT_TOP, D3DXCOLOR(255, 0, 0, 255));
}


CProduct * CProduct::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CProduct* pInstance = new CProduct(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CProduct::Clone(void * pArg/* = nullptr*/)
{
	CProduct* pClone = new CProduct(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CProduct::Free()
{
	Safe_Release(m_pProductTex);
	Safe_Release(m_pProductVIBuffer);
	Safe_Release(m_pGeoMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	
	CGameObject::Free();
}
