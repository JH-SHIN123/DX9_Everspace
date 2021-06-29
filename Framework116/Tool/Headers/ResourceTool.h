#pragma once
#include "afxwin.h"


// CResourceTool 대화 상자입니다.

class CResourceTool : public CDialog
{
	
	DECLARE_DYNAMIC(CResourceTool)

public:
	CResourceTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CResourceTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESOURCETOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);

	CListBox CDropFileList;
	CListBox CTextureIndexList;
	PASSDATA_RESOURCE m_tPicturePathInfo={};
	list<PASSDATA_RESOURCE*> m_ListResource;
	CVIBuffer_RectTexture* m_pVIBuffer = nullptr;
	CTexture* m_pTex = nullptr;
	CVIBuffer_CubeTexture* m_pCube = nullptr;
	CManagement* m_pManagement = nullptr;
	LPDIRECT3DDEVICE9 m_pDevice = nullptr;
private:
	void MultiTextureInfo_Extraction(const wstring wstrFilePath, list<PASSDATA_RESOURCE*>& listPathInfo);
	DWORD MultiTextureCount(const wstring wstrFilePath);
	CString ConvertRelativePath(const CString & wstrabsPath);

	void Render_Texture();
	void Render_Cube();
public:
	afx_msg void OnLbnSelchangeDropfilelist();
	CStatic m_Picture;
	afx_msg void OnLbnSelchangeIndexList();
	afx_msg void OnBnClickedSaveButton();
	afx_msg void OnBnClickedLoadButton();
	CString m_strPassFileName;
};
