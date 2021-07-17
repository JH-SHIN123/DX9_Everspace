#include "stdafx.h"
#include "..\Headers\DataBase.h"

IMPLEMENT_SINGLETON(CDataBase)

CDataBase::CDataBase()
{
	m_tStatInfo.iAtk = 20;
	m_tStatInfo.iDef = 30;
	m_tStatInfo.iMaxHp = 50;
	m_tStatInfo.iHp = 50;
	m_tStatInfo.iMaxShield = 30;
	m_tStatInfo.iShield = 30;
	m_tStatInfo.iMaxEnergy = 40;
	m_tStatInfo.iEnergy = 40;
	m_tStatInfo.iFireRate = 70;
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

void CDataBase::Set_StatInfo(STAT_INFO _tUnitInfo)
{
	if (_tUnitInfo.iAtk >= 100)
	{
		_tUnitInfo.iAtk = 100;
	}
	if (_tUnitInfo.iDef >= 100)
	{
		_tUnitInfo.iDef = 100;
	}
	if (_tUnitInfo.iMaxHp >= 100)
	{
		_tUnitInfo.iMaxHp = 100;
		_tUnitInfo.iHp = 100;
	}
	if (_tUnitInfo.iMaxShield >= 100)
	{
		_tUnitInfo.iShield = 100;
		_tUnitInfo.iMaxShield= 100;
	}
	if (_tUnitInfo.iFireRate >= 100)
	{
		_tUnitInfo.iFireRate = 100;
	}
	if (_tUnitInfo.iMaxEnergy >= 100)
	{
		_tUnitInfo.iEnergy = 100;
		_tUnitInfo.iMaxEnergy = 100;
	}
	m_tStatInfo = _tUnitInfo;
}

void CDataBase::Set_Money(_uint _iMoney)
{
	m_iMoney += _iMoney;
}

