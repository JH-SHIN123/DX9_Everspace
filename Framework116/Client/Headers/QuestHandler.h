#pragma once

#ifndef __UIHANDLER_H__

USING(Engine)

enum class EQuest {
	Stage_1_Ring = 0,
	Stage_1_Target,
	Stage_2_Dodge,
	Stage_2_Rescue,
	Stage_3_Delivery,
	Stage_3_Boss,
	End
};

enum class EStageClear {
	Stage_1 = 0,
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
	_bool Get_IsRetry();
	_bool Get_IsStageLocked(_uint iStageIdx);
	_bool Get_IsStage_1_Locked();
	_bool Get_IsStage_2_Locked();
	_bool Get_IsStage_3_Locked();

public: // 로비씬 가야할 때
	_bool Get_IsPlayer_Dead();
	_bool Get_IsObject_Dead();

public: // 계속 돌려줘야함
	_bool Update_Quest();
	void Release_Ref();

private:
	void Update_Quest_Stage1_Ring();
	void Update_Quest_Stage1_Target();
	void Update_Quest_Stage2_Dodge();
	void Update_Quest_Stage2_Resque();
	void Update_Quest_Stage3_Delivery();
	void Update_Quest_Stage3_Boss();

private: // 퀘스트 
	EQuest	m_eNowQuest = EQuest::End;
	wstring m_wstrQuestName = L"";
	_bool	m_IsClear = true;
	_int	m_iCount = 0;
	_int	m_iCount_Max = 0;
	_float	m_fTimer = 0.f;
	_bool	m_IsRetry = false;
	_bool	m_IsArrivePlayer = false;
	_bool	m_IsArriveObject = false;

private: // 스테이지
	EStageClear m_eStageClear = EStageClear::End;
	_bool m_bClear[3] = { true, true, true };

private:
	list<class CGameObject*> m_listTargetObject;
	CTransform* m_pPlayerTransform = nullptr;
	_float4 m_vSearchTagetDis[20];
};

#define __UIHANDLER_H__
#endif // !__UIHANDLER_H__
