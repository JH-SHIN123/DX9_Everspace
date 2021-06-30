// ../Codes/MeshTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "MeshTool.h"
#include "afxdialogex.h"
#include "Player.h"


// CMeshTool 대화 상자
CMeshTool* CMeshTool::s_pInstance = nullptr;

IMPLEMENT_DYNAMIC(CMeshTool, CDialog)

CMeshTool::CMeshTool(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CMeshTool, pParent)
{
}

CMeshTool::~CMeshTool()
{
}

void CMeshTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listbox_InstallableMesh);

	DDX_Control(pDX, IDC_EDIT2, m_Edit_PosX);
	DDX_Control(pDX, IDC_EDIT3, m_Edit_PosY);
	DDX_Control(pDX, IDC_EDIT4, m_Edit_PosZ);
}

void CMeshTool::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (TRUE == bShow)
	{


	}
}

BOOL CMeshTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	//// 설치할수 있는 기하도형 메시 리스트 채우기
	m_listbox_InstallableMesh.ResetContent();

	m_listbox_InstallableMesh.AddString(L"박스");
	m_listbox_InstallableMesh.AddString(L"실린더");
	m_listbox_InstallableMesh.AddString(L"스피어");
	m_listbox_InstallableMesh.AddString(L"토러스");

	s_pInstance = this;

	return TRUE; 
}


INT_PTR CMeshTool::DoModal()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CDialog::DoModal();
}


void CMeshTool::OnLbnSelchangeList_SelectContent()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCurSel = m_listbox_InstallableMesh.GetCurSel();
	CString strContent = L"";
	m_listbox_InstallableMesh.GetText(iCurSel, strContent);

	CPlayer* pPlayer = (CPlayer*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Player");
	if (pPlayer == nullptr) {
		PRINT_LOG(L"Warning", L"Player is nullptr");
		return;
	}

	if (strContent == L"박스")
	{
		pPlayer->ChangeMesh(L"Component_GeoMesh_Cube");
	}
	else if (strContent == L"실린더")
	{
		pPlayer->ChangeMesh(L"Component_GeoMesh_Cylinder");
	}
	else if (strContent == L"스피어")
	{
		pPlayer->ChangeMesh(L"Component_GeoMesh_Sphere");
	}
	else if (strContent == L"토러스")
	{
		pPlayer->ChangeMesh(L"Component_GeoMesh_Torus");
	}
}

void CMeshTool::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}


BEGIN_MESSAGE_MAP(CMeshTool, CDialog)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_LIST1, &CMeshTool::OnLbnSelchangeList_SelectContent)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CMeshTool 메시지 처리기
