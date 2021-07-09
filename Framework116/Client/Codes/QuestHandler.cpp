#include "stdafx.h"
#include "..\Headers\QuestHandler.h"
#include "TutorialUI.h"


IMPLEMENT_SINGLETON(CQuestHandler)

CQuestHandler::CQuestHandler()
{
}

CQuestHandler::~CQuestHandler()
{
}

HRESULT CQuestHandler::Set_Start_Quest(EQuest eQuest)
{
	if (m_IsClear = false)
		return E_FAIL;

	switch (eQuest)
	{
	case Stage_1_Ring:
		m_IsClear = false;
		m_iCount = 0;
		m_iCount_Max = (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Ring"))->size();
		m_wstrQuestName = L"Mission Ring!";
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
	m_iCount += iCount;

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
	Update_Quest();

	return m_iCount;
}

_int CQuestHandler::Get_CountMax()
{
	Update_Quest();

	return m_iCount_Max;
}

_bool CQuestHandler::Get_IsClear()
{
	Update_Quest();

	return m_IsClear;
}

_bool CQuestHandler::Update_Quest()
{
	m_iCount = ((CTutorialUI*)CManagement::Get_Instance()->Get_GameObject(L"Layer_TutorialUI"))->Get_CountTarget();

	if (m_iCount >= m_iCount_Max)
	{
		m_iCount = m_iCount_Max;
		m_IsClear = QUEST_CLEAR;
	}

	return m_IsClear;
}
