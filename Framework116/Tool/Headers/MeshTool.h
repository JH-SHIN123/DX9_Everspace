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
	afx_msg void OnLbnSelchangeList_SelectContent();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

public:
	HRESULT Add_Layer_Dummy(const wstring& LayerTag);

public:
	static CMeshTool* s_pInstance;

public:
	// PrototypeTag / MeshName
	unordered_map<wstring, wstring> m_umapGameObjectInfo;

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

public:
	CListBox m_Listbox_InstalledMesh;
	afx_msg void OnBnClickedButton_Install();
	afx_msg void OnBnClickedButton_Delete();
	afx_msg void OnBnClickedButton_Clear();
	afx_msg void OnLbnSelchangeList_SelectContent_InstalledMeshList();

public:
	afx_msg void OnBnClickedButton_InitTransform();
	afx_msg void OnBnClickedButton_Save();
	afx_msg void OnBnClickedButton_Load();
	CEdit m_EditMeshName;
};
