#pragma once
#ifndef __DEVICE_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
enum class EDisplayMode
{ 
	Full,
	Win
};

class CDevice_Manager final : public CBase
{
public:
	DECLARE_SINGLETON(CDevice_Manager)

private:
	CDevice_Manager();
	virtual ~CDevice_Manager() = default;

public:
	LPDIRECT3DDEVICE9 Get_Device() const;
	LPD3DXFONT		  Get_Font()const;

public:
	//virtual void Func() final; /* virtual 함수에 final 키워드가 붙어있으면 더이상 자식이 오버라이딩 할 수 없다.  */
	void Render_Begin();
	void Render_End(HWND hWnd = nullptr);

public:
	HRESULT Ready_Graphic_Device(HWND hWnd, _uint iWinCX, _uint iWinCY, EDisplayMode eMode = EDisplayMode::Win);

public:
	virtual void Free() override;

private:

	// com 
	// 장치를 초기화 하는 과정 
	// 장치의 수준을 조사하기 위한 컴객체 생성. 
	// 장치의 지원 수준을 조사. 
	// 조사한 수준을 바탕으로 장치를 제어하는 컴객체 생성. 
	LPDIRECT3D9			m_pSDK;
	LPDIRECT3DDEVICE9	m_pDevice;
	LPD3DXFONT			m_pFont;
	// com 객체는 말그대로 부품이라고 생각하면 된다. 
	// 우리는 이 컴객체를 이용하여 마치 레고 조립하듯 프로그램을 완성해 나가야 한다. 

};
END

#define __DEVICE_MANAGER_H__
#endif