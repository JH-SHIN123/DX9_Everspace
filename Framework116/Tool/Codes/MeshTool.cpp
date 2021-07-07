// ../Codes/MeshTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "MeshTool.h"
#include "afxdialogex.h"
#include "Player.h"
#include "Management.h"
#include "Dummy.h"


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
	DDX_Control(pDX, IDC_EDIT5, m_Edit_RotateX);
	DDX_Control(pDX, IDC_EDIT6, m_Edit_RotateY);
	DDX_Control(pDX, IDC_EDIT7, m_Edit_RotateZ);
	DDX_Control(pDX, IDC_EDIT8, m_Edit_ScaleX);
	DDX_Control(pDX, IDC_EDIT9, m_Edit_ScaleY);
	DDX_Control(pDX, IDC_EDIT10, m_Edit_ScaleZ);
	DDX_Control(pDX, IDC_LIST2, m_Listbox_InstalledMesh);
	DDX_Control(pDX, IDC_EDIT1, m_EditMeshName);
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

	m_listbox_InstallableMesh.AddString(L"GameObject_Player");
	m_listbox_InstallableMesh.AddString(L"운석1");
	m_listbox_InstallableMesh.AddString(L"운석2");
	m_listbox_InstallableMesh.AddString(L"링");

	m_umapGameObjectInfo.emplace(L"GameObject_Player",L"Component_Mesh_BigShip");
	m_umapGameObjectInfo.emplace(L"GameObject_Player", L"Component_Mesh_BigShip");

	s_pInstance = this;

	return TRUE; 
}


void CMeshTool::OnLbnSelchangeList_SelectContent_InstalledMeshList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iCurSel = m_Listbox_InstalledMesh.GetCurSel();

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

	if (strContent == L"플레이어")
	{
		pPlayer->ChangeMesh(L"Component_Mesh_BigShip");
	}
	else if (strContent == L"운석1")
	{
		pPlayer->ChangeMesh(L"Component_GeoMesh_Cylinder");
	}
	else if (strContent == L"운석2")
	{
		pPlayer->ChangeMesh(L"Component_GeoMesh_Sphere");
	}
	else if (strContent == L"링")
	{
		pPlayer->ChangeMesh(L"Component_GeoMesh_Ring");
	}

}

void CMeshTool::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
}

HRESULT CMeshTool::Add_Layer_Dummy(const wstring& LayerTag)
{
	int installedCount = m_Listbox_InstalledMesh.GetCount();

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


void CMeshTool::OnBnClickedButton_Install()
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

	wstring objIndex = to_wstring(m_Listbox_InstalledMesh.GetCount());
	wstring meshTag = pPlayer->Get_MeshPrototypeTag();

	if (meshTag == L"Component_GeoMesh_Cube")
	{
		meshTag = L"박스";
	}
	else if (meshTag == L"Component_GeoMesh_Cylinder")
	{
		meshTag = L"실린더";
	}
	else if (meshTag == L"Component_GeoMesh_Sphere")
	{
		meshTag = L"스피어";
	}
	else if (meshTag == L"Component_GeoMesh_Torus")
	{
		meshTag = L"토러스";
	}

	objIndex += L"/";
	objIndex += meshTag;

	// Add ListBox
	m_Listbox_InstalledMesh.AddString(objIndex.c_str());
}


void CMeshTool::OnEnChangeEdit_ScaleX()
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

void CMeshTool::OnEnChangeEdit_ScaleY()
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


void CMeshTool::OnEnChangeEdit_ScaleZ()
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

void CMeshTool::OnBnClickedButton_Delete()
{
	int iSelect = m_Listbox_InstalledMesh.GetCurSel();
	if (iSelect == -1) return;

	const list<class CGameObject*>* dummyList = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Dummy");
	if (nullptr == dummyList) return;

	CGameObject* pDummy = nullptr;
	for (auto& p : *dummyList)
	{
		if (p->Get_ListBoxIndex() == iSelect) {
			p->Set_IsDead(true);
			break;
		}
	}

	// 인덱스 / 리스트박스 리셋
	m_Listbox_InstalledMesh.ResetContent();

	int iIndex = 0;
	for (auto& p : *dummyList)
	{
		if (p->Get_IsDead())
			continue;

		wstring objIndex = to_wstring(iIndex);
		CDummy* pDummy = (CDummy*)(p);
		if (nullptr == pDummy) continue;
		wstring meshTag = pDummy->Get_MeshPrototypeTag();

		if (meshTag == L"Component_GeoMesh_Cube")
		{
			meshTag = L"박스";
		}
		else if (meshTag == L"Component_GeoMesh_Cylinder")
		{
			meshTag = L"실린더";
		}
		else if (meshTag == L"Component_GeoMesh_Sphere")
		{
			meshTag = L"스피어";
		}
		else if (meshTag == L"Component_GeoMesh_Torus")
		{
			meshTag = L"토러스";
		}

		objIndex += L"/";
		objIndex += meshTag;

		p->Set_ListBoxIndex(iIndex);
		m_Listbox_InstalledMesh.AddString(objIndex.c_str());

		++iIndex;
	}
}

