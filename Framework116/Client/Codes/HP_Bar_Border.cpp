#include "stdafx.h"
#include "..\Headers\HP_Bar_Border.h"
#include "Pipeline.h"
#include "Collision.h"

CHP_Bar_Border::CHP_Bar_Border(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CHP_Bar_Border::CHP_Bar_Border(const CHP_Bar_Border & other)
	: CUI(other)
{
}

HRESULT CHP_Bar_Border::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CHP_Bar_Border::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CUI::Ready_GameObject(pArg);

	return S_OK;
}

_uint CHP_Bar_Border::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);
	
	//Adjust_Pos(fDeltaTime);
	
	//여기서 하지말고 몬스터 내부에서 직접 뿌려줘야 할 것 같은데.
	//if (m_listCheckMonsters->front()->Get_IsCollide() == true)
	//{
	//	_float3 vScale = { m_pTransform->Get_TransformDesc().vScale.x, m_pTransform->Get_TransformDesc().vScale.y, 0.f };
	//	// dmg 만큼 감소
	//	vScale.x -= 1.f;
	//	m_pTransform->Set_Scale(vScale);
	//}


	return NO_EVENT;
}

_uint CHP_Bar_Border::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);

	if (m_IsDead)
		return DEAD_OBJECT;

	return _uint();
}

_uint CHP_Bar_Border::Render_GameObject()
{
	Check_Degree();
	if (!m_IsBack)
		CUI::Render_GameObject();

	return _uint();
}

void CHP_Bar_Border::Set_ScaleX(_float _fDamage)
{
	m_pTransform->Set_ScaleX(m_pTransform->Get_TransformDesc().vScale.x + _fDamage);
}

_uint CHP_Bar_Border::Movement(_float fDeltaTime)
{
	return _uint();
}

_uint CHP_Bar_Border::Adjust_Pos(_float fDeltaTime)
{
	wstring dstLayerTag = L"";
	_float2 vHpOffset = { 0.f,0.f };
	switch (m_eMakerID)
	{
	case CHP_Bar_Border::MAKER_PLAYER:
		break;
	case CHP_Bar_Border::MAKER_BOSS_MONSTER:
		dstLayerTag = L"Layer_Boss_Monster";
		vHpOffset = { -31.5f , 29.f };
		break;
	case CHP_Bar_Border::MAKER_SNIPER:
		dstLayerTag = L"Layer_Sniper";
		break;
	case CHP_Bar_Border::MAKER_DRONE:
		dstLayerTag = L"Layer_Drone";
		break;
	}

	if (dstLayerTag == L"") return -1;

	const list<class CGameObject*>* dstObjList = m_pManagement->Get_GameObjectList(dstLayerTag);
	if(nullptr == dstObjList) return -1;

	if (dstObjList->size() != 0)
	{
		vTargetPos = dstObjList->front()->Get_Collides()->front()->Get_BoundingSphere().Get_Position();
	}

	//////////////////3d좌표를 2d좌표로////////////////////////////
	D3DVIEWPORT9 vp2;
	m_pDevice->GetViewport(&vp2);
	_float4x4 TestView2, TestProj2;
	m_pDevice->GetTransform(D3DTS_VIEW, &TestView2);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj2);
	_float4x4 matCombine2 = TestView2 * TestProj2;
	D3DXVec3TransformCoord(&vTargetPos, &vTargetPos, &matCombine2);
	vTargetPos.x += 1.f;
	vTargetPos.y += 1.f;

	vTargetPos.x = (vp2.Width * (vTargetPos.x)) / 2.f + vp2.X;
	vTargetPos.y = (vp2.Height * (2.f - vTargetPos.y) / 2.f + vp2.Y);

	_float3 ptBoss;
	ptBoss.x = vTargetPos.x;
	ptBoss.y = vTargetPos.y;
	ptBoss.z = 0.f;

	//////////////////////////////////////////////////////////////////
	if (m_pTransform) {
		m_pTransform->Set_Position(_float3(ptBoss.x - (WINCX / 2.f) + vHpOffset.x, - ptBoss.y + (WINCY / 2.f) + vHpOffset.y, 0.f));
		m_pTransform->Update_Transform();
	}
	
	return _uint();
}

_uint CHP_Bar_Border::Who_Make_Me(MAKERID _iMakerName)
{
	m_eMakerID = _iMakerName;
	return _uint();
}

_uint CHP_Bar_Border::Check_Degree()
{
	if (m_pManagement->Get_GameObjectList(L"Layer_Player")->size() == 0
		/*||m_pManagement->Get_GameObjectList(L"Layer_Monster")->size() == 0*/)
		return UPDATE_ERROR;

	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	if (!m_IsRef)
	{
		Safe_AddRef(m_pPlayerTransform);
		m_IsRef = true;
	}
	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
		return E_FAIL;
	}

	if (m_eMakerID != MAKER_PLAYER)
	{
		m_pPlayerTransform->Get_TransformDesc().matWorld;
		_float3 vPlayerLook = m_pPlayerTransform->Get_State(EState::Look);

		if (m_eMakerID == MAKER_BOSS_MONSTER)
		{
			m_listCheckMonsters = m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster");
			if (nullptr == m_listCheckMonsters || m_listCheckMonsters->size() == 0) return NO_EVENT;

			auto& iter = m_listCheckMonsters->begin();

			for (; iter != m_listCheckMonsters->end(); ++iter)
			{
				_float3 v1 = vPlayerLook; // 얘는 방향벡턴데?
				_float3 v2 = (*iter)->Get_Collides()->front()->Get_BoundingSphere().Get_Position() - m_pTransform->Get_State(EState::Position); // 위치벡터네?
				_float fCeta;
				D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);
				_float v1v2 = D3DXVec3Dot(&v1, &v2);
				_float v1Length = D3DXVec3Length(&v1);
				_float v2Length = D3DXVec3Length(&v2);
				fCeta = acosf(v1v2 / (v1Length * v2Length));

				_float fDegree = D3DXToDegree(fCeta);

				if (fabs(fDegree) > 90.f)
				{
					m_IsBack = true;
				}
				else if (fabs(fDegree) <= 90.f)
				{
					m_IsBack = false;
				}
			}
		}

	}
	return _uint();
}

CHP_Bar_Border * CHP_Bar_Border::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CHP_Bar_Border* pInstance = new CHP_Bar_Border(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create HP_Bar");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject * CHP_Bar_Border::Clone(void * pArg/* = nullptr*/)
{
	CHP_Bar_Border* pClone = new CHP_Bar_Border(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone HP_Bar");
		Safe_Release(pClone);
	}
	
   	return pClone;
}

void CHP_Bar_Border::Free()
{
	Safe_Release(m_pPlayerTransform);
	CUI::Free();
}

