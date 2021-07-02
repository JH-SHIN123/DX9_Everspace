// ../Codes/CollideTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "../Headers/CollideTool.h"
#include "afxdialogex.h"

#include "Player.h"
#include "Management.h"
#include "Dummy.h"
#include "Axis.h"
// CCollideTool 대화 상자

IMPLEMENT_DYNAMIC(CCollideTool, CDialog)

CCollideTool::CCollideTool(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CCollideTool, pParent)
{
}

CCollideTool::~CCollideTool()
{
}

void CCollideTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_Listbox_Mesh);
	DDX_Control(pDX, IDC_LIST3, m_Listbox_Collide);
	DDX_Control(pDX, IDC_EDIT2, m_Edit_PosX);
	DDX_Control(pDX, IDC_EDIT3, m_Edit_PosY);
	DDX_Control(pDX, IDC_EDIT4, m_Edit_PosZ);
	DDX_Control(pDX, IDC_EDIT5, m_Edit_RotateX);
	DDX_Control(pDX, IDC_EDIT6, m_Edit_RotateY);
	DDX_Control(pDX, IDC_EDIT7, m_Edit_RotateZ);
	DDX_Control(pDX, IDC_EDIT8, m_Edit_ScaleX);
	DDX_Control(pDX, IDC_EDIT9, m_Edit_ScaleY);
	DDX_Control(pDX, IDC_EDIT10, m_Edit_ScaleZ);
	DDX_Control(pDX, IDC_TEXT, m_Text_MeshName);
}

void CCollideTool::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// Dummy Clone 다 삭제
	if (TRUE == bShow)
	{
		const list<class CGameObject*>* dummyList = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Dummy");
		if (nullptr != dummyList) {
			for (auto& p : *dummyList)
				p->Set_IsDead(true);
		}

		// 배치되어있는 메시 리스트 추가
		// Component_Mesh_BigShip
		// Component_GeoMesh_Torus
		m_Listbox_Mesh.ResetContent();
		m_Listbox_Mesh.AddString(L"Component_Mesh_BigShip");
		m_Listbox_Mesh.AddString(L"Component_GeoMesh_Torus");
	}
}


BOOL CCollideTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
}

HRESULT CCollideTool::Add_Layer_Dummy(const wstring& LayerTag)
{
	int installedCount = m_Listbox_Collide.GetCount();

	CPlayer* pPlayer = (CPlayer*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Player");
	if (pPlayer == nullptr) {
		PRINT_LOG(L"Warning", L"Player is nullptr");
		return E_FAIL;
	}

	CTransform* pPlayerTransform = (CTransform*)(pPlayer->Get_Component(L"Com_Transform"));
	if (pPlayerTransform == nullptr) {
		PRINT_LOG(L"Warning", L"pPlayerTransform is nullptr");
		return E_FAIL;
	}

	DUMMY_DESC tDummyDesc;
	tDummyDesc.wstrMeshPrototypeTag = pPlayer->Get_MeshPrototypeTag();
	tDummyDesc.tTransformDesc = pPlayerTransform->Get_TransformDesc();

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer_Tool(
		EResourceType::Static,
		L"GameObject_Dummy",
		LayerTag, installedCount, &tDummyDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add Dummy In Layer");
		return E_FAIL;
	}

	return S_OK;
}


void CCollideTool::OnLbnSelchangeList_MeshList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCurSel = m_Listbox_Mesh.GetCurSel();
	CString strContent = L"";
	m_Listbox_Mesh.GetText(iCurSel, strContent);

	CAxis* pAxis = (CAxis*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Axis");
	if (pAxis == nullptr) {
		PRINT_LOG(L"Warning", L"CAxis is nullptr");
		return;
	}

	pAxis->ChangeMesh(strContent.GetString());
}


void CCollideTool::OnLbnSelchangeList_CollideList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCurSel = m_Listbox_Collide.GetCurSel();

	const list<class CGameObject*>* dummyList = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Dummy");
	if (nullptr == dummyList) return;

	for (auto& p : *dummyList)
	{
		p->Set_IsPicking(false);
	}

	for (auto& p : *dummyList)
	{
		if (p->Get_ListBoxIndex() == iCurSel) {
			p->Set_IsPicking(true);
			break;
		}
	}
}


