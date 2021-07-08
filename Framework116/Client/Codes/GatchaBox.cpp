#include "stdafx.h"
#include "..\Headers\GatchaBox.h"
#include"Collision.h"
#include"Lobby.h"
#include"LobbyCam.h"
#include"Product.h"
CGatchaBox::CGatchaBox(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CGatchaBox::CGatchaBox(const CGatchaBox & other)
	: CGameObject(other)
{
}

HRESULT CGatchaBox::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CGatchaBox::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_CubeTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_GatchaBox",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 1.f;
	TransformDesc.fRotatePerSec = 1.f;
	TransformDesc.vPosition = _float3(-15.f, 0.f, -15.f);	
	TransformDesc.vScale = { 10.f,10.f,10.f };
	TransformDesc.vRotate = { 0.f,0.f,0.f };
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
	BOUNDING_SPHERE BoundingSphere;
	BoundingSphere.fRadius = 5.f;

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

_uint CGatchaBox::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);
	if (StartUnPacking(fDeltaTime))
	{
		m_pLobby->Set_StartUnPacking(FALSE);
		m_bStartUnpacking = FALSE;
	}
	m_pTransform->Update_Transform();
	return NO_EVENT;
}

_uint CGatchaBox::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CGatchaBox::Render_GameObject()
{
	if (m_bBomb)
		return 0;

	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	CheckPicking();
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer(); 
	// Test

#ifdef _DEBUG // Render Collide
#endif

	return _uint();
}

_uint CGatchaBox::Movement(_float fDeltaTime)
{
	if (m_bStartUnpacking)
		return 0;

	m_pTransform->RotateY(D3DXToRadian(45.f)*fDeltaTime);
	return _uint();
}

_bool CGatchaBox::CheckPicking()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 
		&& !m_bStartUnpacking)
	{
		_float fDist;
		CGameObject* pObj = CCollision::PickingObject(fDist, g_hWnd, WINCX, WINCY, 
			m_pDevice, m_pManagement->Get_GameObjectList(L"Layer_GatchaBox"));
		if (pObj && m_pLobby->Get_Money() >=1000)
		{
			m_fUnPackingTime = 0.f;
			m_bStartUnpacking = TRUE;
			return true;
		}
	}

	return false;
}

_bool CGatchaBox::StartUnPacking(_float fDeltaTime)
{
	if (!m_bStartUnpacking)
		return false;
	if (!m_pLobby)
		return false;
	m_pLobby->Set_StartUnPacking(TRUE);
	m_fUnPackingTime += fDeltaTime;
	_int fTime = (_int)m_fUnPackingTime;
	if (m_fUnPackingTime - fTime <= 0.5f)
		m_pTransform->Go_Side(fDeltaTime*5.f);
	else
		m_pTransform->Go_Side(-fDeltaTime*5.f);
	
	if (m_fUnPackingTime >= 3.f)
	{
		if (!m_bBomb)
		{
			CLobbyCam* pLobbyCam = (CLobbyCam*)m_pManagement->Get_GameObject(L"Layer_Cam");
			pLobbyCam->Set_UnPacked(TRUE);
			wstring strLayerTag = L"Layer_Product";
			Add_Layer_Product(strLayerTag);
		}
		m_bBomb = TRUE;
	}
	if (m_fUnPackingTime >= 12.f)
	{
		m_pLobby->Set_IsGatcha(TRUE);
		CLobbyCam* pLobbyCam = (CLobbyCam*)m_pManagement->Get_GameObject(L"Layer_Cam");
		pLobbyCam->Set_UnPacked(FALSE);
		m_bBomb = FALSE;
		m_pTransform->Set_Position(_float3(-15.f, 0.f, -15.f));
		pLobbyCam->Set_CamAt(m_pTransform->Get_TransformDesc().vPosition);
		return TRUE;
	}
	
	return false;
}

void CGatchaBox::Add_Layer_Product(wstring & wstrLayerTag)
{
	TRANSFORM_DESC TransformDesc;
	
	TransformDesc.vPosition = m_pTransform->Get_State(EState::Position);
	TransformDesc.vRotate = { 0.f,0.f,0.f };
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_Product",
		wstrLayerTag, &TransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Ring In Layer");
		return;
	}
	CProduct* pProduct = (CProduct*)m_pManagement->Get_GameObject(wstrLayerTag);
	pProduct->Set_Scene(m_pLobby);
	Safe_AddRef(m_pLobby);
}

CGatchaBox * CGatchaBox::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CGatchaBox* pInstance = new CGatchaBox(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGatchaBox::Clone(void * pArg/* = nullptr*/)
{
	CGatchaBox* pClone = new CGatchaBox(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CGatchaBox::Free()
{
	Safe_Release(m_pLobby);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
