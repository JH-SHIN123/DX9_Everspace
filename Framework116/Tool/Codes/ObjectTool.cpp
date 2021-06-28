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
	for (auto& Pair : m_mapObjectData)
	{
		delete Pair.second;
		Pair.second = nullptr;
	}

	m_mapObjectData.clear();
}

void CObjectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBoxObject);
	DDX_Control(pDX, IDC_LIST2, m_ListAddedCom);
	DDX_Control(pDX, IDC_LIST3, m_ListComponent);
	DDX_Text(pDX, IDC_EDIT1, m_wstrObjectPrototype_Tag); // 오브젝트 프로토타입 기입란
	DDX_Text(pDX, IDC_EDIT14, m_wstrComponentProtoType_Tag);

	if (m_bListCheck == false)
	{
		Setting_List_Box();
		Setting_ObjectData();
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


	DDX_LBString(pDX, IDC_LIST4, r2);
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
	m_ListComponent.InsertString(8, L"Mesh");

}

void CObjectTool::Setting_ObjectData()
{
	// 툴에서 다루기 위한 key값 세팅
	//PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
	//pData->wstrPrototypeTag = L"";
	//pData->wstrPrototypeTag_Mesh = L"";
	//ZeroMemory(pData->tMaterial, sizeof(D3DXMATERIAL));

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


BEGIN_MESSAGE_MAP(CObjectTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CObjectTool::OnLbnSelchangeList1)
	ON_LBN_SELCHANGE(IDC_LIST2, &CObjectTool::OnLbnSelchangeList2)
	ON_LBN_SELCHANGE(IDC_LIST3, &CObjectTool::OnLbnSelchangeList3)
	ON_BN_CLICKED(IDC_BUTTON1, &CObjectTool::OnBnClickedButton1)	// Insert 
	ON_BN_CLICKED(IDC_BUTTON2, &CObjectTool::OnBnClickedButton2)	// Add Component
	ON_BN_CLICKED(IDC_BUTTON5, &CObjectTool::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CObjectTool::OnBnClickedButton6)
END_MESSAGE_MAP()


// CObjectTool 메시지 처리기입니다.
void CObjectTool::OnBnClickedButton2() // Add Component Button
{
	UpdateData(TRUE);

	if (m_wstrComponentProtoType_Tag == L"" ||
		m_wstrPickedObject == L"")
		return;

	m_ListAddedCom.ResetContent();

	auto Pair = m_mapObjectData.find(m_wstrPickedObject);

	if (Pair != m_mapObjectData.end())
	{
		Pair->second->wstrPrototypeTag = m_wstrObjectPrototype_Tag;
	
		//Pair->second->wstrPrototypeTag_Mesh = m_wstrComponentProtoType_Tag;
		Pair->second->tMaterial = m_tMaterial;

		m_ListAddedCom.AddString(m_wstrComponentProtoType_Tag);
		m_tMaterial = Pair->second->tMaterial;

	}


	UpdateData(FALSE);
}

void CObjectTool::OnLbnSelchangeList1() // Object ListBox 
{
	UpdateData(TRUE);
	m_ListAddedCom.ResetContent();
	
	CString wstrTag = L"";
	_int iIndex = m_ListBoxObject.GetCurSel();

	m_ListBoxObject.GetText(iIndex, wstrTag);

	if (m_wstrPickedObject != L"GameObject_" + wstrTag)
	{
		m_ListAddedCom.ResetContent();

	}
	m_wstrPickedObject = wstrTag;

	m_wstrObjectPrototype_Tag = L"GameObject_" + m_wstrPickedObject;
	//m_wstrPickedObject 


	auto Pair = m_mapObjectData.find(wstrTag);
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

void CObjectTool::OnBnClickedButton1() // Object ProtoType Tag / Insert Button
{
	UpdateData(TRUE);

	
	m_wstrComponentProtoType_Tag;

	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton5() // Save
{
	UpdateData(TRUE);
	auto Pair = m_mapObjectData.find(m_wstrPickedObject);

	if (Pair == m_mapObjectData.end())
		return;

	CString wstrObjectKey = Pair->first;
	wstring wstrObjectTag = Pair->second->wstrPrototypeTag;
	//wstring wstrMeshTag = Pair->second->wstrPrototypeTag_Mesh;
	D3DMATERIAL9 tMaterial = Pair->second->tMaterial;
	
	wstring wstrDiffuse = to_wstring(_float (tMaterial.Diffuse.r)) + L"?" +
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

	CString wstrPath = L"../../Data/PrototypeData/" + wstrObjectKey + L".object";
	CStringA wstrPath_A = (CStringA)wstrPath;
	CHAR szRealPath[MAX_PATH] = "";
	memcpy(szRealPath, wstrPath_A.GetBuffer(), wstrPath_A.GetLength());


	wofstream fout;
	fout.open(szRealPath);

	if (!fout.fail())
	{
		//fout << wstrObjectTag << "?" << wstrMeshTag << "?" << wstrDiffuse << "?" 
		//	<< wstrAmbient << "?" << wstrSpecular << "?" << wstrEmissive << "?" << wstrPower;
	}

	fout.close();


	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButton6()
{
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

		if (!fin.fail())
		{
			TCHAR szObjectProtoTypeTag[MAX_PATH]	= L"";
			TCHAR szMeshProtoTypeTag[MAX_PATH]		= L"";
			TCHAR szMaterial_Diffuse[MAX_PATH][4]	= { L"" };
			TCHAR szMaterial_Ambient[MAX_PATH][4]	= { L"" };
			TCHAR szMaterial_Specular[MAX_PATH][4]	= { L"" };
			TCHAR szMaterial_Emissive[MAX_PATH][4]	= { L"" };
			TCHAR szMaterial_Power[MAX_PATH]		= L"";

			D3DMATERIAL9 tMaterial;

			while (true)
			{
				fin.getline(szObjectProtoTypeTag, MAX_PATH, L'?');
				fin.getline(szMeshProtoTypeTag, MAX_PATH, L'?');

				for (_int i = 0; i < 4; ++i)
					fin.getline(szMaterial_Diffuse[i], MAX_PATH, L'?');
				for (_int i = 0; i < 4; ++i)
					fin.getline(szMaterial_Ambient[i], MAX_PATH, L'?');
				for (_int i = 0; i < 4; ++i)
					fin.getline(szMaterial_Specular[i], MAX_PATH, L'?');
				for (_int i = 0; i < 4; ++i)
					fin.getline(szMaterial_Emissive[i], MAX_PATH, L'?');

				fin.getline(szMaterial_Power, MAX_PATH);

				auto Pair = m_mapObjectData.find(strFileName);
				if (Pair != m_mapObjectData.end())
				{
					tMaterial.Diffuse.r = (_float)_tstof(szMaterial_Diffuse[0]);
					tMaterial.Diffuse.g = (_float)_tstof(szMaterial_Diffuse[1]);
					tMaterial.Diffuse.b = (_float)_tstof(szMaterial_Diffuse[2]);
					tMaterial.Diffuse.a = (_float)_tstof(szMaterial_Diffuse[3]);

					tMaterial.Ambient.r = (_float)_tstof(szMaterial_Ambient[0]);
					tMaterial.Ambient.g = (_float)_tstof(szMaterial_Ambient[1]);
					tMaterial.Ambient.b = (_float)_tstof(szMaterial_Ambient[2]);
					tMaterial.Ambient.a = (_float)_tstof(szMaterial_Ambient[3]);

					tMaterial.Specular.r = (_float)_tstof(szMaterial_Specular[0]);
					tMaterial.Specular.g = (_float)_tstof(szMaterial_Specular[1]);
					tMaterial.Specular.b = (_float)_tstof(szMaterial_Specular[2]);
					tMaterial.Specular.a = (_float)_tstof(szMaterial_Specular[3]);

					tMaterial.Emissive.r = (_float)_tstof(szMaterial_Emissive[0]);
					tMaterial.Emissive.g = (_float)_tstof(szMaterial_Emissive[1]);
					tMaterial.Emissive.b = (_float)_tstof(szMaterial_Emissive[2]);
					tMaterial.Emissive.a = (_float)_tstof(szMaterial_Emissive[3]);

					tMaterial.Power = (_float)_tstof(szMaterial_Power);

					//Pair->first = strPath.GetString;
					Pair->second->wstrPrototypeTag = szObjectProtoTypeTag;
					//Pair->second->wstrPrototypeTag_Mesh = szMeshProtoTypeTag;
					Pair->second->tMaterial = tMaterial;
				}

				if (fin.eof())
					break;
			}
			fin.close();
			UpdateData(FALSE);
		}
	}
}
