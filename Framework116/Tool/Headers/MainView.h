#pragma once



// CMainView 보기

class CMainView : public CScrollView
{
	DECLARE_DYNCREATE(CMainView)

protected:
	CMainView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMainView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()

public:
	HRESULT Ready_StaticResources();
	HRESULT Setup_DefaultSetting();

private:
	bool				m_bStart = true;
	CManagement*		m_pManagement = nullptr;
	LPDIRECT3DDEVICE9	m_pDevice = nullptr;
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};


