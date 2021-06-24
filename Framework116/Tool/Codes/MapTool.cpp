// MapTool.cpp : 구현 파일입니다.
//
#include "pch.h"
#include "stdafx.h"
#include "Tool.h"
#include "MapTool.h"
#include "afxdialogex.h"
#include "GameObject.h"

// CMapTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MAPTOOL, pParent)
	, m_bPickMode(true) // PickMode == true 면 Object, PickMode == false 면 Navigation
	, m_pManagement(CManagement::Get_Instance())
	, m_fScaleX(1.f)
	, m_fScaleY(1.f)
	, m_fScaleZ(1.f)
	, m_fRotateX(0.f)
	, m_fRotateY(0.f)
	, m_fRotateZ(0.f)
	, m_fPositionX(0.f)
	, m_fPositionY(0.f)
	, m_fPositionZ(0.f)
	, m_strCloneName(_T(""))
	, m_fNaviPosX(0.f)
	, m_fNaviPosY(0.f)
	, m_fNaviPosZ(0.f)
{

}

CMapTool::~CMapTool()
{
}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROTOTYPELIST, CPrototypeListBox);
	DDX_Control(pDX, IDC_CLONELIST3, CCloneListBox);
	DDX_Control(pDX, IDC_NAVILIST, CNavigationListBox);
	DDX_Control(pDX, IDC_STAGELIST, m_CStageListBox);
	DDX_Text(pDX, IDC_SCALEX, m_fScaleX);
	DDX_Text(pDX, IDC_SCALEY, m_fScaleY);
	DDX_Text(pDX, IDC_SCALEZ, m_fScaleZ);
	DDX_Text(pDX, IDC_ROTATEX, m_fRotateX);
	DDX_Text(pDX, IDC_ROTATEY, m_fRotateY);
	DDX_Text(pDX, IDC_ROTATEZ, m_fRotateZ);
	DDX_Text(pDX, IDC_POSITIONX, m_fPositionX);
	DDX_Text(pDX, IDC_POSITIONY, m_fPositionY);
	DDX_Text(pDX, IDC_POSITIONZ, m_fPositionZ);
	DDX_Text(pDX, IDC_EDIT1, m_strCloneName);
	DDX_Text(pDX, IDC_NAVIPOSX, m_fNaviPosX);
	DDX_Text(pDX, IDC_NAVIPOSY, m_fNaviPosY);
	DDX_Text(pDX, IDC_NAVIPOSZ, m_fNaviPosZ);
}

BEGIN_MESSAGE_MAP(CMapTool, CDialog)

	ON_BN_CLICKED(IDC_RADIOOBJECT, &CMapTool::OnBnClickedRadioobject)
	ON_BN_CLICKED(IDC_RADIONAVIGATION, &CMapTool::OnBnClickedRadionavigation)
	ON_BN_CLICKED(IDC_STAGESAVE, &CMapTool::OnBnClickedStagesave)
	ON_BN_CLICKED(IDC_STAGELOAD, &CMapTool::OnBnClickedStageload)
	ON_BN_CLICKED(IDC_ADDCLONE, &CMapTool::OnBnClickedAddclone)
	ON_BN_CLICKED(IDC_DELETECLONE, &CMapTool::OnBnClickedDeleteclone)
	ON_BN_CLICKED(IDC_ADDNAVI, &CMapTool::OnBnClickedAddnavi)
	ON_BN_CLICKED(IDC_DELETENAVI, &CMapTool::OnBnClickedDeletenavi)
	ON_BN_CLICKED(IDC_LOADPROTOTYPE, &CMapTool::OnBnClickedLoadPrototype)
	ON_LBN_SELCHANGE(IDC_CLONELIST3, &CMapTool::OnLbnSelchangeClonelist3)
END_MESSAGE_MAP()

// CMapTool 메시지 처리기입니다.

BOOL CMapTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	a = new PASSDATA_MAP;

	a->eObjectType = 1;
	m_listCloneData.emplace_back(a);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapTool::OnBnClickedRadioobject()
{
	m_bPickMode = true;
}

void CMapTool::OnBnClickedRadionavigation()
{
	m_bPickMode = false;
}

