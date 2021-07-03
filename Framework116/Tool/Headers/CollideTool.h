#pragma once


// CCollideTool 대화 상자

class CCollideTool : public CDialog
{
	DECLARE_DYNAMIC(CCollideTool)

public:
	CCollideTool(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CCollideTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CCollideTool };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_Listbox_Mesh;
	CListBox m_Listbox_Collide;
	CEdit m_Edit_PosX;
	CEdit m_Edit_PosY;
	CEdit m_Edit_PosZ;
	CEdit m_Edit_RotateX;
	CEdit m_Edit_RotateY;
	CEdit m_Edit_RotateZ;
	CEdit m_Edit_ScaleX;
	CStatic m_Text_MeshName;

public:
	afx_msg void OnLbnSelchangeList_MeshList();
	afx_msg void OnLbnSelchangeList_CollideList();

public:
	afx_msg void OnBnClickedButton_Install();
	afx_msg void OnBnClickedButton_Delete();
	afx_msg void OnBnClickedButton_Clear();
	afx_msg void OnBnClickedButton_InitTransform();
	afx_msg void OnBnClickedButton_Save();
	afx_msg void OnBnClickedButton_Load();

public:
	virtual BOOL OnInitDialog();
	HRESULT Add_Layer_Dummy(const wstring& LayerTag);

public:

	afx_msg void OnEnChangeEdit_ScaleX();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
