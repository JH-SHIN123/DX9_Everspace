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
	m_tUnitInfo = _tUnitInfo;
}

void CDataBase::Set_Money(_uint _iMoney)
{
	m_iMoney += _iMoney;
}