void CMapTool::OnBnClickedStagesave()
{
	if (m_bPickMode)
	{
		CFileDialog Dlg(FALSE,
			L"mapInfo", L"*.mapInfo",
			OFN_OVERWRITEPROMPT, L"Data File(*.mapInfo) | *.mapInfo||");

		TCHAR szBuf[MAX_PATH] = L"";
		GetCurrentDirectory(MAX_PATH, szBuf);
		PathRemoveFileSpec(szBuf);
		lstrcat(szBuf, L"\\Data");
		Dlg.m_ofn.lpstrInitialDir = szBuf;

		if (Dlg.DoModal())
		{
			CString strPath = Dlg.GetPathName();
			HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (INVALID_HANDLE_VALUE == hFile)
				return;
			
			DWORD dwByte = 0;
			DWORD dwstrByte = 0;
			for (auto& Clone : m_listCloneData)
			{
				//dwstrByte = sizeof(TCHAR) * (rPair.first.GetLength() + 1);
				//WriteFile(hFile, &dwstrByte, sizeof(DWORD), &dwByte, nullptr);
				WriteFile(hFile, &Clone->eObjectType, sizeof(_uint), &dwByte, nullptr);
				WriteFile(hFile, &Clone->matWorld, sizeof(_float4x4), &dwByte, nullptr);
			}

			//for (auto& Navi : m_listNaviPos)
			//{
			//	WriteFile(hFile, &Navi->vNodePos, sizeof(_float3), &dwByte, nullptr);
			//}

			CloseHandle(hFile);
		}
	}
	else
	{
		CFileDialog Dlg(FALSE,
			L"navi", L"*.navi",
			OFN_OVERWRITEPROMPT, L"Data File(*.navi) | *.navi||");

		TCHAR szBuf[MAX_PATH] = L"";
		GetCurrentDirectory(MAX_PATH, szBuf);
		PathRemoveFileSpec(szBuf);
		lstrcat(szBuf, L"\\Data");
		Dlg.m_ofn.lpstrInitialDir = szBuf;

		if (Dlg.DoModal())
		{
			CString strPath = Dlg.GetPathName();
			HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (INVALID_HANDLE_VALUE == hFile)
				return;
			
			DWORD dwByte = 0;
			DWORD dwstrByte = 0;

			for (auto& Navi : m_listNaviPos)
			{
				WriteFile(hFile, &Navi->vNodePos, sizeof(_float3), &dwByte, nullptr);
			}

			CloseHandle(hFile);
		}
	}
}

void CMapTool::OnBnClickedStageload()
{
	if (m_bPickMode)
	{
		CFileDialog Dlg(TRUE,
			L"mapInfo", L"*.mapInfo",
			OFN_OVERWRITEPROMPT, L"Data File(*.mapInfo) | *.mapInfo||");

		TCHAR szBuf[MAX_PATH] = L"";
		GetCurrentDirectory(MAX_PATH, szBuf);
		PathRemoveFileSpec(szBuf);
		lstrcat(szBuf, L"\\Data");
		Dlg.m_ofn.lpstrInitialDir = szBuf;

		if (Dlg.DoModal())
		{
			CString strPath = Dlg.GetPathName();
			HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (INVALID_HANDLE_VALUE == hFile)
				return;
			CCloneListBox.ResetContent();
			for (auto& Clone : m_listCloneData)
			{
				Safe_Delete(Clone);
			}
			m_listCloneData.clear();

			DWORD dwByte = 0;
			DWORD dwStrByte = 0;
			PASSDATA_MAP* pMap = nullptr;
			TCHAR* pFilePath = nullptr;

			while (true)
			{
				// 불러올때 순서 어케할지?
				pMap = new PASSDATA_MAP;
				//ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
				//pFilePath = new TCHAR[dwStrByte];
				//ReadFile(hFile, pFilePath, dwStrByte, &dwByte, nullptr);

				ReadFile(hFile, &pMap->eObjectType, sizeof(_uint), &dwByte, nullptr);
				ReadFile(hFile, &pMap->matWorld, sizeof(D3DXMATRIX), &dwByte, nullptr);

				if (0 == dwByte)
				{
					Safe_Delete(pMap);
					break;
				}
				m_listCloneData.emplace_back(pMap);
				CCloneListBox.AddString(L"asd");

				//for (auto& Navi : m_listNaviPos)
				//{
				//	ReadFile(hFile, &Navi->vNodePos, sizeof(_float3), &dwByte, nullptr);
				//}
			}
			CloseHandle(hFile);
		}
	}
	else
	{
		CFileDialog Dlg(TRUE,
			L"navi", L"*.navi",
			OFN_OVERWRITEPROMPT, L"Data File(*.navi) | *.navi||");

		TCHAR szBuf[MAX_PATH] = L"";
		GetCurrentDirectory(MAX_PATH, szBuf);
		PathRemoveFileSpec(szBuf);
		lstrcat(szBuf, L"\\Data");
		Dlg.m_ofn.lpstrInitialDir = szBuf;

		if (Dlg.DoModal())
		{
			CString strPath = Dlg.GetPathName();
			HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (INVALID_HANDLE_VALUE == hFile)
				return;
			CNavigationListBox.ResetContent();
			for (auto& Navi : m_listNaviPos)
			{
				Safe_Delete(Navi);
			}
			m_listNaviPos.clear();

			DWORD dwByte = 0;
			DWORD dwStrByte = 0;
			PASSDATA_ROUTE* pNavi = nullptr;
			TCHAR* pFilePath = nullptr;

			while (true)
			{
				// 불러올때 순서 어케할지?
				pNavi = new PASSDATA_ROUTE;
				//ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
				//pFilePath = new TCHAR[dwStrByte];
				//ReadFile(hFile, pFilePath, dwStrByte, &dwByte, nullptr);

				ReadFile(hFile, &pNavi->vNodePos, sizeof(_float3), &dwByte, nullptr);


				if (0 == dwByte)
				{
					Safe_Delete(pNavi);
					break;
				}
				m_listNaviPos.emplace_back(pNavi);		
			}
			for (auto& Navi : m_listNaviPos)
			{
				int iOrder = CNavigationListBox.GetCount();
				Navi->iNodeOrder = iOrder + 1;

				wstring wstrCombine = to_wstring(Navi->iNodeOrder) + L"번째: " + L"X: " + to_wstring((int)Navi->vNodePos.x) + L" / " + L"Y: " + to_wstring((int)Navi->vNodePos.y) + L" / " + L"Z: " + to_wstring((int)Navi->vNodePos.z);

				CNavigationListBox.AddString(wstrCombine.c_str());
			}
			CloseHandle(hFile);
		}
	}
}

