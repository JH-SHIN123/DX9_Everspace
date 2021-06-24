#pragma once
#include "afxwin.h"

// CMapTool 대화 상자입니다.

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMapTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAPTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public: // 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioobject();
	afx_msg void OnBnClickedRadionavigation();
	afx_msg void OnBnClickedStagesave();
	afx_msg void OnBnClickedStageload();
	afx_msg void OnBnClickedAddclone();
	afx_msg void OnBnClickedDeleteclone();
	afx_msg void OnBnClickedAddnavi();
	afx_msg void OnBnClickedDeletenavi();
	afx_msg void OnBnClickedLoadPrototype();
	afx_msg void OnLbnSelchangeClonelist3();

private: // 변수
	CManagement* m_pManagement = nullptr;

	// PickMode 라디오 버튼
	_bool m_bPickMode;

	// 프로토타입 보관할 map
	map<CString, PASSDATA_OBJECT*> m_mapPrototype;	

	// 클론들 Matrix 보관할 List
	list<PASSDATA_MAP*> m_listCloneData;
	
	// 항로 보관할 List
	list<PASSDATA_ROUTE*> m_listNaviPos; 
	
	//vector<list<PASSDATA_MAP*>> m_vecStage;

	//리스트박스
	CListBox CPrototypeListBox;
	CListBox CCloneListBox;
	CListBox CNavigationListBox;
	CListBox m_CStageListBox;

	//Test
	PASSDATA_MAP* a;

	// Transform / NaviPos 변수들.
public:
	float m_fScaleX;
	float m_fScaleY;
	float m_fScaleZ;
	float m_fRotateX;
	float m_fRotateY;
	float m_fRotateZ;
	float m_fPositionX;
	float m_fPositionY;
	float m_fPositionZ;

	/*CString m_strCloneName;*/
	wstring m_wstrCloneName;

	float m_fNaviPosX;
	float m_fNaviPosY;
	float m_fNaviPosZ;
};
