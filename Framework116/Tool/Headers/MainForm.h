#pragma once
#include "MapTool.h"


// CMainForm 폼 보기
#include"ResourceTool.h"
class CMainForm : public CFormView
{
	DECLARE_DYNCREATE(CMainForm)

protected:
	CMainForm();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMainForm();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CMainForm };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
<<<<<<< HEAD
	CResourceTool m_tResourceTool;
=======
	CMapTool m_tMapTool;

>>>>>>> origin/New_YoonYoung
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()
<<<<<<< HEAD

public:
	afx_msg void OnBnClickedResourceToolButton();
=======
public:
	afx_msg void OnBnClickedMapTool();
>>>>>>> origin/New_YoonYoung
};


