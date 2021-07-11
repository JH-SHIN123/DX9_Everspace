#include "Frame_Manager.h"
#include "Time_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CFrame_Manager)

CFrame_Manager::CFrame_Manager()
	:m_fSPF(0.f)
	, m_fDeltaTime(0.f)
	, m_BeginTime({})
	, m_EndTime({})
	, m_CpuTick({})
	, m_fFPSTime(0.f)
	, m_iFPS(0)
	, m_szFPS(L"")
{
}

HRESULT CFrame_Manager::Ready_FrameManager(const _float fSPF)
{
	m_fSPF = 1.f / fSPF;

	BOOL isSuccess = FALSE;
	isSuccess = QueryPerformanceFrequency(&m_CpuTick);
	isSuccess = QueryPerformanceCounter(&m_BeginTime);

	if (false == isSuccess)
		return E_FAIL;

	return S_OK;
}

HRESULT CFrame_Manager::FrameLock()
{
	QueryPerformanceCounter(&m_EndTime);
	m_fDeltaTime += float(m_EndTime.QuadPart - m_BeginTime.QuadPart) / m_CpuTick.QuadPart;
	m_BeginTime.QuadPart = m_EndTime.QuadPart;
	if (m_fDeltaTime >= m_fSPF)
	{
		++m_iFPS;
		QueryPerformanceFrequency(&m_CpuTick);
		m_fDeltaTime = 0.f;
		return true;
	}
	return false;
}

void CFrame_Manager::ShowFrame(const HWND hWnd)
{
	m_fFPSTime += CTime_Manager::Get_Instance()->Get_DeltaTime();
	if (1.f <= m_fFPSTime)
	{
		swprintf_s(m_szFPS, L"FPS : %d", m_iFPS);
		m_iFPS = 0;
		m_fFPSTime = 0.f;
	}

	SetWindowText(hWnd, m_szFPS);
}

void CFrame_Manager::Free()
{
}
