#pragma once

#ifndef __UIHANDLER_H__

#define QUEST_NOTCLEAR	0
#define QUEST_CLEAR		1
#define QUEST_FAIELD	2

USING(Engine)

enum Quest {
	Stage_1_Ring, Stage_1_Target,
	End
};


class CQuestHandler
{
	DECLARE_SINGLETON(CQuestHandler)

private:
	CQuestHandler();
	~CQuestHandler();

public:
	HRESULT Start_Quest(const _int& iCount_Max, const wstring& wstrQuestName = L"");
	_int	Set_Counting(const _int iCount = 1);

public:
	const wstring& Get_QusetName() const;
	_int  Get_CountRemaining();
	_int  Get_CountMax();
	_bool Get_IsClear();

private:
	_bool Update_Quest();

private:
	Quest	m_eNowQuest = End;
	wstring m_wstrQuestName = L"";
	_bool	m_IsClear = true;
	_int	m_iCount = 0;
	_int	m_iCount_Max = 0;
};

#define __UIHANDLER_H__
#endif // !__UIHANDLER_H__
