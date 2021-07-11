#include "stdafx.h"
#include "..\Headers\HP_Bar.h"
#include "Pipeline.h"
#include "Collision.h"

CHP_Bar::CHP_Bar(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CHP_Bar::CHP_Bar(const CHP_Bar & other)
	: CUI(other)
{
}

HRESULT CHP_Bar::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CHP_Bar::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CUI::Ready_GameObject(pArg);

	return S_OK;
}

_uint CHP_Bar::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);
	
	Adjust_Pos(fDeltaTime);
	
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

_uint CHP_Bar::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);

	if (m_IsDead)
		return DEAD_OBJECT;

	return _uint();
}

_uint CHP_Bar::Render_GameObject()
{
	CUI::Render_GameObject();

	return _uint();
}

void CHP_Bar::Set_ScaleX(_float _fDamage)
{
	m_pTransform->Set_ScaleX(m_pTransform->Get_TransformDesc().vScale.x + _fDamage);
}

_uint CHP_Bar::Movement(_float fDeltaTime)
{
	return _uint();
}

_uint CHP_Bar::Adjust_Pos(_float fDeltaTime)
{
	wstring dstLayerTag = L"";
	_float2 vHpOffset = { 0.f,0.f };

	switch (m_eMakerID)
	{
	case CHP_Bar::MAKER_PLAYER:
		if (m_pTransform) {
			m_pTransform->Set_Position(_float3(-(WINCX / 2.f) + 132.5f, WINCY / 2.f - 84.f, 0.f));
			m_pTransform->Update_Transform();
		}
		return _uint();
	case CHP_Bar::MAKER_BOSS_MONSTER:
		dstLayerTag = L"Layer_Boss_Monster";
		vHpOffset = {-30.f, 30.f};
		break;
	case CHP_Bar::MAKER_MONSTER:
		break;
	case CHP_Bar::MAKER_DRONE:
		dstLayerTag = L"Layer_Drone";
		break;
	}

	if (dstLayerTag == L"") return -1;

	const list<class CGameObject*>* dstObjList = m_pManagement->Get_GameObjectList(dstLayerTag);
	if (nullptr == dstObjList) return -1;

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
		m_pTransform->Set_Position(_float3(ptBoss.x - (WINCX / 2.f) + vHpOffset.x, -ptBoss.y + (WINCY / 2.f) + vHpOffset.y, 0.f));
		m_pTransform->Update_Transform();
	}

	return _uint();
}

_uint CHP_Bar::Who_Make_Me(MAKERID _iMakerName)
{
	m_eMakerID = _iMakerName;
	return _uint();
}

CHP_Bar * CHP_Bar::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CHP_Bar* pInstance = new CHP_Bar(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create HP_Bar");
		Safe_Release(pInstance);
	}
	
	return pInstance;
}

CGameObject * CHP_Bar::Clone(void * pArg/* = nullptr*/)
{
	CHP_Bar* pClone = new CHP_Bar(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone HP_Bar");
		Safe_Release(pClone);
	}
	
   	return pClone;
}

void CHP_Bar::Free()
{
	CUI::Free();
}

