#pragma once

#ifndef __UIHANDLER_H__

#define QUEST_NOTCLEAR	0
#define QUEST_CLEAR		1
#define QUEST_FAIELD	2

USING(Engine)

enum class EQuest {
	Stage_1_Ring = 0, 
	Stage_1_Target,
	End
};

enum class EStageClear {
	None = 0,
	Stage_1,
	Stage_2,
	Stage_3,
	End
};


class CQuestHandler
{
	DECLARE_SINGLETON(CQuestHandler)

private:
	CQuestHandler();
	~CQuestHandler();

public:
	HRESULT Set_Start_Quest(EQuest eQuest);
	_int	Set_Counting(const _int iCount = 1);
	_int	Set_ClearStage(EStageClear eClearStage);

public:
	const wstring& Get_QusetName() const;
	_int  Get_CountRemaining();
	_int  Get_CountMax();
	_bool Get_IsClear();
	const EStageClear Get_StageClear() const;

public: // 계속 돌려줘야함
	_bool Update_Quest();
	void Release_Ref();

private:
	void Update_Quest_Stage1_Ring();
	void Update_Quest_Stage1_Target();

private:
	EQuest	m_eNowQuest = EQuest::End;
	wstring m_wstrQuestName = L"";
	_bool	m_IsClear = true;
	_int	m_iCount = 0;
	_int	m_iCount_Max = 0;

private:
	EStageClear m_eStageClear = EStageClear::None;

private:
	list<class CGameObject*> m_listTargetObject;
	CTransform* m_pPlayerTransform = nullptr;
	_float4 m_vSearchTagetDis[20];
	_bool m_bAllTargetCollide = false;
};

#define __UIHANDLER_H__
#endif // !__UIHANDLER_H__
