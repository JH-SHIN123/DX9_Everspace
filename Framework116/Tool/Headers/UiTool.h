#pragma once
#include "afxwin.h"


// CUiTool 대화 상자입니다.
USING(Engine)
class CUiTool : public CDialog
{
	DECLARE_DYNAMIC(CUiTool)

public:
	CUiTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUiTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UITOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();
	DECLARE_MESSAGE_MAP()
public:
	CListBox CTextureList;
	list<PASSDATA_RESOURCE*>* m_pListResource;
	list<wstring> m_ListTexturePrototypeTag;
	list<CUI*> m_ListUi;
	CManagement* m_pManageMent;
	wstring m_strSelectPrototypeTag;
	CListBox m_CloneList;

	afx_msg void OnLbnSelchangeTextureList();
	afx_msg void OnBnClickedLoadButton();
	afx_msg void OnBnClickedCreateUiButton();
	afx_msg void OnLbnSelchangeCloneUIList();
	float m_fPosX;
	float m_fPosY;
	float m_fRotX;
	float m_fRotY;
	float m_fScaleX;
	float m_fScaleY;

	CUI* m_pTargetUi;
	afx_msg void OnBnClickedDeleteButton();
	int m_iUiCount = 0;
};

