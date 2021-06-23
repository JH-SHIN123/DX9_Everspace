
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "Tool.h"

#include "MainFrm.h"
#include "MainView.h"
#include "MainForm.h"
#include "MiniView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;




	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	//cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	//cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;

}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 뷰 창으로 포커스를 이동합니다.
	//m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 뷰에서 첫째 크랙이 해당 명령에 나타나도록 합니다.
	//if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//	return TRUE;

	// 그렇지 않으면 기본 처리합니다.
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// 0, 0		0,	1
	// 1, 0		1,	1
	m_tMainSplitterWnd.CreateStatic(this, 1, 2);
	m_tSecondSplitterWnd.CreateStatic(&m_tMainSplitterWnd, 2, 1, WS_VISIBLE | WS_CHILD, m_tMainSplitterWnd.IdFromRowCol(0, 0));
	m_tSecondSplitterWnd.CreateView(0, 0, RUNTIME_CLASS(CMiniView), CSize(MINIVIEW_WINCX, MINIVIEW_WINCY), pContext);
	m_tSecondSplitterWnd.CreateView(1, 0, RUNTIME_CLASS(CMainForm), CSize(MAINFORM_WINCX, MAINFORM_WINCY), pContext);
	m_tMainSplitterWnd.SetColumnInfo(0, MAINFORM_WINCX, 10);

	m_tMainSplitterWnd.CreateView(0, 1, RUNTIME_CLASS(CMainView), CSize(MAINVIEW_WINCX, MAINVIEW_WINCY), pContext);

	return TRUE;
	//return CFrameWnd::OnCreateClient(lpcs, pContext);
}
