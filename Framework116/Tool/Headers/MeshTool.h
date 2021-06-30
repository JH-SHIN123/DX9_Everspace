#pragma once


// CMeshTool 대화 상자

class CMeshTool : public CDialog
{
	DECLARE_DYNAMIC(CMeshTool)

public:
	CMeshTool(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMeshTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CMeshTool };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_listbox_InstallableMesh;

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoModal();
	afx_msg void OnLbnSelchangeList_SelectContent();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

public:
	static CMeshTool* s_pInstance;

public:
	CEdit m_Edit_PosX;
	CEdit m_Edit_PosY;
	CEdit m_Edit_PosZ;
	CEdit m_Edit_RotateX;
	CEdit m_Edit_RotateY;
	CEdit m_Edit_RotateZ;
	CEdit m_Edit_ScaleX;
	CEdit m_Edit_ScaleY;
	CEdit m_Edit_ScaleZ;
	afx_msg void OnEnChangeEdit_ScaleX();
	afx_msg void OnEnChangeEdit_ScaleY();
	afx_msg void OnEnChangeEdit_ScaleZ();
};
