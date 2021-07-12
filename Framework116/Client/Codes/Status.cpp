#include "stdafx.h"
#include "..\Headers\Status.h"
#include"Pipeline.h"
#include"LobbyCam.h"
#include"Lobby.h"
#include"VIBuffer_HexagonTex.h"
#include"GatchaBox.h"
CStatus::CStatus(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CStatus::CStatus(const CStatus & other)
	: CGameObject(other)
{
}

HRESULT CStatus::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CStatus::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_VIBuffer_HexagonTex",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}
	

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_StatusBoard",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	CTransform* pTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_StatusBoard", L"Com_Transform");
	TRANSFORM_DESC tTrans;
	tTrans.vScale = pTransform->Get_TransformDesc().vScale;
	tTrans.vScale *= 0.25f;
	tTrans.vPosition = pTransform->Get_TransformDesc().vPosition;
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,

		&tTrans)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}
	

	return S_OK;
}

_uint CStatus::Update_GameObject(_float fDeltaTime)
{
	CGatchaBox* pBox = (CGatchaBox*)m_pManagement->Get_GameObject(L"Layer_GatchaBox");
	if (pBox)
	{
		if (pBox->Get_StartUnpacking())
			return 0;
	}
	CGameObject::Update_GameObject(fDeltaTime);

	Movement(fDeltaTime);
	if (UpdateHexagon(fDeltaTime))
	{
		_float fInc = 2.f/3.f;
		
		_float3 vAtk = {0,((_float)m_tPreUnitInfo.iAtk/100.f),0.f};

		_float3 vDef = { ((_float)m_tPreUnitInfo.iDef / 100.f),
			(((_float)m_tPreUnitInfo.iDef / 100.f) * fInc),0.f };

		_float3 vHp = { (_float)(m_tPreUnitInfo.iMaxHp / 100.f)
		,-((_float)m_tPreUnitInfo.iMaxHp / 100.f) *fInc,0.f };

		_float3 vShield = { 0,-((_float)m_tPreUnitInfo.iMaxShield / 100.f),0.f };

		_float3 vEnergy = { -((_float)m_tPreUnitInfo.iMaxEnergy / 100.f)
					,((_float)m_tPreUnitInfo.iMaxEnergy / 100.f)*-fInc,0.f };

		_float3 vFireRate = { -((_float)m_tPreUnitInfo.iFireRate / 100.f)
			,((_float)m_tPreUnitInfo.iFireRate / 100.f)*fInc,0.f };

		static_cast<CVIBuffer_HexagonTex*>(m_pVIBuffer)->SetVB(vAtk, vDef, vHp, vShield, vEnergy, vFireRate);
	}
	
	return m_pTransform->Update_Transform();

}

_uint CStatus::LateUpdate_GameObject(_float fDeltaTime)
{


	CGameObject::LateUpdate_GameObject(fDeltaTime);


	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::AlphaUI, this)))
		return UPDATE_ERROR;


	return _uint();
}

_uint CStatus::Render_GameObject()
{

	if (m_pLobby->Get_GotoNextScene())
		return 0;

	CGatchaBox* pBox = (CGatchaBox*)m_pManagement->Get_GameObject(L"Layer_GatchaBox");
	if (pBox)
	{
		if (pBox->Get_StartUnpacking())
			return 0;
	}
	CGameObject::Render_GameObject();
	
	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();
	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = transformDesc.vScale.x;
	matView._22 = transformDesc.vScale.y;
	matView._41 = transformDesc.vPosition.x;
	matView._42 = transformDesc.vPosition.y;
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
	m_pTexture->Set_Texture(1);
	m_pVIBuffer->Render_VIBuffer();
	/////////////////////////////////////////////////////////////////

	return _uint();
}

_uint CStatus::Movement(_float fDeltaTime)
{
	return _uint();
}

_bool CStatus::UpdateHexagon(_float fDeltaTime)
{
	m_pUnitInfo = m_pLobby->Get_UnitInfo();
	if (!m_pUnitInfo)
		return FALSE;
	static _float fUpdateTime = 0;
	fUpdateTime+=fDeltaTime;
	if (fUpdateTime <= 0.1f)
		return FALSE;
	UNIT_INFO m_tCurUnitInfo = *m_pUnitInfo;
	_bool bUpdate = FALSE;
	if (m_tCurUnitInfo.iAtk != m_tPreUnitInfo.iAtk)
	{
		if (m_tCurUnitInfo.iAtk > m_tPreUnitInfo.iAtk)
			m_tPreUnitInfo.iAtk++;
		else if (m_tCurUnitInfo.iAtk < m_tPreUnitInfo.iAtk)
			m_tPreUnitInfo.iAtk--;
		bUpdate = TRUE;
	}
	if (m_tCurUnitInfo.iDef != m_tPreUnitInfo.iDef)
	{
		if (m_tCurUnitInfo.iDef > m_tPreUnitInfo.iDef)
			m_tPreUnitInfo.iDef++;
		else if (m_tCurUnitInfo.iDef < m_tPreUnitInfo.iDef)
			m_tPreUnitInfo.iDef--;
		bUpdate = TRUE;
	}
	if (m_tCurUnitInfo.iMaxHp != m_tPreUnitInfo.iMaxHp)
	{
		if (m_tCurUnitInfo.iMaxHp > m_tPreUnitInfo.iMaxHp)
			m_tPreUnitInfo.iMaxHp++;
		else if (m_tCurUnitInfo.iMaxHp < m_tPreUnitInfo.iMaxHp)
			m_tPreUnitInfo.iMaxHp--;
		bUpdate = TRUE;
	}
	if (m_tCurUnitInfo.iMaxShield != m_tPreUnitInfo.iMaxShield)
	{
		if (m_tCurUnitInfo.iMaxShield > m_tPreUnitInfo.iMaxShield)
			m_tPreUnitInfo.iMaxShield++;
		else if (m_tCurUnitInfo.iMaxShield < m_tPreUnitInfo.iMaxShield)
			m_tPreUnitInfo.iMaxShield--;
		bUpdate = TRUE;
	}
	if (m_tCurUnitInfo.iMaxEnergy != m_tPreUnitInfo.iMaxEnergy)
	{
		if (m_tCurUnitInfo.iMaxEnergy > m_tPreUnitInfo.iMaxEnergy)
			m_tPreUnitInfo.iMaxEnergy++;
		else if (m_tCurUnitInfo.iMaxEnergy < m_tPreUnitInfo.iMaxEnergy)
			m_tPreUnitInfo.iMaxEnergy--;
		bUpdate = TRUE;
	}
	if (m_tCurUnitInfo.iFireRate != m_tPreUnitInfo.iFireRate)
	{
		if (m_tCurUnitInfo.iFireRate > m_tPreUnitInfo.iFireRate)
			m_tPreUnitInfo.iFireRate++;
		else if (m_tCurUnitInfo.iFireRate < m_tPreUnitInfo.iFireRate)
			m_tPreUnitInfo.iFireRate--;
		bUpdate = TRUE;
	}
	fUpdateTime = 0.f;
	return bUpdate;

}



CStatus * CStatus::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CStatus* pInstance = new CStatus(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatus::Clone(void * pArg/* = nullptr*/)
{
	CStatus* pClone = new CStatus(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CStatus::Free()
{

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	CGameObject::Free();
}
