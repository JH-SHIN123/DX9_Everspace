// ../Codes/ObjectTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "pch.h"
#include "Tool.h"
#include "../Headers/ObjectTool.h"
#include "afxdialogex.h"
//#include "MyAddComponent.h"




// CObjectTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CObjectTool, CDialog)

CObjectTool::CObjectTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OBJECTTOOL, pParent)
	, m_wstrPickedObject(_T(""))
	, m_wstrComponentProtoType_Tag(_T(""))
	, r2(_T(""))
{
	ZeroMemory(&m_tMaterial, sizeof(D3DMATERIAL9));
}

CObjectTool::~CObjectTool()
{
	Release_ObjectData();
}

void CObjectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBoxObject);
	DDX_Control(pDX, IDC_LIST2, m_ListAddedCom);
	DDX_Control(pDX, IDC_LIST3, m_ListComponent);
	DDX_Control(pDX, IDC_LIST4, m_ListObject_Save);
	DDX_Text(pDX, IDC_EDIT1, m_wstrObjectPrototype_Tag); // 오브젝트 프로토타입 기입란
	DDX_Text(pDX, IDC_EDIT14, m_wstrComponentProtoType_Tag);

	if (m_bListCheck == false)
	{
		Setting_List_Box();
		//Setting_ObjectData();
		m_bListCheck = true;
	}

	DDX_Text(pDX, IDC_EDIT3, m_tMaterial.Diffuse.r);
	DDX_Text(pDX, IDC_EDIT4, m_tMaterial.Diffuse.g);
	DDX_Text(pDX, IDC_EDIT2, m_tMaterial.Diffuse.b);
	DDX_Text(pDX, IDC_EDIT5, m_tMaterial.Diffuse.a);

	DDX_Text(pDX, IDC_EDIT7, m_tMaterial.Ambient.r);
	DDX_Text(pDX, IDC_EDIT8, m_tMaterial.Ambient.g);
	DDX_Text(pDX, IDC_EDIT6, m_tMaterial.Ambient.b);
	DDX_Text(pDX, IDC_EDIT9, m_tMaterial.Ambient.a);

	DDX_Text(pDX, IDC_EDIT11, m_tMaterial.Specular.r);
	DDX_Text(pDX, IDC_EDIT12, m_tMaterial.Specular.g);
	DDX_Text(pDX, IDC_EDIT10, m_tMaterial.Specular.b);
	DDX_Text(pDX, IDC_EDIT13, m_tMaterial.Specular.a);

	DDX_Text(pDX, IDC_EDIT16, m_tMaterial.Emissive.r);
	DDX_Text(pDX, IDC_EDIT17, m_tMaterial.Emissive.g);
	DDX_Text(pDX, IDC_EDIT15, m_tMaterial.Emissive.b);
	DDX_Text(pDX, IDC_EDIT18, m_tMaterial.Emissive.a);

	DDX_Text(pDX, IDC_EDIT19, m_tMaterial.Power);

}

void CObjectTool::Setting_List_Box()
{

	auto Pair = m_mapObjectData.begin();

	// 오브젝트 목록
	m_ListBoxObject.InsertString(0, L"Player");
	m_ListBoxObject.InsertString(1, L"Monster_Normal");
	m_ListBoxObject.InsertString(2, L"Monster_Sniper");
	m_ListBoxObject.InsertString(3, L"Monster_Stealth");
	m_ListBoxObject.InsertString(4, L"Monster_Buff");
	m_ListBoxObject.InsertString(5, L"Monster_Boss");

	m_ListBoxObject.InsertString(6, L"DUMMY"); // 더미 데이터



	// 추가 가능한 컴포넌트 목록
	m_ListComponent.InsertString(0, L"VIBuffer_TriColor");
	m_ListComponent.InsertString(1, L"VIBuffer_RectColor");
	m_ListComponent.InsertString(2, L"VIBuffer_RectTexture");
	m_ListComponent.InsertString(3, L"VIBuffer_TerrainColor");
	m_ListComponent.InsertString(4, L"VIBuffer_TerrainTexture");
	m_ListComponent.InsertString(5, L"VIBuffer_CubeTexture");
	m_ListComponent.InsertString(6, L"Transform");
	m_ListComponent.InsertString(7, L"Texture");
	m_ListComponent.InsertString(8, L"Geometry_Polygon");

}

