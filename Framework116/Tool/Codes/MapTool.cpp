// MapTool.cpp : 구현 파일입니다.
//
#include "pch.h"
#include "stdafx.h"
#include "Tool.h"
#include "MapTool.h"
#include "afxdialogex.h"
#include "GameObject.h"
#include "Player.h"

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
	, m_bStart(false)
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

_uint CMapTool::Movement(_float fDeltaTime)
{
	//if (GetAsyncKeyState('W') & 0x8000)
	//	UpdateData(TRUE);

	//if (GetAsyncKeyState('S') & 0x8000)
	//	UpdateData(TRUE);

	//if (GetAsyncKeyState('D') & 0x8000)
	//	UpdateData(TRUE);

	//if (GetAsyncKeyState('A') & 0x8000)
	//	UpdateData(TRUE);

	//if (GetAsyncKeyState('Q') & 0x8000)
	//	UpdateData(TRUE);

	//if (GetAsyncKeyState('E') & 0x8000)
	//	UpdateData(TRUE);
	//
	return _uint();
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
	ON_BN_CLICKED(IDC_BUTTON1, &CMapTool::OnBnClickedUpdateTrans)
	ON_LBN_SELCHANGE(IDC_PROTOTYPELIST, &CMapTool::OnLbnSelchangePrototypelist)
END_MESSAGE_MAP()

// CMapTool 메시지 처리기입니다.

BOOL CMapTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bStart = true;
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
				WriteFile(hFile, &Clone->wstrCloneName, sizeof(wstring), &dwByte, nullptr);
				WriteFile(hFile, &Clone->Rotate, sizeof(_float3), &dwByte, nullptr);
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
				WriteFile(hFile, &Navi->iNodeOrder, sizeof(_int), &dwByte, nullptr);
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
				ReadFile(hFile, &pMap->wstrCloneName, sizeof(wstring), &dwByte, nullptr);
				ReadFile(hFile, &pMap->Rotate, sizeof(_float3), &dwByte, nullptr);

				if (0 == dwByte)
				{
					Safe_Delete(pMap);
					break;
				}
				m_listCloneData.emplace_back(pMap);
				CCloneListBox.AddString(pMap->wstrCloneName.c_str());

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
				ReadFile(hFile, &pNavi->iNodeOrder, sizeof(_int), &dwByte, nullptr);


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

	//m_pPlayerTransform = (CTransform*)m_pManagement->Get_GameObject(L"Layer_Player")->Get_Component(L"Com_Transform");
	//D3DXMATRIX matPlayerWorld = m_pPlayerTransform->Get_TransformDesc().matWorld;
	m_pPlayerTransform = (CTransform*)m_pManagement->Get_GameObject(L"Layer_Player")->Get_Component(L"Com_Transform");
	D3DXMATRIX matPlayerWorld = m_pPlayerTransform->Get_TransformDesc().matWorld;

	m_fScaleX = m_pPlayerTransform->Get_TransformDesc().vScale.x * 100.f;
	m_fScaleY = m_pPlayerTransform->Get_TransformDesc().vScale.y * 100.f;
	m_fScaleZ = m_pPlayerTransform->Get_TransformDesc().vScale.z * 100.f;

	m_fPositionX = m_pPlayerTransform->Get_TransformDesc().vPosition.x;
	m_fPositionY = m_pPlayerTransform->Get_TransformDesc().vPosition.y;
	m_fPositionZ = m_pPlayerTransform->Get_TransformDesc().vPosition.z;

	m_fRotateX = m_pPlayerTransform->Get_TransformDesc().vRotate.x;
	m_fRotateY = m_pPlayerTransform->Get_TransformDesc().vRotate.y;
	m_fRotateZ = m_pPlayerTransform->Get_TransformDesc().vRotate.z;

	PASSDATA_MAP* pClone = new PASSDATA_MAP;

	pClone->eObjectType = 1;
 	pClone->matWorld = matPlayerWorld;
	pClone->wstrCloneName = m_strCloneName;

	pClone->Rotate.x = m_fRotateX;
	pClone->Rotate.y = m_fRotateY;
	pClone->Rotate.z = m_fRotateZ;
		
	m_listCloneData.emplace_back(pClone);
	CCloneListBox.AddString(pClone->wstrCloneName.c_str());

	int m_iCloneIndex = CCloneListBox.GetCount();

	TRANSFORM_DESC TransformDesc;
	TransformDesc.vPosition = { m_fPositionX, m_fPositionY, m_fPositionZ };
	TransformDesc.vRotate = { m_fRotateX, m_fRotateY, m_fRotateZ };
	TransformDesc.vScale = { m_fScaleX, m_fScaleY, m_fScaleZ };
	TransformDesc.matWorld = m_pPlayerTransform->Get_TransformDesc().matWorld;

	wstring PrototypeTag = m_strPrototypeTag;
	wstring LayerTag = (wstring)L"Layer_" + PrototypeTag;

	if (FAILED(m_pManagement->Add_GameObject_InLayer_Tool(
		EResourceType::Static, 
		PrototypeTag,
		LayerTag,
		m_iCloneIndex, 
		&TransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Picking Dummy");
		return;
	}
	
	//m_fScaleX = 1.f;	m_fScaleY = 1.f;	m_fScaleZ = 1.f;
	//m_fRotateX = 0.f;	m_fRotateY = 0.f;	m_fRotateZ = 0.f;
	//m_fPositionX = 0.f;	m_fPositionY = 0.f;	m_fPositionZ = 0.f;


	UpdateData(FALSE);
	
	
}

void CMapTool::OnBnClickedDeleteclone()
{
	UpdateData(TRUE);

	int iIndex = CCloneListBox.GetCurSel();
	CString strFindName;
	CCloneListBox.GetText(iIndex, strFindName);
	auto& iter = m_listCloneData.begin();
	for (; iter != m_listCloneData.end();)
	{
		if ((*iter)->wstrCloneName.c_str() == strFindName)
		{
			Safe_Delete(*iter);
			m_listCloneData.erase(iter);
			CCloneListBox.DeleteString(iIndex);
			break;
		}
		else
			++iter;
	}
	// map이 아니라서 String 으로 list안에있는걸 찾아서 지우는게 안대넹. 저장할때 구분할 수 있는 무언가를 또 저장합시다.
	
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

	wstring wstrCombine = to_wstring(NaviPos->iNodeOrder) + L"번: " + L"X: " + to_wstring((int)NaviPos->vNodePos.x) + L" / " +  L"Y: " + to_wstring((int)NaviPos->vNodePos.y) + L" / " + L"Z: " + to_wstring((int)NaviPos->vNodePos.z);
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
	//CFileDialog Dlg(TRUE,
	//	L"object", L"*.object",
	//	OFN_OVERWRITEPROMPT, L"Data File(*.object) | *.object||");

	//TCHAR szBuf[MAX_PATH] = L"";
	//GetCurrentDirectory(MAX_PATH, szBuf);
	//PathRemoveFileSpec(szBuf);
	//lstrcat(szBuf, L"\\Data");
	//Dlg.m_ofn.lpstrInitialDir = szBuf;

	//if (Dlg.DoModal())
	//{
	//	CString strPath = Dlg.GetPathName();
	//	HANDLE hFile = CreateFile(strPath.GetString(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	//	if (INVALID_HANDLE_VALUE == hFile)
	//		return;
	//	CPrototypeListBox.ResetContent();
	//	for (auto& rPair : m_mapPrototype)
	//		Safe_Delete(rPair.second);
	//	m_mapPrototype.clear();

	//	DWORD dwByte = 0;
	//	DWORD dwStrByte = 0;
	//	PASSDATA_OBJECT* pObject = nullptr;
	//	TCHAR* pFilePath = nullptr;

	//	while (true)
	//	{
	//		// 불러올때 순서 어케할지?
	//		pObject = new PASSDATA_OBJECT;
	//		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	//		pFilePath = new TCHAR[dwStrByte];
	//		ReadFile(hFile, pFilePath, dwStrByte, &dwByte, nullptr);
	//		pObject->wstrPrototypeTag = pFilePath;
	//		Safe_Delete_Array(pFilePath);
	//		ReadFile(hFile, &pObject->wstrPrototypeTag, sizeof(wstring), &dwByte, nullptr);
	//		ReadFile(hFile, &pObject->wstrPrototypeTag_Mesh, sizeof(wstring), &dwByte, nullptr);

	//		if (0 == dwByte)
	//		{
	//			Safe_Delete(pObject);
	//			break;
	//		}
	//		m_mapPrototype.emplace(pObject->wstrPrototypeTag, pObject);

	//		CPrototypeListBox.AddString(pObject->wstrPrototypeTag);
	//	}
	//	CloseHandle(hFile);
	//}

	////////////////////////////////////////////////////////////////////////////////////////////

	UpdateData(TRUE);

	g_IsMainViewInvalidate = false;

	CFileDialog Dlg(TRUE
		, L".object", L"*.object"
		, OFN_OVERWRITEPROMPT, L"Data File(*.object) | *.object||"
		, 0, 0, 0);

	TCHAR szBuf[MAX_PATH] = L"";
	GetCurrentDirectory(MAX_PATH, szBuf);
	PathRemoveFileSpec(szBuf);
	PathRemoveFileSpec(szBuf);
	lstrcat(szBuf, L"\\Data\\PrototypeData");

	Dlg.m_ofn.lpstrInitialDir = szBuf;

	if (Dlg.DoModal())
	{
		CString strPath = Dlg.GetPathName();
		CString strFileName = Dlg.GetFileName();
		AfxExtractSubString(strFileName, strFileName, 0, '.');

		wifstream fin;
		fin.open(strPath.GetString());

		// 읽는 순서
		// 프로토타입 태그 > 클래스 이름 > 머테리얼 정보들 >
		// 수많은 컴포넌트 태그들

		if (!fin.fail())
		{
			//TCHAR szObjectProtoTypeTag_Start[MAX_PATH]	= L"";
			TCHAR szObjectProtoTypeTag[MAX_PATH] = L"";
			TCHAR szObjectClassName[MAX_PATH] = L"";
			TCHAR szMaterial_Diffuse[MAX_PATH][4] = { L"" };
			TCHAR szMaterial_Ambient[MAX_PATH][4] = { L"" };
			TCHAR szMaterial_Specular[MAX_PATH][4] = { L"" };
			TCHAR szMaterial_Emissive[MAX_PATH][4] = { L"" };
			TCHAR szMaterial_Power[MAX_PATH] = L"";
			TCHAR szComponentTag_Count[MAX_PATH] = L"";
			TCHAR szComponentTag[MAX_PATH] = L"";


			while (true)
			{
				PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
				pData->wstrPrototypeTag = L"";
				ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));
				pData->vecPrototypeTag_Mesh.reserve(10);

				//fin.getline(szObjectProtoTypeTag_Start, MAX_PATH, L'%');// % 건너 뛰고
				fin.getline(szObjectProtoTypeTag, MAX_PATH, L'(');		// PrototypeTag
				fin.getline(szObjectClassName, MAX_PATH, L')');			// ClassName

				if (fin.eof())
				{
					delete pData;
					pData = nullptr;
					break;
				}

				for (_int i = 0; i < 4; ++i)
					fin.getline(szMaterial_Diffuse[i], MAX_PATH, L'?');
				for (_int i = 0; i < 4; ++i)
					fin.getline(szMaterial_Ambient[i], MAX_PATH, L'?');
				for (_int i = 0; i < 4; ++i)
					fin.getline(szMaterial_Specular[i], MAX_PATH, L'?');
				for (_int i = 0; i < 4; ++i)
					fin.getline(szMaterial_Emissive[i], MAX_PATH, L'?');

				fin.getline(szMaterial_Power, MAX_PATH, L'?');

				// 읽은 값 삽입
				pData->wstrPrototypeTag = szObjectProtoTypeTag;

				pData->tMaterial.Diffuse.r = _ttof(szMaterial_Diffuse[0]);
				pData->tMaterial.Diffuse.g = _ttof(szMaterial_Diffuse[1]);
				pData->tMaterial.Diffuse.b = _ttof(szMaterial_Diffuse[2]);
				pData->tMaterial.Diffuse.a = _ttof(szMaterial_Diffuse[3]);

				pData->tMaterial.Ambient.r = _ttof(szMaterial_Ambient[0]);
				pData->tMaterial.Ambient.g = _ttof(szMaterial_Ambient[1]);
				pData->tMaterial.Ambient.b = _ttof(szMaterial_Ambient[2]);
				pData->tMaterial.Ambient.a = _ttof(szMaterial_Ambient[3]);

				pData->tMaterial.Specular.r = _ttof(szMaterial_Specular[0]);
				pData->tMaterial.Specular.g = _ttof(szMaterial_Specular[1]);
				pData->tMaterial.Specular.b = _ttof(szMaterial_Specular[2]);
				pData->tMaterial.Specular.a = _ttof(szMaterial_Specular[3]);

				pData->tMaterial.Emissive.r = _ttof(szMaterial_Emissive[0]);
				pData->tMaterial.Emissive.g = _ttof(szMaterial_Emissive[1]);
				pData->tMaterial.Emissive.b = _ttof(szMaterial_Emissive[2]);
				pData->tMaterial.Emissive.a = _ttof(szMaterial_Emissive[3]);

				pData->tMaterial.Power = _ttof(szMaterial_Power);


				// 카운트 만큼 읽어서 삽입
				fin.getline(szComponentTag_Count, MAX_PATH, L'|');
				_uint iComponentTag_Count = _ttoi(szComponentTag_Count);

				for (_uint i = 0; i < iComponentTag_Count; ++i)
				{
					fin.getline(szComponentTag, MAX_PATH, L'|');
					pData->vecPrototypeTag_Mesh.emplace_back(szComponentTag);
				}

				auto Pair = m_mapPrototype.find(szObjectProtoTypeTag); // ClassName으로 탐색

				if (Pair != m_mapPrototype.end())
					break;

				m_mapPrototype.insert(make_pair(szObjectClassName, pData));
				CPrototypeListBox.AddString(szObjectClassName);

			}
			fin.close();
		}
	}

	g_IsMainViewInvalidate = true;
	UpdateData(FALSE);




	/////////////////////////////////////////////////////////////////////////////////////////////
}