void CMapTool::OnBnClickedAddclone()
{
	UpdateData(TRUE);
	PASSDATA_MAP* pClone = new PASSDATA_MAP;
	D3DXMATRIX matWorld, matScale, matRotX, matRotY, matRotZ, matTrans;

	D3DXMatrixScaling(&matScale, m_fScaleX, m_fScaleY, m_fScaleZ);
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(m_fRotateX));
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(m_fRotateY));
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(m_fRotateZ));
	D3DXMatrixTranslation(&matTrans, m_fPositionX, m_fPositionY, m_fPositionZ);

	matWorld = matScale * matRotX * matRotY * matRotZ * matTrans;

	pClone->eObjectType = 1;
	pClone->matWorld = matWorld;

	m_pManagement->Get_Device()->SetTransform(D3DTS_WORLD, &pClone->matWorld);

	
	//int iOrder = CNavigationListBox.GetCount();
	//pClone->iNodeOrder = iOrder + 1; // 보류.
	// 구조체안에 wstring 하나 넣어서 구별하도록 할까?
	pClone->스트링 = m_strCloneName;

	//m_pManagement->Add_GameObject_InLayer_Tool(EResourceType::Static, //프로토타입태그/레이어태그/리스트박스인덱스/아규먼트);

	m_listCloneData.emplace_back(pClone);
	CCloneListBox.AddString(m_strCloneName);

	m_fScaleX = 1.f;	m_fScaleY = 1.f;	m_fScaleZ = 1.f;
	m_fRotateX = 0.f;	m_fRotateY = 0.f;	m_fRotateZ = 0.f;
	m_fPositionX = 0.f;	m_fPositionY = 0.f;	m_fPositionZ = 0.f;
	UpdateData(FALSE);
	
	
}

void CMapTool::OnBnClickedDeleteclone()
{
	UpdateData(TRUE);
	int iSelect = CCloneListBox.GetCurSel();
	

	// map이 아니라서 String 으로 list안에있는걸 찾아서 지우는게 안대넹. 저장할때 구분할 수 있는 무언가를 또 저장합시다.
	
	CCloneListBox.DeleteString(iSelect);
	UpdateData(FALSE);
}

