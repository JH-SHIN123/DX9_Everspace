#include "Device_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CDevice_Manager)

CDevice_Manager::CDevice_Manager()
	: m_pDevice(nullptr)
	, m_pSDK(nullptr)
{
}
LPDIRECT3DDEVICE9 CDevice_Manager::Get_Device() const
{
	return m_pDevice;
}
//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!
//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!
void CDevice_Manager::Render_Begin()
{
	m_pDevice->Clear(
		0, nullptr, 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(255, 255, 0, 255), 1.f, 0);
	m_pDevice->BeginScene();	
}
//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!
void CDevice_Manager::Render_End(HWND hWnd/*= nullptr*/)
{	
	m_pDevice->EndScene();
	m_pDevice->Present(nullptr, nullptr, hWnd, nullptr);
}
//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!
HRESULT CDevice_Manager::Ready_Graphic_Device(HWND hWnd, _uint iWinCX, _uint iWinCY, EDisplayMode eMode)
{
	// 	// 장치를 초기화 하는 과정 
	// 장치의 수준을 조사하기 위한 컴객체 생성. 
	// 장치의 지원 수준을 조사. 
	// 조사한 수준을 바탕으로 장치를 제어하는 컴객체 생성. 
	D3DCAPS9 caps;
	ZeroMemory(&caps, sizeof(D3DCAPS9));

	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	if (nullptr == m_pSDK)
	{
		PRINT_LOG(L"Error", L"Failed to Direct3DCreate9");
		return E_FAIL;
	}

	//D3DADAPTER_DEFAULT - 현재 진단도구에 나열된 그래픽 카드의 값을 사용하겠다. 
	//D3DDEVTYPE_HAL - HAL이라는 추상계층에서 데이터를 얻어오겠다 라는 뜻. 
	// 마지막 인자는 얻어온 데이터를 받아올 공간. 
	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps)))
	{
		PRINT_LOG(L"Error", L"Failed to GetDeviceCaps");
		return E_FAIL;
	}
	//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!
	DWORD vp = 0;
	//1000 0000
	//1011 0110
	//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!! 
	//D3DCREATE_MULTITHREADED 쓰레드 쓰면서 이 인자를 넣어주지 않으면 터지거나 급격한 프레임 드랍이 발생!
	if (caps.DevCaps &  D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	//D3DCREATE_MULTITHREADED 쓰레드 쓰면서 이 인자를 넣어주지 않으면 터지거나 급격한 프레임 드랍이 발생!
	else
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	//D3DCREATE_MULTITHREADED 쓰레드 쓰면서 이 인자를 넣어주지 않으면 터지거나 급격한 프레임 드랍이 발생!

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth = iWinCX;
	d3dpp.BackBufferHeight = iWinCY;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;
	//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	// D3DSWAPEFFECT_COPY - 너네가 이때까지 사용하던 더블 버퍼링 방식을 사용하겠다. 
	//D3DSWAPEFFECT_DISCARD - 스왑체인 방식을 사용하겟다. 
	// 스왑체인이라는건 전면과 후면을 번갈아가며 화면을 출력하는 방식. 
	// 복사하는 방식이 아니라 더블버퍼링보다 빠르다. 
	// 쉽게 말하면 더블버퍼링의 상위 호환이라 생각하면 편하다. 
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = (BOOL)eMode;// 전체화면을 하겠다 라면 FALSE, 창모드라면 TRUE
	// 깊이 버퍼와 스텐실을 사용할 것이냐 라는 것을 묻고 있다. 
	// 당장은 2D라서 깊이가 필요 없으며 
	// 2D에서는 그려지는 그려야 할 것이고 나발이고 없이 오로지 그려지는 순서에 의해서 출력이 되는 형식이기 때문에 
	// 당장은 사용하지 않는다 스텐실도 .
	// 그렇지만 나중 3D넘어가서 사용할 것이기 때문에 TRUE로 넣어줌. 그냥 신경꺼. 

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;


	/* FullScreen_RefreshRateInHz must be zero for Windowed mode */
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!																		//IDirect3DDevice9
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dpp, &m_pDevice)))
	{
		PRINT_LOG(L"Error", L"Failed to CreateDevice");
		return E_FAIL;
	}
	
	return S_OK;
}
//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!

void CDevice_Manager::Free()
{
	//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!
	// 단 순서 주의. 이 순서대로 지워 져야 한다. 	
	if (Safe_Release(m_pDevice))
	{
		PRINT_LOG(L"Warning", L"Failed To Release m_pDevice");
	}
	//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!
	if (Safe_Release(m_pSDK))
	{
		PRINT_LOG(L"Warning", L"Failed To Release m_pSDK");
	}
}
//vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED; 이거 바꿔줘야함!!