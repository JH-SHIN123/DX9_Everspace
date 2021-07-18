#include "stdafx.h"
#include "..\Headers\DataBase.h"

IMPLEMENT_SINGLETON(CDataBase)

CDataBase::CDataBase()
{

	m_tStatInfo.iAtk = 30;
	m_tStatInfo.iDef = 25;
	m_tStatInfo.iMaxHp = 30;
	m_tStatInfo.iHp = 30;
	m_tStatInfo.iMaxShield = 20;
	m_tStatInfo.iShield = 20;
	m_tStatInfo.iMaxEnergy = 17;
	m_tStatInfo.iEnergy = 17;
	m_tStatInfo.iFireRate = 22;
}

CDataBase::~CDataBase()
{}

_uint CDataBase::Get_Money() const
{
	return m_iMoney;
}

STAT_INFO * CDataBase::Get_StatInfo()
{
	return &m_tStatInfo;
}

void CDataBase::SetAtkBuffItemCount(_uint iPlus)
{
	if (iPlus == -1) {
		m_iAtkBuffItem += iPlus;
	}
	else
		m_iAtkBuffItem += iPlus + rand() % 5 + 4;
}

void CDataBase::SetDefBuffItemCount(_uint iPlus)
{
	if (iPlus == -1) {
		m_iDefBuffItem += iPlus;
	}
	else
		m_iDefBuffItem += iPlus + rand() % 5 + 4;
}

void CDataBase::SetHpBuffItemCount(_uint iPlus)
{
	if (iPlus == -1) {
		m_iHpBuffItem += iPlus;
	}
	else
		m_iHpBuffItem += iPlus + rand() % 5 + 4;
}

void CDataBase::SetEnergyBuffItemCount(_uint iPlus)
{
	if (iPlus == -1) {
		m_iEnergyBuffItem += iPlus;
	}
	else
		m_iEnergyBuffItem += iPlus + rand() % 5 + 4;
}

void CDataBase::SetMissileCount(_uint iPlus)
{
	if (iPlus == -1) {
		m_iMissileCount += iPlus;
	}
	else
		m_iMissileCount += iPlus + rand() % 5 + 4;
}

void CDataBase::SetVMaxBuffItemCount(_uint iPlus)
{
	if (iPlus == -1) {
		m_iVMaxBuffItem += iPlus;
	}
	else
		m_iVMaxBuffItem += iPlus + rand() % 5 + 4;
}

void CDataBase::Set_StatInfo(STAT_INFO _tUnitInfo)
{
	if (_tUnitInfo.iAtk >= 120)
	{
		_tUnitInfo.iAtk = 120;
	}
	if (_tUnitInfo.iDef >= 120)
	{
		_tUnitInfo.iDef = 120;
	}
	if (_tUnitInfo.iMaxHp >= 120)
	{
		_tUnitInfo.iMaxHp = 120;
		_tUnitInfo.iHp = 120;
	}
	if (_tUnitInfo.iMaxShield >= 120)
	{
		_tUnitInfo.iShield = 120;
		_tUnitInfo.iMaxShield= 120;
	}
	if (_tUnitInfo.iFireRate >= 120)
	{
		_tUnitInfo.iFireRate = 120;
	}
	if (_tUnitInfo.iMaxEnergy >= 120)
	{
		_tUnitInfo.iEnergy = 120;
		_tUnitInfo.iMaxEnergy = 120;
	}
	m_tStatInfo = _tUnitInfo;
}

void CDataBase::Set_Money(_uint _iMoney)
{
	m_iMoney += _iMoney;
}

