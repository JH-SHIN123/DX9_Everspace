#pragma once
#ifndef __FRAME_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
class CFrame_Manager final : public CBase
{
	DECLARE_SINGLETON(CFrame_Manager)

private:
	CFrame_Manager();
	virtual ~CFrame_Manager() = default;

public:
	HRESULT Ready_FrameManager(const _float fSPF);
	HRESULT FrameLock();
	void ShowFrame(const HWND hWnd);

public:
	virtual void Free() override;

private:
	LARGE_INTEGER m_BeginTime;
	LARGE_INTEGER m_EndTime;
	LARGE_INTEGER m_CpuTick;
	_float m_fDeltaTime;

	_float m_fSPF;

	_int m_iFPS;
	TCHAR m_szFPS[32];
	_float m_fFPSTime;
};
END

#define __FRAME_MANAGER_H__
#endif