void CCollideTool::OnBnClickedButton_Install()
{
	// Add Layer Dummy
	if (FAILED(Add_Layer_Dummy(L"Layer_Dummy"))) {
		PRINT_LOG(L"Error", L"Failed To Add Dummy In Layer");
		return;
	}

	CPlayer* pPlayer = (CPlayer*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Player");
	if (pPlayer == nullptr) {
		PRINT_LOG(L"Warning", L"Player is nullptr");
		return;
	}

	wstring objIndex = to_wstring(m_Listbox_Collide.GetCount());
	wstring meshTag = pPlayer->Get_MeshPrototypeTag();

	//if (meshTag == L"Component_GeoMesh_Cube")
	//{
	//	meshTag = L"박스";
	//}
	//else if (meshTag == L"Component_GeoMesh_Cylinder")
	//{
	//	meshTag = L"실린더";
	//}
	//else if (meshTag == L"Component_GeoMesh_Sphere")
	//{
	//	meshTag = L"스피어";
	//}
	//else if (meshTag == L"Component_GeoMesh_Torus")
	//{
	//	meshTag = L"토러스";
	//}

	//objIndex += L"/";
	//objIndex += meshTag;

	//// Add ListBox
	//m_Listbox_InstalledMesh.AddString(objIndex.c_str());
}


void CCollideTool::OnBnClickedButton_Delete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CCollideTool::OnBnClickedButton_Clear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CCollideTool::OnBnClickedButton_InitTransform()
{
}


void CCollideTool::OnBnClickedButton_Save()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CCollideTool::OnBnClickedButton_Load()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CCollideTool::OnEnChangeEdit_ScaleX()
{
	CTransform* pPlayerTransform = (CTransform*)CManagement::Get_Instance()->Get_Component(L"Layer_Player", L"Com_Transform");
	if (pPlayerTransform == nullptr) {
		PRINT_LOG(L"Warning", L"pPlayerTransform is nullptr");
		return;
	}

	CString strScaleX;
	m_Edit_ScaleX.GetWindowTextW(strScaleX);

	float fScaleX = _ttof(strScaleX);
	pPlayerTransform->Set_ScaleX(fScaleX);
}


void CCollideTool::OnEnChangeEdit_ScaleY()
{
	CTransform* pPlayerTransform = (CTransform*)CManagement::Get_Instance()->Get_Component(L"Layer_Player", L"Com_Transform");
	if (pPlayerTransform == nullptr) {
		PRINT_LOG(L"Warning", L"pPlayerTransform is nullptr");
		return;
	}

	CString strScaleY;
	m_Edit_ScaleY.GetWindowTextW(strScaleY);

	float fScaleY = _ttof(strScaleY);
	pPlayerTransform->Set_ScaleY(fScaleY);
}


void CCollideTool::OnEnChangeEdit_ScaleZ()
{
	CTransform* pPlayerTransform = (CTransform*)CManagement::Get_Instance()->Get_Component(L"Layer_Player", L"Com_Transform");
	if (pPlayerTransform == nullptr) {
		PRINT_LOG(L"Warning", L"pPlayerTransform is nullptr");
		return;
	}

	CString strScaleZ;
	m_Edit_ScaleZ.GetWindowTextW(strScaleZ);

	float fScaleZ = _ttof(strScaleZ);
	pPlayerTransform->Set_ScaleZ(fScaleZ);
}


BEGIN_MESSAGE_MAP(CCollideTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST2, &CCollideTool::OnLbnSelchangeList_MeshList)
	ON_LBN_SELCHANGE(IDC_LIST3, &CCollideTool::OnLbnSelchangeList_CollideList)
	ON_BN_CLICKED(IDC_BUTTON4, &CCollideTool::OnBnClickedButton_Install)
	ON_BN_CLICKED(IDC_BUTTON5, &CCollideTool::OnBnClickedButton_Delete)
	ON_BN_CLICKED(IDC_BUTTON6, &CCollideTool::OnBnClickedButton_Clear)
	ON_BN_CLICKED(IDC_BUTTON7, &CCollideTool::OnBnClickedButton_InitTransform)
	ON_BN_CLICKED(IDC_BUTTON2, &CCollideTool::OnBnClickedButton_Save)
	ON_BN_CLICKED(IDC_BUTTON1, &CCollideTool::OnBnClickedButton_Load)
	ON_EN_CHANGE(IDC_EDIT8, &CCollideTool::OnEnChangeEdit_ScaleX)
	ON_EN_CHANGE(IDC_EDIT9, &CCollideTool::OnEnChangeEdit_ScaleY)
	ON_EN_CHANGE(IDC_EDIT10, &CCollideTool::OnEnChangeEdit_ScaleZ)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CCollideTool 메시지 처리기
