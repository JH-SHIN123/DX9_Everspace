#include "stdafx.h"
#include "..\Headers\QuestHandler.h"
#include "Player.h"
#include "CollisionHandler.h"
#include "Delivery.h"
#include "Player.h"
#include "Monster.h"
#include "Sniper.h"
#include "Boss_Monster.h"
#include "Sniper_Bullet.h"
#include "Bullet_EMP_Bomb.h"
#include "Bullet_EnergyBall.h"
#include "Bullet_Laser.h"
#include "Planet.h"
#include "Asteroid.h"
#include "ScriptUI.h"

IMPLEMENT_SINGLETON(CQuestHandler)

CQuestHandler::CQuestHandler()
{
}

CQuestHandler::~CQuestHandler()
{
}

HRESULT CQuestHandler::Set_Start_Quest(EQuest eQuest)
{
	Release_Ref();

	m_eNowQuest = eQuest;
	m_IsClear = false;
	m_IsRetry = false;
	m_IsArrivePlayer = false;
	m_IsArriveObject = false;
	//m_IsSpecial_Script = false;
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
		m_iCount_Max = 15;
	}
	break;

	case EQuest::Stage_2_Rescue:
	{
		m_wstrQuestName = L"아군을 구조하라";
		m_iCount_Max = 1;
	}
	break;

	case EQuest::Stage_3_Delivery:
	{
		m_wstrQuestName = L"화물을 호위하라";
		m_iCount_Max = 1;
	}
	break;
	case EQuest::Stage_3_Boss:
	{
		m_wstrQuestName = L"보스를 격파하라";
		m_iCount_Max = 1;
	}
	break;
	default:
		break;
	}

	// 여기서 레퍼런스 카운팅
	if (m_listTargetObject.empty() && m_eNowQuest != EQuest::Stage_2_Dodge&&m_eNowQuest != EQuest::Stage_2_Rescue)
	{
		if (m_eNowQuest == EQuest::Stage_3_Delivery ||
			m_eNowQuest == EQuest::Stage_3_Boss)
			return S_OK;

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
		return true;

	return false;
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

_bool CQuestHandler::Get_IsRetry()
{
	return m_IsRetry;
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

_bool CQuestHandler::Get_IsSpecial_Script(EQuest eQuest)
{
	switch (m_eNowQuest)
	{
	case EQuest::Stage_1_Ring:
	{
		if (m_iCount == 3)
			return TRUE;
	}
		break;
	case EQuest::Stage_1_Target:
		break;
	case EQuest::Stage_2_Dodge:
		break;
	case EQuest::Stage_2_Rescue:
		break;
	case EQuest::Stage_3_Delivery:
		break;
	case EQuest::Stage_3_Boss:
		break;
	case EQuest::End:
		break;
	default:
		break;
	}

	return FALSE;
}

_bool CQuestHandler::Get_IsPlayer_Dead()
{
	return m_IsArrivePlayer;
}

_bool CQuestHandler::Get_IsObject_Dead()
{
	return m_IsArriveObject;
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
	case EQuest::Stage_2_Dodge:
		Update_Quest_Stage2_Dodge();
		break;
	case EQuest::Stage_2_Rescue:
		Update_Quest_Stage2_Resque();
		break;
	case EQuest::Stage_3_Delivery:
		Update_Quest_Stage3_Delivery();
		break;
	case EQuest::Stage_3_Boss:
		Update_Quest_Stage3_Boss();
		break;
	default:
		break;
	}

	if (m_iCount >= m_iCount_Max)
	{
		m_iCount = m_iCount_Max;
		m_IsClear = true;
	}

	if (m_IsArrivePlayer == false)
		m_IsArrivePlayer = ((CPlayer*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Player"))->Get_IsDead();

	m_IsRetry = m_IsArrivePlayer;

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

void CQuestHandler::Lock_MonsterAI(_bool bLock)
{
	// true 일떄 움직임을 false로 멈춘다
	
	if (true == bLock)
	{
		list<CGameObject*> pList;
	
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Monster") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Monster")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Monster"));

				for (auto& iter : pList)
				{
					static_cast<CMonster*>(iter)->Set_IsFight(false);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper"));

				for (auto& iter : pList)
				{
					static_cast<CSniper*>(iter)->Set_IsFight(false);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Boss_Monster") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Boss_Monster")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Boss_Monster"));

				for (auto& iter : pList)
				{
					static_cast<CBoss_Monster*>(iter)->Set_BossFight(false);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Planet") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Planet")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Planet"));

				for (auto& iter : pList)
				{
					static_cast<CPlanet*>(iter)->Set_IsMove(false);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid"));

				for (auto& iter : pList)
				{
					static_cast<CAsteroid*>(iter)->Set_IsMove(false);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid_Deco") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid_Deco")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid_Deco"));

				for (auto& iter : pList)
				{
					static_cast<CAsteroid*>(iter)->Set_IsMove(false);
				}
			}
		}


		// 몬스터 총알 지우기
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_EnergyBall") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_EnergyBall")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_EnergyBall"));

				for (auto& iter : pList)
				{
					iter->Set_IsDead(TRUE);
				}
			}
		}

		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_Laser") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_Laser")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_Laser"));

				for (auto& iter : pList)
				{
					iter->Set_IsDead(TRUE);
				}
			}
		}

		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_EMP_Bomb") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_EMP_Bomb")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Bullet_EMP_Bomb"));

				for (auto& iter : pList)
				{
					iter->Set_IsDead(TRUE);
				}
			}
		}

		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper_Bullet") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper_Bullet")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper_Bullet"));

				for (auto& iter : pList)
				{
					iter->Set_IsDead(TRUE);
				}
			}
		}

	}




	else if (false == bLock)
	{
		list<CGameObject*> pList;
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Monster") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Monster")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Monster"));

				for (auto& iter : pList)
				{
					static_cast<CMonster*>(iter)->Set_IsFight(true);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Sniper"));

				for (auto& iter : pList)
				{
					static_cast<CSniper*>(iter)->Set_IsFight(true);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Boss_Monster") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Boss_Monster")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Boss_Monster"));

				for (auto& iter : pList)
				{
					static_cast<CBoss_Monster*>(iter)->Set_BossFight(true);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Planet") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Planet")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Planet"));

				for (auto& iter : pList)
				{
					static_cast<CPlanet*>(iter)->Set_IsMove(true);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid"));

				for (auto& iter : pList)
				{
					static_cast<CAsteroid*>(iter)->Set_IsMove(true);
				}
			}
		}
		if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid_Deco") != nullptr)
		{
			if (CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid_Deco")->empty() == false)
			{
				pList = *(CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Asteroid_Deco"));

				for (auto& iter : pList)
				{
					static_cast<CAsteroid*>(iter)->Set_IsMove(true);
				}
			}
		}

	}
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
	if (CManagement::Get_Instance()->Get_GameObject(L"Layer_Player"))
	{
		if (CManagement::Get_Instance()->Get_GameObject(L"Layer_Player")->Get_IsDead())
			return;
	}
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

void CQuestHandler::Update_Quest_Stage3_Delivery()
{
	m_IsClear = false;

	m_IsArriveObject = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Delivery")->empty();

	if (m_IsArriveObject == true)
	{
		m_IsClear = false;
		return;
	}

	m_IsClear = ((CDelivery*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Delivery"))->Get_IsArrive();

	if (m_IsClear == true)
		m_iCount = m_iCount_Max;
}

void CQuestHandler::Update_Quest_Stage3_Boss()
{
	if (m_IsClear == false)
	{
		m_IsArriveObject = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Delivery")->empty();

		if (m_IsArriveObject == true)
		{
			m_IsClear = false;
			return;
		}

		m_IsClear = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Boss_Monster")->empty();

		if (m_IsClear == true)
			m_iCount = m_iCount_Max;
	}

}