void CMapTool::OnLbnSelchangeClonelist3()
{
	UpdateData(TRUE);

	int iSelect = CCloneListBox.GetCurSel();
	CString strFindName = L"";
	CCloneListBox.GetText(iSelect, strFindName);

  	for (auto& Clone : m_listCloneData)
	{
		if (Clone->wstrCloneName.c_str() == strFindName)
		{
			m_strCloneName = Clone->wstrCloneName.c_str();

			m_fPositionX = Clone->matWorld._41;
			m_fPositionY = Clone->matWorld._42;
			m_fPositionZ = Clone->matWorld._43;

			m_fScaleX = Clone->matWorld._11;
			m_fScaleY = Clone->matWorld._22;
			m_fScaleZ = Clone->matWorld._33;

			m_fRotateX = Clone->Rotate.x;
			m_fRotateY = Clone->Rotate.y;
			m_fRotateZ = Clone->Rotate.z;
			break;
		}
	}
	UpdateData(FALSE);
}

void CMapTool::OnBnClickedUpdateTrans()
{
	UpdateData(TRUE);

	//m_pPlayerTransform = (CTransform*)m_pManagement->Get_GameObject(L"Layer_Player")->Get_Component(L"Com_Transform");
	//D3DXMATRIX matPlayerWorld = m_pPlayerTransform->Get_TransformDesc().matWorld;
	m_pPlayerTransform = (CTransform*)m_pManagement->Get_GameObject(L"Layer_Player")->Get_Component(L"Com_Transform");
	D3DXMATRIX matPlayerWorld = m_pPlayerTransform->Get_TransformDesc().matWorld;

	m_fScaleX = m_pPlayerTransform->Get_TransformDesc().vScale.x * 100.f;
	m_fScaleY = m_pPlayerTransform->Get_TransformDesc().vScale.y * 100.f;
	m_fScaleZ = m_pPlayerTransform->Get_TransformDesc().vScale.z * 100.f;

	m_fPositionX = m_pPlayerTransform->Get_TransformDesc().vPosition.x;
	m_fPositionY = m_pPlayerTransform->Get_TransformDesc().vPosition.y;
	m_fPositionZ = m_pPlayerTransform->Get_TransformDesc().vPosition.z;

	m_fRotateX = m_pPlayerTransform->Get_TransformDesc().vRotate.x;
	m_fRotateY = m_pPlayerTransform->Get_TransformDesc().vRotate.y;
	m_fRotateZ = m_pPlayerTransform->Get_TransformDesc().vRotate.z;

	UpdateData(FALSE);
}

void CMapTool::OnLbnSelchangePrototypelist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int iIndex = CPrototypeListBox.GetCurSel();
	CPrototypeListBox.GetText(iIndex, m_strPrototypeTag);
	UpdateData(FALSE);
}