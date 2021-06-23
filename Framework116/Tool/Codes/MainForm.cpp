// ../Codes/MainForm.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "../Headers/MainForm.h"


// CMainForm

IMPLEMENT_DYNCREATE(CMainForm, CFormView)

CMainForm::CMainForm()
	: CFormView(IDD_CMainForm)
{

}

CMainForm::~CMainForm()
{
}

void CMainForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainForm::OnBnObjectToolButton1)
END_MESSAGE_MAP()


// CMainForm 진단

#ifdef _DEBUG
void CMainForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainForm 메시지 처리기


void CMainForm::OnBnObjectToolButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	if (nullptr == m_tObjectTool.GetSafeHwnd())
		m_tObjectTool.Create(IDD_OBJECTTOOL);
	m_tObjectTool.ShowWindow(SW_SHOW);
}
