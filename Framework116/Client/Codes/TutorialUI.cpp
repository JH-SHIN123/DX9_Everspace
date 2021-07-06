#include "stdafx.h"
#include "..\Headers\TutorialUI.h"
#include "Collision.h"
#include "Pipeline.h"
#include "Ring.h"

CTutorialUI::CTutorialUI(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CTutorialUI::CTutorialUI(const CTutorialUI & other)
	: CGameObject(other)
{
}

HRESULT CTutorialUI::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CTutorialUI::Ready_GameObject(void * pArg/* = nullptr*/)
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
		L"Component_Texture_Tutorial_Nevi",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	TRANSFORM_DESC Transform;
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	/*m_pTargetCollide = (CCollideSphere*)pArg;*/
	/*m_ray = (RAY*)pArg;*/

	wstring* m_pTargetLayerTag = (wstring*)pArg;
	m_wstrLayerTag = *m_pTargetLayerTag;
	
	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);
	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"m_pTargetTransform is nullptr");
		return UPDATE_ERROR;
	}

	//m_listTargetTransforms =
	/*
	1. 타겟의 리스트를 받는다
	2. 순회하면서 그 타겟의 트랜스폼 컴포넌트를 내 리스트에 넣는다.
	3. 넣어 주면서 Add_Ref
	*/
	m_listTargetObject = *(m_pManagement->Get_GameObjectList(L"Layer_Ring"));
	if (m_listTargetObject.empty())
	{
		PRINT_LOG(L"Error", L"m_listTargetObject is empty");
		return E_FAIL;
	}
	else
	{
		for(auto& iter : m_listTargetObject)
			Safe_AddRef(iter);
	}

	return S_OK;
}

_uint CTutorialUI::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	

	//m_pTargetTransform = (CTransform*)m_pManagement->Get_Component(m_wstrLayerTag, L"Com_Transform");
	
	Search_Target(fDeltaTime);

	Movement(fDeltaTime);	
	


	return m_pTransform->Update_Transform();
}

_uint CTutorialUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);
	
	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;



	return _uint();
}

_uint CTutorialUI::Render_GameObject()
{
	CGameObject::Render_GameObject();

	_float4x4 matWorld = m_pTransform->Get_TransformDesc().matWorld;
	matWorld._11 = 10.f;
	matWorld._22 = 10.f;
	matWorld._33 = 10.f;

	matWorld._41 = m_vSearchTagetDis[0].x;
	matWorld._42 = m_vSearchTagetDis[0].y;
	matWorld._43 = m_vSearchTagetDis[0].z;

	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);

	_float4x4 matBill;
	D3DXMatrixIdentity(&matBill);

	matBill = matView;
	matBill._41 = 0.f;
	matBill._42 = 0.f;
	matBill._43 = 0.f;

	D3DXMatrixInverse(&matBill, 0, &matBill);
	_float4x4 realmatWorld;
	realmatWorld = matBill * matWorld;
	m_pTransform->Set_WorldMatrix(realmatWorld);

	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return _uint();
}

_uint CTutorialUI::Movement(_float fDeltaTime)
{
	//m_pTransform->

	return _uint();
}

_uint CTutorialUI::Search_Target(_float fDeltaTime)
{
	//auto iter = (m_pManagement->Get_GameObjectList(L"Layer_Ring"))->begin();
	//for (auto& iter : *m_pManagement->Get_GameObjectList(L"Layer_Ring"))
	//{
	//	//v = ((CTransform*)(iter.Get_Component(L"Com_Transform"))).Get_State(EState::Position);
	//}

	_float3 fPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_uint i = 0;
	for (auto& iter : m_listTargetObject)
	{
		if (iter->Get_IsCollide() == false)
		{
			_float3 vPos;
			vPos = ((CTransform*)(iter->Get_Component(L"Com_Transform")))->Get_State(EState::Position);

			_float fDis = fabs(D3DXVec3Length(&(vPos - fPlayerPos)));

			m_vSearchTagetDis[i] = { vPos.x, vPos.y, vPos.z, fDis };
		}
		++i;
	}

	_uint iSize = m_listTargetObject.size();

	for (_uint i = 0; i < iSize; ++i)
	{
		for (_uint j = 0; j < iSize; ++j)
		{
			if (m_vSearchTagetDis[j].w > m_vSearchTagetDis[j + 1].w)
			{
				_float4 vTemp = m_vSearchTagetDis[j];
				m_vSearchTagetDis[j] = m_vSearchTagetDis[j + 1];
				m_vSearchTagetDis[j + 1] = vTemp;
			}
		}
	}



	_float3 v;


	return _uint();
}

CTutorialUI * CTutorialUI::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CTutorialUI* pInstance = new CTutorialUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create TutorialUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTutorialUI::Clone(void * pArg/* = nullptr*/)
{
	CTutorialUI* pClone = new CTutorialUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone TutorialUI");
		Safe_Release(pClone);
	}

	return pClone;
}

void CTutorialUI::Free()
{
	for (auto& pObject : m_listTargetObject)
	{
		Safe_Release(pObject);
	}
	m_listTargetObject.clear();

	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