void CMapTool::OnBnClickedAddnavi()
{
	UpdateData(TRUE);
	PASSDATA_ROUTE* NaviPos = new PASSDATA_ROUTE;
	NaviPos->vNodePos.x = m_fNaviPosX;
	NaviPos->vNodePos.y = m_fNaviPosY;
	NaviPos->vNodePos.z = m_fNaviPosZ;
	//_itow_s(NaviPos->vNodePos.x, szPosX, 10); // 10진수를 사용하겠다.
	int iOrder = CNavigationListBox.GetCount();
	NaviPos->iNodeOrder = iOrder + 1;

	wstring wstrCombine = to_wstring(NaviPos->iNodeOrder) + L"번째: " + L"X: " + to_wstring((int)NaviPos->vNodePos.x) + L" / " +  L"Y: " + to_wstring((int)NaviPos->vNodePos.y) + L" / " + L"Z: " + to_wstring((int)NaviPos->vNodePos.z);
	m_listNaviPos.emplace_back(NaviPos);
	CNavigationListBox.AddString(wstrCombine.c_str());
	/*Safe_Delete(NaviPos);*/
	m_fNaviPosX = 0.f;
	m_fNaviPosY = 0.f;
	m_fNaviPosZ = 0.f;
	UpdateData(FALSE);
}

void CMapTool::OnBnClickedDeletenavi()
{
	UpdateData(TRUE);
	int iIndex = CNavigationListBox.GetCurSel();
	auto& iter = m_listNaviPos.begin();
	for (; iter != m_listNaviPos.end();)
	{
		if ((*iter)->iNodeOrder - 1 == iIndex)
		{
			Safe_Delete(*iter);
			m_listNaviPos.erase(iter);
			CNavigationListBox.DeleteString(iIndex);
			break;
		}
		else	
			++iter;	
	}

	// 문제점 : 마지막으로 찍은 애만 Delete 해야 순서가 안꼬임.
	// 해결책 : NodeOrder 다시 정렬할 수 있는 방법이 있는감?? 
	

	//for (auto& Navi : m_listNaviPos)
	//{
	//	if (Navi->iNodeOrder - 1 == iIndex )
	//	{
	//		Safe_Delete(Navi);
	//		CNavigationListBox.DeleteString(iIndex);
	//		return;
	//	}
	//}
	UpdateData(FALSE);
}

void CMapTool::OnBnClickedLoadPrototype()
{
	CFileDialog Dlg(TRUE,
		L"object", L"*.object",
		OFN_OVERWRITEPROMPT, L"Data File(*.object) | *.object||");

	TCHAR szBuf[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Data");
	Dlg.m_ofn.lpstrInitialDir = szBuf;

	if (Dlg.DoModal())
	{
		CString strPath = Dlg.GetPathName();
		HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (INVALID_HANDLE_VALUE == hFile)
			return;
		CPrototypeListBox.ResetContent();
		for (auto& rPair : m_mapPrototype)
			Safe_Delete(rPair.second);
		m_mapPrototype.clear();

		DWORD dwByte = 0;
		DWORD dwStrByte = 0;
		PASSDATA_OBJECT* pObject = nullptr;
		TCHAR* pFilePath = nullptr;

		while (true)
		{
			// 불러올때 순서 어케할지?
			pObject = new PASSDATA_OBJECT;
			ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
			pFilePath = new TCHAR[dwStrByte];
			ReadFile(hFile, pFilePath, dwStrByte, &dwByte, nullptr);
			pObject->wstrPrototypeTag = pFilePath;
			Safe_Delete_Array(pFilePath);
			ReadFile(hFile, &pObject->wstrPrototypeTag, sizeof(wstring), &dwByte, nullptr);
			ReadFile(hFile, &pObject->wstrPrototypeTag_Mesh, sizeof(wstring), &dwByte, nullptr);

			if (0 == dwByte)
			{
				Safe_Delete(pObject);
				break;
			}
			m_mapPrototype.emplace(pObject->wstrPrototypeTag, pObject);

			CPrototypeListBox.AddString(pObject->wstrPrototypeTag);
		}
		CloseHandle(hFile);
	}
}

void CMapTool::OnLbnSelchangeClonelist3()
{
	UpdateData(TRUE);

	int iIndex = CCloneListBox.GetCurSel();
	
	// 구조체에 인덱스도 같이 저장하면 좋을 듯?

	// map에서 strFindName 키값을주고 map안에서 얘랑 똑같은 애들 찾아서 second를 반환해야하는데. list라서 find 함수가 없음.
	// Add_Clone 할때 리스트박스의 인덱스를 같이 저장해서. 툴에서만 쓰는 용도로 하나 만들어준다고 했음.


	UpdateData(FALSE);
}