void CObjectTool::Setting_ObjectData()
{

	/*
	안씀 안씀 안씀 안씀 안씀 안씀 안씀 안씀 안씀 안씀 안씀 안씀 안씀 안씀 안씀 
	*/

	auto Pair = m_mapObjectData.find(L"Player");
	if (Pair == m_mapObjectData.end())
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));

		m_mapObjectData.insert(make_pair(L"Player", pData));
	}

	Pair = m_mapObjectData.find(L"Monster_Normal");
	if (Pair == m_mapObjectData.end())
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));

		m_mapObjectData.insert(make_pair(L"Monster_Normal", pData));
	}

	Pair = m_mapObjectData.find(L"Monster_Sniper");
	if (Pair == m_mapObjectData.end())
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));

		m_mapObjectData.insert(make_pair(L"Monster_Sniper", pData));
	}

	Pair = m_mapObjectData.find(L"Monster_Stealth");
	if (Pair == m_mapObjectData.end())
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));

		m_mapObjectData.insert(make_pair(L"Monster_Stealth", pData));
	}

	Pair = m_mapObjectData.find(L"Monster_Buff");
	if (Pair == m_mapObjectData.end())
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));

		m_mapObjectData.insert(make_pair(L"Monster_Buff", pData));
	}

	Pair = m_mapObjectData.find(L"Monster_Boss");
	if (Pair == m_mapObjectData.end())
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));

		m_mapObjectData.insert(make_pair(L"Monster_Boss", pData));
	}

	Pair = m_mapObjectData.find(L"DUMMY");
	if (Pair == m_mapObjectData.end())
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));

		m_mapObjectData.insert(make_pair(L"DUMMY", pData));
	}

}

void CObjectTool::Release_ObjectData()
{
	for (auto& Pair : m_mapObjectData)
	{
		delete Pair.second;
		Pair.second = nullptr;
	}

	m_mapObjectData.clear();
}


BEGIN_MESSAGE_MAP(CObjectTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CObjectTool::OnLbnSelchangeList1)	// Object Prototype List Picked
	ON_LBN_SELCHANGE(IDC_LIST2, &CObjectTool::OnLbnSelchangeList2)	// Added Components List (Save Data)
	ON_LBN_SELCHANGE(IDC_LIST3, &CObjectTool::OnLbnSelchangeList3)	// Component List
	ON_LBN_SELCHANGE(IDC_LIST4, &CObjectTool::OnLbnSelchangeList4)	// Save Object List (Save Data)
	ON_BN_CLICKED(IDC_BUTTON1, &CObjectTool::OnBnClickedButton1)	// Data 수정용
	ON_BN_CLICKED(IDC_BUTTON2, &CObjectTool::OnBnClickedButton2)	// Add Component
	ON_BN_CLICKED(IDC_BUTTON3, &CObjectTool::OnBnClickedButton3)	// Delete This Component
	ON_BN_CLICKED(IDC_BUTTON5, &CObjectTool::OnBnClickedButton5)	// Save
	ON_BN_CLICKED(IDC_BUTTON6, &CObjectTool::OnBnClickedButton6)	// Load
	ON_BN_CLICKED(IDC_BUTTON8, &CObjectTool::OnBnClickedButton8)	// Add Object List
	ON_BN_CLICKED(IDC_BUTTON9, &CObjectTool::OnBnClickedButton9)	// Clear Object List
	ON_BN_CLICKED(IDC_BUTTON10, &CObjectTool::OnBnClickedButton10)	// Delete This Object List
END_MESSAGE_MAP()