void CMeshTool::OnBnClickedButton_Clear()
{
	const list<class CGameObject*>* dummyList = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Dummy");
	if (nullptr == dummyList) return;

	for (auto& p : *dummyList)
	{
		if(p) p->Set_IsDead(true);
	}

	m_Listbox_InstalledMesh.ResetContent();
}

void CMeshTool::OnBnClickedButton_InitTransform()
{
	CTransform* pPlayerTransform = (CTransform*)CManagement::Get_Instance()->Get_Component(L"Layer_Player", L"Com_Transform");
	if (pPlayerTransform == nullptr) {
		PRINT_LOG(L"Warning", L"pPlayerTransform is nullptr");
		return;
	}

	pPlayerTransform->Set_Position({ 0.f,0.f,0.f });
	pPlayerTransform->Set_Rotate({ 0.f,0.f,0.f });
	pPlayerTransform->Set_Scale({ 1.f, 1.f, 1.f });
}


void CMeshTool::OnBnClickedButton_Save()
{
	CString meshName = L"";
	CString meshFullName = L"Component_CustomMesh_";
	m_EditMeshName.GetWindowTextW(meshName);

	if (meshName == L"") {
		return;
	}
	meshFullName += meshName;

	g_IsMainViewInvalidate = false;

	CFileDialog Dlg(FALSE,// 반대로 TRUE라면? 불러오기
		L"mesh", L"*.mesh",
		OFN_OVERWRITEPROMPT, L"Data File(*.mesh) | *.mesh||");

	TCHAR szBuf[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Resources\\Data");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	if (Dlg.DoModal())
	{
		CString strPath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		//WriteFile(hFile, pTile, sizeof(TILE), &dwByte, nullptr);
		
		CloseHandle(hFile);
	}

	g_IsMainViewInvalidate = true;
}


void CMeshTool::OnBnClickedButton_Load()
{
	g_IsMainViewInvalidate = false;

	CFileDialog Dlg(TRUE,// 반대로 TRUE라면? 불러오기
		L"mesh", L"*.mesh",
		OFN_OVERWRITEPROMPT, L"Data File(*.mesh) | *.mesh||");

	TCHAR szBuf[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Resources\\Data");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	if (Dlg.DoModal())
	{
		CString strPath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		while (true)
		{
			//ReadFile(hFile, pTile, sizeof(TILE), &dwByte, nullptr);

			if (0 == dwByte)
			{
				//Safe_Delete(pTile);
				break;
			}
		}

		CloseHandle(hFile);
	}

	g_IsMainViewInvalidate = true;
}


BEGIN_MESSAGE_MAP(CMeshTool, CDialog)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_LIST1, &CMeshTool::OnLbnSelchangeList_SelectContent)
	ON_WM_ACTIVATE()
	ON_EN_CHANGE(IDC_EDIT8, &CMeshTool::OnEnChangeEdit_ScaleX)
	ON_EN_CHANGE(IDC_EDIT9, &CMeshTool::OnEnChangeEdit_ScaleY)
	ON_EN_CHANGE(IDC_EDIT10, &CMeshTool::OnEnChangeEdit_ScaleZ)
	ON_BN_CLICKED(IDC_BUTTON4, &CMeshTool::OnBnClickedButton_Install)
	ON_BN_CLICKED(IDC_BUTTON5, &CMeshTool::OnBnClickedButton_Delete)
	ON_BN_CLICKED(IDC_BUTTON6, &CMeshTool::OnBnClickedButton_Clear)
	ON_LBN_SELCHANGE(IDC_LIST2, &CMeshTool::OnLbnSelchangeList_SelectContent_InstalledMeshList)
	ON_BN_CLICKED(IDC_BUTTON7, &CMeshTool::OnBnClickedButton_InitTransform)
	ON_BN_CLICKED(IDC_BUTTON1, &CMeshTool::OnBnClickedButton_Save)
	ON_BN_CLICKED(IDC_BUTTON3, &CMeshTool::OnBnClickedButton_Load)
END_MESSAGE_MAP()


// CMeshTool 메시지 처리기
