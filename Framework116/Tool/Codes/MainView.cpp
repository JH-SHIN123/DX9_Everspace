// MainView.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "MainView.h"
#include <MainFrm.h>
#include "MainScene.h"

// CMainView

IMPLEMENT_DYNCREATE(CMainView, CScrollView)

CMainView::CMainView() :
	m_pManagement(CManagement::Get_Instance())
{

}

CMainView::~CMainView()
{
	//CManagement::Destroy
	Safe_Release(m_pDevice);
	
	if (Safe_Release(m_pManagement))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Management");
	}
}


BEGIN_MESSAGE_MAP(CMainView, CScrollView)
END_MESSAGE_MAP()


// CMainView 그리기

void CMainView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	RECT rcMain{};
	pMain->GetWindowRect(&rcMain);
	SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	RECT rcView{};
	GetClientRect(&rcView);
	int iGapX = (rcMain.right - rcView.right) + 1;
	int iGapY = (rcMain.bottom - rcView.bottom) + 1;
	pMain->SetWindowPos(nullptr, 0, 0, MAINVIEW_WINCX + iGapX, MAINVIEW_WINCY + iGapY, PM_NOREMOVE);

	// Init Device 
	if (FAILED(m_pManagement->Ready_Game(m_hWnd, MAINVIEW_WINCX, MAINVIEW_WINCY, EDisplayMode::Win)))
	{
		PRINT_LOG(L"Error", L"Failed To Ready Game");
		return;
	}

	m_pDevice = m_pManagement->Get_Device();
	Safe_AddRef(m_pDevice);
	if (nullptr == m_pDevice)
	{
		PRINT_LOG(L"Error", L"m_pDevice is nullptr");
		return;
	}

	if (FAILED(m_pManagement->Setup_CurrentScene((_uint)ESceneType::MainScene, CMainScene::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Setup Logo Scene");
		return;
	}

	if (FAILED(Ready_StaticResources()))
	{
		PRINT_LOG(L"Error", L"Failed To Ready Static Resources");
		return;
	}

	if (FAILED(Setup_DefaultSetting()))
	{
		PRINT_LOG(L"Error", L"Failed To Setup Default Setting");
		return;
	}
}

HRESULT CMainView::Ready_StaticResources()
{
	return S_OK;
}

HRESULT CMainView::Setup_DefaultSetting()
{
	return S_OK;
}

void CMainView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.

	// 툴별로 씬을 나누자.
	m_pManagement->Update_Game();
}


// CMainView 진단
#ifdef _DEBUG
void CMainView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainView 메시지 처리기
