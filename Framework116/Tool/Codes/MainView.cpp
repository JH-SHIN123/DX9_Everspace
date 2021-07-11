// MainView.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "MainView.h"
#include <MainFrm.h>
#include "MainScene.h"

#include "Player.h"
#include "MainCam.h"
#include "Axis.h"
#include"ToolUI.h"

// CMainView
HWND g_hWnd;

IMPLEMENT_DYNCREATE(CMainView, CScrollView)

CMainView::CMainView() :
	m_pManagement(CManagement::Get_Instance())
{
}

CMainView::~CMainView()
{
	//CManagement::Destroy
	Safe_Release(m_pDevice);
	
	if (Safe_Release(m_pManagement))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Management");
	}
}


BEGIN_MESSAGE_MAP(CMainView, CScrollView)
END_MESSAGE_MAP()


// CMainView 그리기
void CMainView::OnInitialUpdate()
{
	if (false == m_bStart) return;

	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: 이 뷰의 전체 크기를 계산합니다.
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
	g_hWnd = m_hWnd;

	CMainFrame* pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
	RECT rcMain{};
	pMain->GetWindowRect(&rcMain);
	SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	RECT rcView{};
	GetClientRect(&rcView);
	int iGapX = (rcMain.right - rcView.right) + 1;
	int iGapY = (rcMain.bottom - rcView.bottom) + 1;
	pMain->SetWindowPos(nullptr, 0, 0, MAINVIEW_WINCX + iGapX, MAINVIEW_WINCY + iGapY, PM_NOREMOVE);

	// Init Device 
	if (FAILED(m_pManagement->Ready_Game(m_hWnd, MAINVIEW_WINCX, MAINVIEW_WINCY, EDisplayMode::Win)))
	{
		PRINT_LOG(L"Error", L"Failed To Ready Game");
		return;
	}

	m_pDevice = m_pManagement->Get_Device();
	Safe_AddRef(m_pDevice);

	if (nullptr == m_pDevice)
	{
		PRINT_LOG(L"Error", L"m_pDevice is nullptr");
		return;
	}

	if (FAILED(Ready_StaticResources()))
	{
		PRINT_LOG(L"Error", L"Failed To Ready Static Resources");
		return;
	}

	if (FAILED(Setup_DefaultSetting()))
	{
		PRINT_LOG(L"Error", L"Failed To Setup Default Setting");
		return;
	}

	if (FAILED(m_pManagement->Setup_CurrentScene((_uint)ESceneType::MainScene, CMainScene::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Setup MainScene Scene");
		return;
	}

	m_bStart = false;
}

HRESULT CMainView::Ready_StaticResources()
{
#pragma region GameObjects
	/* For.GameObject_Player */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Player",
		CPlayer::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Player");
		return E_FAIL;
	}

	/* For.GameObject_MainCam */
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_MainCam",
		CMainCam::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_MainCam");
		return E_FAIL;
	}

	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_Axis",
		CAxis::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Axis");
		return E_FAIL;
	}
	if (FAILED(m_pManagement->Add_GameObject_Prototype(
		EResourceType::Static,
		L"GameObject_UI",
		CToolUI::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_Axis");
		return E_FAIL;
	}
#pragma endregion

#pragma region Components
	/* For.Component_VIBuffer_TriColor */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_TriColor",
		CVIBuffer_TriColor::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_TriColor");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_RectColor */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_RectColor",
		CVIBuffer_RectColor::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_RectColor");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_RectTexture */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture",
		CVIBuffer_RectTexture::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_RectTexture");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_CubeTexture */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_VIBuffer_CubeTexture",
		CVIBuffer_CubeTexture::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_VIBuffer_CubeTexture");
		return E_FAIL;
	}

	/* For.Component_Transform */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Transform",
		CTransform::Create(m_pDevice))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Transform");
		return E_FAIL;
	}

	/* For.Component_Mesh_BigShip */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_BigShip",
		CMesh::Create(m_pDevice, L"../../Resources/Models/ship.X", L"../../Resources/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_BigShip");
		return E_FAIL;
	}

	/* For.Component_Mesh_Axis */
	if (FAILED(m_pManagement->Add_Component_Prototype(
		EResourceType::Static,
		L"Component_Mesh_Axis",
		CMesh::Create(m_pDevice, L"../../Resources/Models/axis.X", L"../../Resources/"))))
	{
		PRINT_LOG(L"Error", L"Failed To Add Component_Mesh_Axis");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CMainView::Setup_DefaultSetting()
{
	// 조명 off
	if (FAILED(m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE)))
	{
		PRINT_LOG(L"Error", L"Failed To Set Lighting false");
		return E_FAIL;
	}

	///*
	//D3DFILL_WIREFRAME: 색을 채우지말고 외곽선만 그려라.
	//D3DFILL_SOLID: 색을 채워넣어라(디폴트)
	//*/
	//if (FAILED(m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME)))
	//{
	//	PRINT_LOG(L"Error", L"Failed To Set wireframe");
	//	return E_FAIL;
	//}

	/*
	D3DCULL_CCW(Counter Clock Wise): 반시계 방향으로 구성한 폴리곤을 추려낸다. (디폴트값)
	D3DCULL_CW(Clock Wise): 시계 방향으로 구성한 폴리곤을 추려낸다.
	D3DCULL_NONE: 후면추려내기 안함.
	*/
	if (FAILED(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW)))
	{
		PRINT_LOG(L"Error", L"Failed To Set D3DRS_CULLMODE");
		return E_FAIL;
	}

	return S_OK;
}

void CMainView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 여기에 그리기 코드를 추가합니다.

	// 툴별로 씬을 나누자.
	if (FAILED(m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE)))
	{
		PRINT_LOG(L"Error", L"Failed To Set Lighting false");
	}
	m_pManagement->Get_Device()->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pManagement->Update_Game();

	Invalidate(FALSE);
}


// CMainView 진단
#ifdef _DEBUG
void CMainView::AssertValid() const
{
	CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainView 메시지 처리기
