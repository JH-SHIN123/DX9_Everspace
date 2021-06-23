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
			// 이제 저장. 
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
			// 이제 저장. 
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
				CCloneListBox.AddString(L"asd");
				
			}
			CloseHandle(hFile);
		}
	}
}

void CMapTool::OnBnClickedAddclone()
{
}

void CMapTool::OnBnClickedDeleteclone()
{
}

void CMapTool::OnBnClickedAddnavi()
{
}

void CMapTool::OnBnClickedDeletenavi()
{
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
