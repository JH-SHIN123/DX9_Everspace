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
	for (auto& pObject : m_listTargetObject)
	{
		Safe_Release(pObject);
	}
	m_listTargetObject.clear();

	Safe_Release(m_pPlayerTransform);
}

HRESULT CQuestHandler::Set_Start_Quest(EQuest eQuest)
{
	if (m_IsClear = false)
		return E_FAIL;

	for (auto& pObject : m_listTargetObject)
	{
		Safe_Release(pObject);
	}
	m_listTargetObject.clear();

	Safe_Release(m_pPlayerTransform);

	m_eNowQuest = eQuest;

	switch (eQuest)
	{
	case Stage_1_Ring:
		m_IsClear = false;
		m_iCount = 0;
		m_iCount_Max = (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Ring"))->size();
		m_wstrQuestName = L"고리를 통과하라";
		m_listTargetObject = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Ring"));
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
		break;
	case Stage_1_Target:
		break;
	default:
		break;
	}

	return S_OK;
}

_int CQuestHandler::Set_Counting(const _int iCount)
{
	Update_Quest();

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
