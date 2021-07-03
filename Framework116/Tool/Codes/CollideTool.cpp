// ../Codes/CollideTool.cpp: 구현 파일
//

#include "pch.h"
#include "Tool.h"
#include "../Headers/CollideTool.h"
#include "afxdialogex.h"

#include "MainView.h"
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

		// 플레이어 Bounding Sphere
		CPlayer* pPlayer = (CPlayer*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Player");
		if (pPlayer == nullptr) {
			PRINT_LOG(L"Warning", L"CPlayer is nullptr");
			return;
		}

		pPlayer->ChangeMesh(L"Component_GeoMesh_Sphere");
		pPlayer->m_bRenderWire = true;
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

HRESULT CCollideTool::Add_Layer_Dummy(const PASSDATA_COLLIDE& tPassData)
{

	DUMMY_DESC tDummyDesc;
	tDummyDesc.wstrMeshPrototypeTag = tPassData.wstrMeshPrototypeTag;

	for (auto& p : tPassData.vecBoundingSphere) {
		int installedCount = m_Listbox_Collide.GetCount();

		tDummyDesc.tTransformDesc.vPosition = p.vCenter;
		tDummyDesc.tTransformDesc.vScale = { p.fRadius,p.fRadius,p.fRadius };

		// Clone 추가
		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer_Tool(
			EResourceType::Static,
			L"GameObject_Dummy",
			L"Layer_Dummy", installedCount, &tDummyDesc)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Dummy In Layer");
			return E_FAIL;
		}

		// 리스트에 추가
		m_Listbox_Collide.AddString(to_wstring(installedCount).c_str());
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

	// Add ListBox
	m_Listbox_Collide.AddString(objIndex.c_str());
}


void CCollideTool::OnBnClickedButton_Delete()
{
	int iSelect = m_Listbox_Collide.GetCurSel();
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
	m_Listbox_Collide.ResetContent();

	int iIndex = 0;
	for (auto& p : *dummyList)
	{
		if (p->Get_IsDead())
			continue;

		wstring objIndex = to_wstring(iIndex);
		CDummy* pDummy = (CDummy*)(p);
		if (nullptr == pDummy) continue;
		wstring meshTag = pDummy->Get_MeshPrototypeTag();

		objIndex += L"/";
		objIndex += meshTag;

		p->Set_ListBoxIndex(iIndex);
		m_Listbox_Collide.AddString(objIndex.c_str());

		++iIndex;
	}
}


void CCollideTool::OnBnClickedButton_Clear()
{
	const list<class CGameObject*>* dummyList = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Dummy");
	if (nullptr == dummyList) return;

	for (auto& p : *dummyList)
	{
		if (p) p->Set_IsDead(true);
	}

	m_Listbox_Collide.ResetContent();
}


void CCollideTool::OnBnClickedButton_InitTransform()
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


void CCollideTool::OnBnClickedButton_Save()
{
	// 파싱할 데이터 가공
	CString meshPrototypeTag = L"";
	if (m_Listbox_Mesh.GetCurSel() == -1) return;
	m_Listbox_Mesh.GetText(m_Listbox_Mesh.GetCurSel(), meshPrototypeTag);
	if (meshPrototypeTag == L"") return;

	PASSDATA_COLLIDE tPassData;
	tPassData.wstrMeshPrototypeTag = meshPrototypeTag;

	const list<class CGameObject*>* dummyList = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Dummy");
	if (nullptr == dummyList) return;
	tPassData.vecBoundingSphere.reserve(dummyList->size());
	
	BOUNDING_SPHERE tBounds;
	for (auto& p : *dummyList)
	{
		if (nullptr == p) continue;
		CTransform* pTransform = (CTransform*)p->Get_Component(L"Com_Transform");
		if (nullptr == pTransform) continue;

		tBounds.vCenter = pTransform->Get_TransformDesc().vPosition;
		tBounds.fRadius = pTransform->Get_TransformDesc().vScale.x;

		tPassData.vecBoundingSphere.emplace_back(tBounds);
	}


	// 파싱 시작
	g_IsMainViewInvalidate = false;

	CFileDialog Dlg(FALSE,// 반대로 TRUE라면? 불러오기
		L"collide", L"*.collide",
		OFN_OVERWRITEPROMPT, L"Data File(*.collide) | *.collide||");

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
		DWORD dwstrByte = 0;

		// 메시프로토타입 태그
		dwstrByte = sizeof(TCHAR) * (tPassData.wstrMeshPrototypeTag.GetLength() + 1);
		WriteFile(hFile, &dwstrByte, sizeof(DWORD), &dwByte, nullptr);
		WriteFile(hFile, tPassData.wstrMeshPrototypeTag.GetString(), dwstrByte, &dwByte, nullptr);

		// Bounding Sphere 정보
		size_t boundsCnt = tPassData.vecBoundingSphere.size();
		WriteFile(hFile, &boundsCnt, sizeof(size_t), &dwByte, nullptr);
		for (auto& p : tPassData.vecBoundingSphere)
		{
			WriteFile(hFile, &p, sizeof(BOUNDING_SPHERE), &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}

	g_IsMainViewInvalidate = true;
	CMainView::s_pMainView->Invalidate(FALSE);
}


void CCollideTool::OnBnClickedButton_Load()
{
	g_IsMainViewInvalidate = false;

	CFileDialog Dlg(TRUE,// 반대로 TRUE라면? 불러오기
		L"collide", L"*.collide",
		OFN_OVERWRITEPROMPT, L"Data File(*.collide) | *.collide||");

	TCHAR szBuf[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Resources\\Data");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	PASSDATA_COLLIDE tPassData;
	TCHAR* pMeshPrototypeTag = nullptr;

	if (Dlg.DoModal())
	{
		CString strPath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;

		DWORD dwByte = 0;
		DWORD dwStrByte = 0;

		// 메시 프로토타입 태그 받아오기
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		pMeshPrototypeTag = new TCHAR[dwStrByte];
		ReadFile(hFile, pMeshPrototypeTag, dwStrByte, &dwByte, nullptr);
		tPassData.wstrMeshPrototypeTag = pMeshPrototypeTag;

		// 바운딩스피어 정보 로드
		size_t boundsCnt = 0;
		ReadFile(hFile, &boundsCnt, sizeof(size_t), &dwByte, nullptr);
		tPassData.vecBoundingSphere.reserve(boundsCnt);

		BOUNDING_SPHERE tBounds;
		for (size_t i =0; i<boundsCnt; ++i)
		{
			ReadFile(hFile, &tBounds, sizeof(BOUNDING_SPHERE), &dwByte, nullptr);
			tPassData.vecBoundingSphere.emplace_back(tBounds);
		}

		CloseHandle(hFile);
	}

	// Tool Info 적용
	// 기존 데이터 초기화
	m_Listbox_Collide.ResetContent();
	const list<class CGameObject*>* dummyList = CManagement::Get_Instance()->Get_GameObjectList(L"Layer_Dummy");
	if (nullptr != dummyList) {
		for (auto& p : *dummyList)
			p->Set_IsDead(true);
	}

	Add_Layer_Dummy(tPassData);

	// 로드한 콜라이드 정보의 메시정보 적용
	int iFind = m_Listbox_Mesh.FindString(0, tPassData.wstrMeshPrototypeTag);
	m_Listbox_Mesh.SetCurSel(iFind);

	CAxis* pAxis = (CAxis*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Axis");
	if (pAxis == nullptr) {
		PRINT_LOG(L"Warning", L"CAxis is nullptr");
		return;
	}
	pAxis->ChangeMesh(tPassData.wstrMeshPrototypeTag.GetString());

	g_IsMainViewInvalidate = true;
	CMainView::s_pMainView->Invalidate(FALSE);
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
	pPlayerTransform->Set_Scale({ fScaleX,fScaleX,fScaleX });
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
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CCollideTool 메시지 처리기
