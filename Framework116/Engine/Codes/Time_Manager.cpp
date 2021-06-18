#include "Time_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTime_Manager)

CTime_Manager::CTime_Manager()
	:m_fDeltaTime(0.f)
{
	QueryPerformanceFrequency(&m_CpuTick);
	QueryPerformanceCounter(&m_BeginTime); 
	QueryPerformanceCounter(&m_EndTime); 

}

_float CTime_Manager::Get_DeltaTime() const
{
	return m_fDeltaTime;
}

_float CTime_Manager::Update_Time_Manager()
{
	QueryPerformanceFrequency(&m_CpuTick); 
	QueryPerformanceCounter(&m_EndTime);
	m_fDeltaTime = float(m_EndTime.QuadPart - m_BeginTime.QuadPart) / m_CpuTick.QuadPart; 
	m_BeginTime.QuadPart = m_EndTime.QuadPart;

	return m_fDeltaTime;
}

void CTime_Manager::Free()
{
}
