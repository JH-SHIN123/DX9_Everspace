#pragma once

#ifndef __DATABASE_H__

#include"Lobby.h"

USING(Engine)

class CDataBase
{
	DECLARE_SINGLETON(CDataBase)

private:
	CDataBase();
	~CDataBase();

public:
	
	_uint Get_Money()const;
	STAT_INFO* Get_StatInfo();

	_uint GetAtkBuffItemCount() { return m_iAtkBuffItem; }
	_uint GetDefBuffItemCount() { return m_iDefBuffItem; }
	_uint GetHpBuffItemCount() { return m_iHpBuffItem; }
	_uint GetEnergyBuffItemCount() { return m_iEnergyBuffItem; }
	_uint GetMissileCount() { return m_iMissileCount; }
	_uint GetVMaxBuffItem() { return m_iVMaxBuffItem; }

	void SetAtkBuffItemCount(_uint iPlus) { m_iAtkBuffItem += iPlus; }
	void SetDefBuffItemCount(_uint iPlus) { m_iDefBuffItem += iPlus; }
	void SetHpBuffItemCount(_uint iPlus) { m_iHpBuffItem += iPlus; }
	void SetEnergyBuffItemCount(_uint iPlus) { m_iEnergyBuffItem += iPlus; }
	void SetMissileCount(_uint iPlus) { m_iMissileCount += iPlus; }
	void SetVMaxBuffItemCount(_uint iPlus) { m_iVMaxBuffItem += iPlus; }
	void Set_StatInfo(STAT_INFO _tUnitInfo);
	void Set_Money(_uint _iMoney);
private:
	_uint m_iMoney = 300000;
	STAT_INFO m_tStatInfo;

	_uint m_iAtkBuffItem = 10;
	_uint m_iDefBuffItem = 10;
	_uint m_iHpBuffItem = 10;
	_uint m_iEnergyBuffItem = 10;
	_uint m_iMissileCount = 10;
	_uint m_iVMaxBuffItem = 10;

};

#define __DATABASE_H__
#endif // !__UIHANDLER_H__
