#pragma once
#ifndef __TIME_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
class CTime_Manager final : public CBase
{
	DECLARE_SINGLETON(CTime_Manager)

private:
	CTime_Manager();
	virtual ~CTime_Manager() = default;

public:
	_float Get_DeltaTime() const;
	_float Update_Time_Manager(); 

public:
	virtual void Free() override;

private:
	LARGE_INTEGER m_CpuTick; 
	LARGE_INTEGER m_BeginTime; 
	LARGE_INTEGER m_EndTime; 
	float m_fDeltaTime; 
};
END

#define __TIME_MANAGER_H__
#endif