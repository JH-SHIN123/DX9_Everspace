#include "stdafx.h"
#include "..\Headers\DataBase.h"

IMPLEMENT_SINGLETON(CDataBase)

CDataBase::CDataBase()
{
}

CDataBase::~CDataBase()
{}

_uint CDataBase::Get_Money() const
{
	return m_iMoney;
}

UNIT_INFO * CDataBase::Get_UnitInfo()
{
	return &m_tUnitInfo;
}

void CDataBase::Set_UnitInfo(UNIT_INFO _tUnitInfo)
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
	m_tUnitInfo = _tUnitInfo;
}

void CDataBase::Set_Money(_uint _iMoney)
{
	m_iMoney += _iMoney;
}

