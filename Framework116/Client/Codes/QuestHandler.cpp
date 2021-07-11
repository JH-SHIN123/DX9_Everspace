#include "stdafx.h"
#include "..\Headers\QuestHandler.h"
#include "TutorialUI.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CQuestHandler)

CQuestHandler::CQuestHandler()
{
}

CQuestHandler::~CQuestHandler()
{
}

HRESULT CQuestHandler::Set_Start_Quest(EQuest eQuest)
{
	if (m_IsClear == false)
		return E_FAIL;

	Release_Ref();

	m_eNowQuest = eQuest;
	m_IsClear = false;
	m_bAllTargetCollide = false;
	m_iCount = 0;

	switch (eQuest)
	{
	case Stage_1_Ring:
	{
		m_wstrQuestName = L"고리를 통과하라";
		m_iCount_Max = (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Ring"))->size();
		m_listTargetObject = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Ring"));
	}
		break;
	case Stage_1_Target:
	{
		m_wstrQuestName = L"과녁을 파괴하라";
		m_iCount_Max = (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Drone"))->size();
		m_listTargetObject = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Drone"));
	}

		break;
	default:
		break;
	}

	// 여기서 레퍼런스 카운팅
	if (m_listTargetObject.empty())
	{
		PRINT_LOG(L"Error", L"m_listTargetObject is empty");
		return E_FAIL;
	}
	else
	{
		for (auto& iter : m_listTargetObject)
			Safe_AddRef(iter);
	}
	m_pPlayerTransform = (CTransform*)CManagement::Get_Instance()->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

_int CQuestHandler::Set_Counting(const _int iCount)
{
	m_iCount += iCount;

	if (m_IsClear == true)
		return QUEST_CLEAR;

	return QUEST_NOTCLEAR;
}

const wstring& CQuestHandler::Get_QusetName() const
{
	return m_wstrQuestName;
}

_int CQuestHandler::Get_CountRemaining()
{
	return m_iCount;
}

_int CQuestHandler::Get_CountMax()
{
	return m_iCount_Max;
}

_bool CQuestHandler::Get_IsClear()
{
	return m_IsClear;
}

_bool CQuestHandler::Update_Quest()
{
	switch (m_eNowQuest)
	{
	case Stage_1_Ring:
		Update_Quest_Stage1_Ring();
		break;
	case Stage_1_Target:
		Update_Quest_Stage1_Target();
		break;
	default:
		break;
	}

	m_IsClear = m_bAllTargetCollide;


	if (m_iCount >= m_iCount_Max)
	{
		m_iCount = m_iCount_Max;
		m_IsClear = QUEST_CLEAR;
	}

	return m_IsClear;
}

void CQuestHandler::Release_Ref()
{
	if (m_listTargetObject.empty() == false)
	{
		for (auto& pObject : m_listTargetObject)
		{
			Safe_Release(pObject);
		}
		m_listTargetObject.clear();
	}


	if (m_pPlayerTransform != nullptr)
		Safe_Release(m_pPlayerTransform);

}

void CQuestHandler::Update_Quest_Stage1_Ring()
{
	m_bAllTargetCollide = true;
	_float3 fPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_uint i = 0;
	m_iCount = 0;
	for (auto& iter : m_listTargetObject)
	{
		++m_iCount;

		if (iter->Get_IsCollide() == false)
		{
			m_bAllTargetCollide = false;
			_float3 vPos;
			vPos = ((CTransform*)(iter->Get_Component(L"Com_Transform")))->Get_State(EState::Position);

			_float fDis = fabs(D3DXVec3Length(&(vPos - fPlayerPos)));

			m_vSearchTagetDis[i] = { vPos.x, vPos.y, vPos.z, fDis };
			++i;
		}
	}
	m_iCount -= i;


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

}

void CQuestHandler::Update_Quest_Stage1_Target()
{
	m_iCount = 0;
	for (auto& iter : m_listTargetObject)
	{
		if(iter->Get_IsDead() == true)
			++m_iCount;
	}
}
