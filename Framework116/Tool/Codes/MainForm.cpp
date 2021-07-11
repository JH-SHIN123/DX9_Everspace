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

void CMainForm::OnBnClickedResourceToolButton()
{
	if (nullptr == m_tResourceTool.GetSafeHwnd())
		m_tResourceTool.Create(IDD_RESOURCETOOL);
	m_tResourceTool.ShowWindow(SW_SHOW);
}

void CMainForm::OnBnClickedUiToolButton()
{
	if (nullptr == m_tUiTool.GetSafeHwnd())
		m_tUiTool.Create(IDD_UITOOL);
	m_tUiTool.m_pListResource = &m_tResourceTool.m_ListResource;
	m_tUiTool.ShowWindow(SW_SHOW);
	
}

BEGIN_MESSAGE_MAP(CMainForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON2, &CMainForm::OnBnClickedResourceToolButton)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainForm::OnBnClickedUiToolButton)
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