// CObjectTool 메시지 처리기입니다.
void CObjectTool::OnBnClickedButton2() // Add Component Button
{
	UpdateData(TRUE);

	if (m_wstrComponentProtoType_Tag == L"" ||
		m_wstrPickedObjectList_Tag_Save == L"")
		return;

	m_ListAddedCom.ResetContent();

	auto Pair = m_mapObjectData.find(m_wstrPickedObjectList_Tag_Save);

	if (Pair != m_mapObjectData.end())
	{
		// 이미 입력됨
		// Pair->second->wstrPrototypeTag = m_wstrObjectPrototype_Tag;
		// Pair->second->tMaterial = m_tMaterial;

		//Pair->second->wstrPrototypeTag_Mesh = m_wstrComponentProtoType_Tag;

		auto iter = Pair->second->vecPrototypeTag_Mesh.begin();
		// vector가 텅텅 빌때 까지 for문
		for (; iter != Pair->second->vecPrototypeTag_Mesh.end(); ++iter)
		{
			m_ListAddedCom.AddString(iter->GetString());
		}

		Pair->second->vecPrototypeTag_Mesh.emplace_back(m_wstrComponentProtoType_Tag);
		m_ListAddedCom.AddString(m_wstrComponentProtoType_Tag);
		m_tMaterial = Pair->second->tMaterial;

	}


	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton3() // Delete This Component
{

	// 1. Save Object List에서 마지막으로 클릭한 인덱스로 key값 순회
	// 2. 찾은 key값을 바탕으로 Added Component List의 인덱스로 value의 vetor 순회
	// 3. 해당 value 삭제
	// 4. Added Component List 갱신

	_uint iObjectList_Index = m_ListObject_Save.GetCurSel();
	CString wstrObject_Tag = L"";
	m_ListObject_Save.GetText(iObjectList_Index, wstrObject_Tag);

	auto Pair = m_mapObjectData.find(wstrObject_Tag);
	if (Pair != m_mapObjectData.end())
	{
		_uint iComponentList_Index = m_ListAddedCom.GetCurSel();

		Pair->second->vecPrototypeTag_Mesh.erase(
			Pair->second->vecPrototypeTag_Mesh.begin() + iComponentList_Index);

		m_ListAddedCom.ResetContent();


		auto iter = Pair->second->vecPrototypeTag_Mesh.begin();
		// vector가 텅텅 빌때 까지 for문
		for (; iter != Pair->second->vecPrototypeTag_Mesh.end(); ++iter)
		{
			m_ListAddedCom.AddString(iter->GetString()); 
		}
	}
}

void CObjectTool::OnLbnSelchangeList1() // Object ListBox 
{
	UpdateData(TRUE);

	CString wstrTag = L"";
	_int iIndex = m_ListBoxObject.GetCurSel();

	m_ListBoxObject.GetText(iIndex, wstrTag);

	m_wstrPickedObject = wstrTag;
	m_wstrObjectPrototype_Tag = L"GameObject_" + m_wstrPickedObject;
	ZeroMemory(&m_tMaterial, sizeof(D3DMATERIAL9));


	//if (m_wstrPickedObject != L"GameObject_" + wstrTag)
	//{
	//	m_ListAddedCom.ResetContent();
	//}
	//m_ListAddedCom.ResetContent();
	//m_wstrPickedObject 
	//auto Pair = m_mapObjectData.find(wstrTag);
	//if (Pair != m_mapObjectData.end())
	//{
	//	if (Pair->second->wstrPrototypeTag_Mesh != L"")
	//	{
	//		m_wstrObjectPrototype_Tag = Pair->second->wstrPrototypeTag;
	//		m_wstrComponentProtoType_Tag = Pair->second->wstrPrototypeTag_Mesh;
	//		m_tMaterial = Pair->second->tMaterial;

	//		m_ListAddedCom.AddString(m_wstrComponentProtoType_Tag);
	//	}
	//	else
	//		ZeroMemory(&m_tMaterial, sizeof(D3DMATERIAL9));
	//}


	UpdateData(FALSE);
}

void CObjectTool::OnLbnSelchangeList2()	// Object's Added Component ListBox 
{
	UpdateData(TRUE);

	//CString wstrTag = L"";
	//m_ListAddedCom.GetText(iIndex, wstrTag);
	_int iIndex = m_ListAddedCom.GetCurSel();

	auto Pair = m_mapObjectData.find(m_wstrPickedObjectList_Tag_Save);
	if (Pair != m_mapObjectData.end())
	{
		//auto Iter = Pair->second->vecPrototypeTag_Mesh;
		m_wstrComponentProtoType_Tag = Pair->second->vecPrototypeTag_Mesh[iIndex];


	}

	UpdateData(FALSE);
}

void CObjectTool::OnLbnSelchangeList3()	// Addable Component ListBox 
{
	UpdateData(TRUE);

	CString wstrTag = L"";
	_int iIndex = m_ListComponent.GetCurSel();

	m_ListComponent.GetText(iIndex, m_wstrPickedComponentTag);
	m_wstrComponentProtoType_Tag = L"Component_" + m_wstrPickedComponentTag;

	UpdateData(FALSE);
}

void CObjectTool::OnLbnSelchangeList4() // Save Object List (Save Data)
{
	UpdateData(TRUE);

	CString wstrTag = L"";
	m_iPickedObjectList_Tag_Index = m_ListObject_Save.GetCurSel();
	m_ListObject_Save.GetText(m_iPickedObjectList_Tag_Index, wstrTag);

	auto Pair = m_mapObjectData.find(wstrTag);

	if (Pair != m_mapObjectData.end()) // 원하는 tag 선택
	{
		m_wstrPickedObjectList_Tag_Save = wstrTag;
		m_wstrObjectPrototype_Tag = Pair->second->wstrPrototypeTag;
		m_tMaterial = Pair->second->tMaterial;
		m_ListAddedCom.ResetContent();

		auto iter = Pair->second->vecPrototypeTag_Mesh.begin();
		// vector가 텅텅 빌때 까지 for문
		for (; iter != Pair->second->vecPrototypeTag_Mesh.end(); ++iter)
		{
			m_ListAddedCom.AddString(iter->GetString());
		}
	}

	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton1() // Object ProtoType Tag / Insert Button
{
	UpdateData(TRUE);

	m_wstrComponentProtoType_Tag;

	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton5() // Save
{
	UpdateData(TRUE);

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
		wofstream fout;

		fout.open(strPath.GetString());

		if (!fout.fail())
		{
			for (auto& Pair : m_mapObjectData)
			{
				D3DMATERIAL9 tMaterial = Pair.second->tMaterial;
				wstring wstrObjectPrototypeTag = Pair.second->wstrPrototypeTag;
				wstring wstrObjectClassTag = Pair.first;

				wstring wstrDiffuse = to_wstring(_float(tMaterial.Diffuse.r)) + L"?" +
					to_wstring(_float(tMaterial.Diffuse.g)) + L"?" +
					to_wstring(_float(tMaterial.Diffuse.b)) + L"?" +
					to_wstring(_float(tMaterial.Diffuse.a));

				wstring wstrAmbient = to_wstring(_float(tMaterial.Ambient.r)) + L"?" +
					to_wstring(_float(tMaterial.Ambient.g)) + L"?" +
					to_wstring(_float(tMaterial.Ambient.b)) + L"?" +
					to_wstring(_float(tMaterial.Ambient.a));

				wstring wstrSpecular = to_wstring(_float(tMaterial.Specular.r)) + L"?" +
					to_wstring(_float(tMaterial.Specular.g)) + L"?" +
					to_wstring(_float(tMaterial.Specular.b)) + L"?" +
					to_wstring(_float(tMaterial.Specular.a));

				wstring wstrEmissive = to_wstring(_float(tMaterial.Emissive.r)) + L"?" +
					to_wstring(_float(tMaterial.Emissive.g)) + L"?" +
					to_wstring(_float(tMaterial.Emissive.b)) + L"?" +
					to_wstring(_float(tMaterial.Emissive.a));

				wstring wstrPower = to_wstring(_float(tMaterial.Power));

				fout << wstrObjectPrototypeTag << "("
					<< wstrObjectClassTag << ")"
					<< wstrDiffuse << "?"
					<< wstrAmbient << "?"
					<< wstrSpecular << "?"
					<< wstrEmissive << "?"
					<< wstrPower << "?" << endl;

				wstring wstrComponentTags = L"";
				wstring wstrComponentTags_Count = to_wstring(_uint(Pair.second->vecPrototypeTag_Mesh.size()));

				for (auto& Iter : Pair.second->vecPrototypeTag_Mesh)
					wstrComponentTags = wstrComponentTags.c_str() + Iter + L"|";

				fout << wstrComponentTags_Count << L"|"
					<< wstrComponentTags << endl;
			}

			fout.close();
		}
	}
	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton6() // Load
{
	UpdateData(TRUE);

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
			TCHAR szObjectProtoTypeTag[MAX_PATH] = L"";
			TCHAR szObjectClassName[MAX_PATH] = L"";

			TCHAR szMaterial_Diffuse_r[MAX_PATH] = L"";
			TCHAR szMaterial_Diffuse_g[MAX_PATH] = L"";
			TCHAR szMaterial_Diffuse_b[MAX_PATH] = L"";
			TCHAR szMaterial_Diffuse_a[MAX_PATH] = L"";

			TCHAR szMaterial_Ambient_r[MAX_PATH] = L"";
			TCHAR szMaterial_Ambient_g[MAX_PATH] = L"";
			TCHAR szMaterial_Ambient_b[MAX_PATH] = L"";
			TCHAR szMaterial_Ambient_a[MAX_PATH] = L"";

			TCHAR szMaterial_Specular_r[MAX_PATH] = L"";
			TCHAR szMaterial_Specular_g[MAX_PATH] = L"";
			TCHAR szMaterial_Specular_b[MAX_PATH] = L"";
			TCHAR szMaterial_Specular_a[MAX_PATH] = L"";

			TCHAR szMaterial_Emissive_r[MAX_PATH] = L"";
			TCHAR szMaterial_Emissive_g[MAX_PATH] = L"";
			TCHAR szMaterial_Emissive_b[MAX_PATH] = L"";
			TCHAR szMaterial_Emissive_a[MAX_PATH] = L"";

			TCHAR szMaterial_Power[MAX_PATH] = L"";

			TCHAR szComponentTag_Count[MAX_PATH] = L"";
			TCHAR szComponentTag[MAX_PATH] = L"";

			Release_ObjectData();

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

				//for (_int i = 0; i < 4; ++i)
				fin.getline(szMaterial_Diffuse_r, MAX_PATH, L'?');
				fin.getline(szMaterial_Diffuse_g, MAX_PATH, L'?');
				fin.getline(szMaterial_Diffuse_b, MAX_PATH, L'?');
				fin.getline(szMaterial_Diffuse_a, MAX_PATH, L'?');

				fin.getline(szMaterial_Ambient_r, MAX_PATH, L'?');
				fin.getline(szMaterial_Ambient_g, MAX_PATH, L'?');
				fin.getline(szMaterial_Ambient_b, MAX_PATH, L'?');
				fin.getline(szMaterial_Ambient_a, MAX_PATH, L'?');

				fin.getline(szMaterial_Specular_r, MAX_PATH, L'?');
				fin.getline(szMaterial_Specular_g, MAX_PATH, L'?');
				fin.getline(szMaterial_Specular_b, MAX_PATH, L'?');
				fin.getline(szMaterial_Specular_a, MAX_PATH, L'?');

				fin.getline(szMaterial_Emissive_r, MAX_PATH, L'?');
				fin.getline(szMaterial_Emissive_g, MAX_PATH, L'?');
				fin.getline(szMaterial_Emissive_b, MAX_PATH, L'?');
				fin.getline(szMaterial_Emissive_a, MAX_PATH, L'?');

				fin.getline(szMaterial_Power, MAX_PATH, L'?');

				// 읽은 값 삽입
				pData->wstrPrototypeTag = szObjectProtoTypeTag;

				pData->tMaterial.Diffuse.r = (_float)_ttof(szMaterial_Diffuse_r);
				pData->tMaterial.Diffuse.g = (_float)_ttof(szMaterial_Diffuse_g);
				pData->tMaterial.Diffuse.b = (_float)_ttof(szMaterial_Diffuse_b);
				pData->tMaterial.Diffuse.a = (_float)_ttof(szMaterial_Diffuse_a);

				pData->tMaterial.Ambient.r = (_float)_ttof(szMaterial_Ambient_r);
				pData->tMaterial.Ambient.g = (_float)_ttof(szMaterial_Ambient_g);
				pData->tMaterial.Ambient.b = (_float)_ttof(szMaterial_Ambient_b);
				pData->tMaterial.Ambient.a = (_float)_ttof(szMaterial_Ambient_a);

				pData->tMaterial.Specular.r = (_float)_ttof(szMaterial_Specular_r);
				pData->tMaterial.Specular.g = (_float)_ttof(szMaterial_Specular_g);
				pData->tMaterial.Specular.b = (_float)_ttof(szMaterial_Specular_b);
				pData->tMaterial.Specular.a = (_float)_ttof(szMaterial_Specular_a);

				pData->tMaterial.Emissive.r = (_float)_ttof(szMaterial_Emissive_r);
				pData->tMaterial.Emissive.g = (_float)_ttof(szMaterial_Emissive_g);
				pData->tMaterial.Emissive.b = (_float)_ttof(szMaterial_Emissive_b);
				pData->tMaterial.Emissive.a = (_float)_ttof(szMaterial_Emissive_a);

				pData->tMaterial.Power = (_float)_ttof(szMaterial_Power);


				// 카운트 만큼 읽어서 삽입
				fin.getline(szComponentTag_Count, MAX_PATH, L'|');
				_uint iComponentTag_Count = _ttoi(szComponentTag_Count);

				for (_uint i = 0; i < iComponentTag_Count; ++i)
				{
					fin.getline(szComponentTag, MAX_PATH, L'|');
					pData->vecPrototypeTag_Mesh.emplace_back(szComponentTag);
				}

				auto Pair = m_mapObjectData.find(szObjectProtoTypeTag); // ClassName으로 탐색

				if (Pair != m_mapObjectData.end())
					break;

				m_mapObjectData.insert(make_pair(szObjectClassName, pData));
				m_ListObject_Save.AddString(szObjectClassName);

			}
			fin.close();
		}
	}

	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton8() // Add Object List
{
	UpdateData(TRUE);

	auto Pair = m_mapObjectData.find(m_wstrPickedObject);

	if (Pair == m_mapObjectData.end()) // 없을때만 추가
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag = m_wstrObjectPrototype_Tag;
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));
		pData->vecPrototypeTag_Mesh.reserve(10);

		m_mapObjectData.insert(make_pair(m_wstrPickedObject, pData));

		Pair = m_mapObjectData.find(m_wstrPickedObject);
		if (Pair != m_mapObjectData.end())
		{
			//Pair->second->vecPrototypeTag_Mesh.emplace_back(L"None");
			Pair->second->wstrPrototypeTag = m_wstrObjectPrototype_Tag.GetString();
			Pair->second->tMaterial = m_tMaterial;
			m_ListObject_Save.AddString(m_wstrPickedObject);
		}
	}

	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton9() // Clear Object List
{
	UpdateData(TRUE);

	Release_ObjectData();

	m_ListObject_Save.ResetContent();

	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton10()	// Delete This Object List
{
	UpdateData(TRUE);

	auto Pair = m_mapObjectData.find(m_wstrPickedObjectList_Tag_Save);

	if (Pair != m_mapObjectData.end())
	{
		delete Pair->second;
		Pair->second = nullptr;
	}
	m_mapObjectData.erase(Pair);

	m_ListObject_Save.ResetContent();

	for (auto Pair2 : m_mapObjectData)
		m_ListObject_Save.AddString(Pair2.first);

	UpdateData(FALSE);
}
