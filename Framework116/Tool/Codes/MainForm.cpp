// ../Codes/MainForm.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "../Headers/MainForm.h"


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

void CMainForm::OnBnClickedButton_MeshTool()
{
	if (nullptr == m_MeshTool.GetSafeHwnd())
		m_MeshTool.Create(IDD_CMeshTool);
	m_MeshTool.ShowWindow(SW_SHOW);
}

void CMainForm::OnBnClickedButton_CollideTool()
{
	if (nullptr == m_CollideTool.GetSafeHwnd())
		m_CollideTool.Create(IDD_CCollideTool);
	m_CollideTool.ShowWindow(SW_SHOW);
}

BEGIN_MESSAGE_MAP(CMainForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CMainForm::OnBnClickedButton_MeshTool)
	ON_BN_CLICKED(IDC_BUTTON2, &CMainForm::OnBnClickedButton_CollideTool)
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
