#include "stdafx.h"
#include "..\Headers\QuestHandler.h"
#include "Player.h"
#include "CollisionHandler.h"

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
	m_iCount = 0;
	m_iCount_Max = 0;
	m_fTimer = 0.f;

	switch (eQuest)
	{
	case EQuest::Stage_1_Ring:
	{
		m_wstrQuestName = L"고리를 통과하라";
		m_iCount_Max = (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Ring"))->size();
		m_listTargetObject = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Ring"));
	}
	break;

	case EQuest::Stage_1_Target:
	{
		m_wstrQuestName = L"과녁을 파괴하라";
		m_iCount_Max = (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Drone"))->size();
		m_listTargetObject = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Drone"));
	}
	break;

	case EQuest::Stage_2_Dodge:
	{
		m_wstrQuestName = L"운석을 피하라";
		m_iCount_Max = 20;
	}
	break;

	case EQuest::Stage_2_Rescue:
	{
		m_wstrQuestName = L"아군을 구조하라";
		m_iCount_Max = 1;
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

_int CQuestHandler::Set_ClearStage(EStageClear eClearStage)
{
	if (EStageClear::End <= eClearStage)
		return UPDATE_ERROR;

	m_eStageClear = eClearStage;
	m_bClear[(_uint)eClearStage] = false;

	return NO_ERROR;
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

_bool CQuestHandler::Get_IsStageLocked(_uint iStageIdx)
{
	return m_bClear[iStageIdx];
}

_bool CQuestHandler::Get_IsStage_1_Locked()
{
	return m_bClear[0];
}

_bool CQuestHandler::Get_IsStage_2_Locked()
{
	return m_bClear[1];
}

_bool CQuestHandler::Get_IsStage_3_Locked()
{
	return m_bClear[2];
}

_bool CQuestHandler::Update_Quest()
{
	switch (m_eNowQuest)
	{
	case EQuest::Stage_1_Ring:
		Update_Quest_Stage1_Ring();
		break;
	case EQuest::Stage_1_Target:
		Update_Quest_Stage1_Target();
		break;
	default:
		break;
	}

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
	_bool bAllTargetCollide = true;
	_float3 fPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_uint i = 0;
	m_iCount = 0;
	for (auto& iter : m_listTargetObject)
	{
		++m_iCount;

		if (iter->Get_IsCollide() == false)
		{
			bAllTargetCollide = false;
			_float3 vPos;
			vPos = ((CTransform*)(iter->Get_Component(L"Com_Transform")))->Get_State(EState::Position);

			_float fDis = fabs(D3DXVec3Length(&(vPos - fPlayerPos)));

			m_vSearchTagetDis[i] = { vPos.x, vPos.y, vPos.z, fDis };
			++i;
		}
	}

	m_iCount -= i;
	m_IsClear = bAllTargetCollide;
}

void CQuestHandler::Update_Quest_Stage1_Target()
{
	m_iCount = 0;
	for (auto& iter : m_listTargetObject)
	{
		if (iter->Get_IsDead() == true)
			++m_iCount;
	}
}

void CQuestHandler::Update_Quest_Stage2_Dodge()
{
	if (m_IsClear == true)
		return;

	_float fTime = CManagement::Get_Instance()->Get_DeltaTime();

	m_fTimer += fTime;
	m_iCount = (_uint)m_fTimer;

	if (m_fTimer >= (_float)m_iCount_Max)
	{
		m_iCount = m_iCount_Max;
		m_IsClear = true;
	}
}

void CQuestHandler::Update_Quest_Stage2_Resque()
{
	if (m_IsClear == false)
	{
		m_IsClear = CCollisionHandler::Collision_PlayerToObject(L"Layer_Player", L"Layer_Broken_Plane");

		if (m_IsClear == true)
		{
			m_iCount = m_iCount_Max;
		}
	}
}
