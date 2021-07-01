// ../Codes/UiTool.cpp : 구현 파일입니다.
//

#include"pch.h"
#include "stdafx.h"
#include "Tool.h"
#include "../Headers/UiTool.h"
#include "afxdialogex.h"
#include"../../Reference/Headers/Transform.h"
#include"../../Reference/Headers/Pipeline.h"
#include"ToolUI.h"

// CUiTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUiTool, CDialog)

CUiTool::CUiTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_UITOOL, pParent)
	, m_fPosX(0)
	, m_fPosY(0)
	, m_fRotX(0)
	, m_fRotY(0)
	, m_fScaleX(0)
	, m_fScaleY(0)
	, m_strFileName(_T(""))
{
}

CUiTool::~CUiTool()
{
}

void CUiTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, CTextureList);
	DDX_Control(pDX, IDC_LIST2, m_CloneList);
	DDX_Text(pDX, IDC_EDIT1, m_fPosX);
	DDX_Text(pDX, IDC_EDIT2, m_fPosY);
	DDX_Text(pDX, IDC_EDIT3, m_fRotX);
	DDX_Text(pDX, IDC_EDIT4, m_fRotY);
	DDX_Text(pDX, IDC_EDIT5, m_fScaleX);
	DDX_Text(pDX, IDC_EDIT6, m_fScaleY);
	DDX_Text(pDX, IDC_EDIT7, m_strFileName);
}

void CUiTool::OnInitialUpdate()
{
}

void CUiTool::OnLbnSelchangeTextureList()
{
	
	int iSelect = CTextureList.GetCurSel();
	auto& iter = m_ListTexturePrototypeTag.begin();
	for (int i = 0; i<iSelect; iter++,i++)
	{
	}
	m_strSelectPrototypeTag = (*iter);
	
}

void CUiTool::OnBnClickedLoadButton()
{
	if (m_pListResource->size())
	{
		CTextureList.ResetContent();
		for (auto& pDst : *m_pListResource)
			{
				for (int i = 0; i < pDst->dwTextureCount; i++)
				{
				wstring strTextureProtoTypeTag = pDst->wstrPrototypeTag;
				strTextureProtoTypeTag += L"%d";
				switch (pDst->dwResourceType)
				{
				case (DWORD)ETextureType::SINGLE:
					strTextureProtoTypeTag += L".png";
					break;
				case (DWORD)ETextureType::MULTI:
					strTextureProtoTypeTag += L".png";
					break;	
				case (DWORD)ETextureType::Cube:
					strTextureProtoTypeTag += L".dds";
						break;
				}
				TCHAR szTag[MAX_PATH] = L"";
				swprintf_s(szTag, strTextureProtoTypeTag.c_str(), i);
				CTextureList.AddString(szTag);
				m_ListTexturePrototypeTag.emplace_back(szTag);
				m_pManageMent = CManagement::Get_Instance();
			}
		}
	}

}

void CUiTool::OnBnClickedCreateUiButton()
{
	if (0 <= CTextureList.GetCurSel())
	{
		
		CToolUI* pUi = CToolUI::Create(m_pManageMent->Get_Device(),this);
		pUi->Set_ListBoxIndex(m_iUiCount);
		TRANSFORM_DESC UiTrans;
		UiTrans.vPosition = { 0,0,0 };
		UiTrans.vScale = { 30.f,30.f,0.f };
		UI_DESC UiDesc;
		UiDesc.wstrTexturePrototypeTag = L"Component_Texture_"+m_strSelectPrototypeTag;
		UiDesc.tTransformDesc = UiTrans;
		wstring strProtoTypeTag = L"GameObject_"+m_strSelectPrototypeTag;
		pUi->Ready_GameObject(&UiDesc);
		PASSDATA_RESOURCE tPassData;
		for (auto& pDst : *m_pListResource)
		{
			for (int i = 0; i < pDst->dwTextureCount; i++)
			{
			wstring wstrPrototypeTag = pDst->wstrPrototypeTag;
			wstrPrototypeTag += L"%d";
			switch (pDst->dwTextureCount)
			{
			case(DWORD)ETextureType::SINGLE:
			case(DWORD)ETextureType::MULTI:
				wstrPrototypeTag += L".png";
				break;	
			case(DWORD)ETextureType::Cube:
				wstrPrototypeTag += L".dds";
					break;
			}
				if (wstrPrototypeTag == m_strSelectPrototypeTag)
				{
					tPassData = *pDst;
				}
			}
		}
		if (FAILED(m_pManageMent->Add_GameObject_Prototype(EResourceType::NonStatic, strProtoTypeTag, pUi)))
		{
			PRINT_LOG(L"Error", L"Add_GameObject_Prototype Ui Failed");
		}

		if (FAILED(m_pManageMent->Add_GameObject_InLayer_Tool(EResourceType::NonStatic, strProtoTypeTag
			, L"Layer_Ui", m_iUiCount,&UiDesc)))
		{
			PRINT_LOG(L"Error", L"Add_GameObject_InLayerTool_Failed");
		}
		m_ListUi.emplace_back(pUi);
		TCHAR szCountBuf[32]; 
		_itow_s(m_iUiCount,szCountBuf,10);
		wstring strCount;
		if (m_iUiCount < 100)
		{
			if (m_iUiCount >= 10)
			{
				strCount = szCountBuf;
				strCount = L"0" + wstring(szCountBuf);
			}

			else
			{
				strCount = szCountBuf;
				strCount = L"00" + wstring(szCountBuf);
			}
		}
		wstring strCloneName = strCount + L" | "+strProtoTypeTag;
		m_CloneList.AddString(strCloneName.c_str());
		m_ListCloneID.emplace_back(strCloneName);
		m_iUiCount++;
	}
}

void CUiTool::OnLbnSelchangeCloneUIList()
{
	UpdateData(TRUE);
	int iSelect = m_CloneList.GetCurSel();
	int iCount = 0;
	m_pManageMent->Get_GameObjectList(L"Layer_Ui");
	auto& iter = m_pManageMent->Get_GameObjectList(L"Layer_Ui")->begin();
	for (; iter != m_pManageMent->Get_GameObjectList(L"Layer_Ui")->end(); iter++)
	{
		if (iCount != (*iter)->Get_ListBoxIndex())
		{
			for (; iCount != (*iter)->Get_ListBoxIndex();)
			{
				iSelect++;
				iCount++;
			}
		}
		if (iSelect ==(*iter)->Get_ListBoxIndex() )
		{
			m_pTargetUi = static_cast<CUI*>(*iter);
			CTransform* pUiTransform = (CTransform*)((*iter)->Get_Component(L"Com_Transform"));
			if (pUiTransform == nullptr)
				return;
			m_fPosX = pUiTransform->Get_TransformDesc().vPosition.x;
			m_fPosY = pUiTransform->Get_TransformDesc().vPosition.y;
			m_fRotX = pUiTransform->Get_TransformDesc().vRotate.x;
			m_fRotY = pUiTransform->Get_TransformDesc().vRotate.y;
			m_fScaleX = pUiTransform->Get_TransformDesc().vScale.x;
			m_fScaleY = pUiTransform->Get_TransformDesc().vScale.y;
		}
		iCount++;
		
	}
	UpdateData(FALSE);
}

void CUiTool::OnBnClickedDeleteButton()
{
	if (m_pTargetUi)
	{
		auto& iter = m_pManageMent->Get_GameObjectList(L"Layer_Ui")->begin();
		for (; iter != m_pManageMent->Get_GameObjectList(L"Layer_Ui")->end(); iter++)
		{
			if ((*iter) == m_pTargetUi)
			{
				static_cast<CToolUI*>(*iter)->Set_Dead();
				m_pTargetUi = nullptr;
				int iSelect = m_CloneList.GetCurSel();
				m_CloneList.DeleteString(iSelect);
			}
		}
	}
}

void CUiTool::OnBnClickedSaveButton()
{
	wofstream fout;
	UpdateData(TRUE);
	wstring strPassFileName = L"../../Data/";
	strPassFileName += m_strFileName;
	strPassFileName += L".txt";
	fout.open(strPassFileName);

	if (!fout.fail())
	{
		auto& iter = m_pManageMent->Get_GameObjectList(L"Layer_Ui")->begin();
		for (; iter != m_pManageMent->Get_GameObjectList(L"Layer_Ui")->end(); iter++)
		{
			wstring strPrototypeTag = static_cast<CToolUI*>(*iter)->Get_TexturePrototypeTag();
			CTransform* CloneTrans = (CTransform*)(*iter)->Get_Component(L"Com_Transform");
			fout << strPrototypeTag.c_str() 
				<< "|" <<
				CloneTrans->Get_TransformDesc().vPosition.x
				<<"|" <<
				CloneTrans->Get_TransformDesc().vPosition.y
				<< "|" <<
				CloneTrans->Get_TransformDesc().vScale.x
				<< "|" <<
				CloneTrans->Get_TransformDesc().vScale.y
				<< endl;
		}
		}
		fout.close();
}

void CUiTool::OnBnClickedLoadFileButton()
{

	wifstream fin;
	UpdateData(TRUE);
	wstring strPassFileName = L"../../Data/";
	strPassFileName += m_strFileName;
	strPassFileName += L".txt";
	m_iUiCount = 0;
	if (m_ListUi.size())
	{
		auto& iter = m_pManageMent->Get_GameObjectList(L"Layer_Ui")->begin();
		for (; iter != m_pManageMent->Get_GameObjectList(L"Layer_Ui")->end(); iter++)
		{
			static_cast<CToolUI*>(*iter)->Set_Dead();
		}
		m_pManageMent->Update_Game();
	}
	m_ListUi.clear();
	m_CloneList.ResetContent();
	fin.open(strPassFileName);

	if (!fin.fail())
	{
		TCHAR szTexturePrototypeTag[MAX_PATH] = L"";
		TCHAR szPosX[MAX_PATH] = L"";
		TCHAR szPosY[MAX_PATH] = L"";
		TCHAR szScaleX[MAX_PATH] = L"";
		TCHAR szScaleY[MAX_PATH] = L"";

		while (true)
		{
			fin.getline(szTexturePrototypeTag, MAX_PATH, L'|');
			fin.getline(szPosX, MAX_PATH, L'|');
			fin.getline(szPosY, MAX_PATH, L'|');
			fin.getline(szScaleX, MAX_PATH,L'|');
			fin.getline(szScaleY, MAX_PATH);

			if (fin.eof())
				break;
			CToolUI* pUi = CToolUI::Create(m_pManageMent->Get_Device(), this);
			pUi->Set_ListBoxIndex(m_iUiCount);
			TRANSFORM_DESC UiTrans;
			_float PosX = _ttof(szPosX);
			_float PosY = _ttof(szPosY);
			_float ScaleX = _ttof(szScaleX);
			_float ScaleY = _ttof(szScaleY);
			UiTrans.vPosition = { PosX,PosY,0 };
			UiTrans.vScale = { ScaleX,ScaleY,0.f };
			UI_DESC UiDesc;
			UiDesc.wstrTexturePrototypeTag =szTexturePrototypeTag;
			wstring strObjectName = szTexturePrototypeTag;
			L"Component_Texture_";
			strObjectName.erase(0, 18);
			UiDesc.tTransformDesc = UiTrans;
			wstring strProtoTypeTag = L"GameObject_" + strObjectName;
			pUi->Ready_GameObject(&UiDesc);

			if (FAILED(m_pManageMent->Add_GameObject_Prototype(EResourceType::NonStatic, szTexturePrototypeTag, pUi)))
			{
				PRINT_LOG(L"Error", L"Add_GameObject_Prototype Ui Failed");
			}

			if (FAILED(m_pManageMent->Add_GameObject_InLayer_Tool(EResourceType::NonStatic, szTexturePrototypeTag
				, L"Layer_Ui", m_iUiCount, &UiDesc)))
			{
				PRINT_LOG(L"Error", L"Add_GameObject_InLayerTool_Failed");
			}
			m_ListUi.emplace_back(pUi);
			TCHAR szCountBuf[32];
			_itow_s(m_iUiCount, szCountBuf, 10);
			wstring strCount;
			if (m_iUiCount < 100)
			{
				if (m_iUiCount >= 10)
				{
					strCount = szCountBuf;
					strCount = L"0" + wstring(szCountBuf);
				}

				else
				{
					strCount = szCountBuf;
					strCount = L"00" + wstring(szCountBuf);
				}
			}
			wstring strCloneName = strCount + L" | " + strProtoTypeTag;
			m_CloneList.AddString(strCloneName.c_str());
			m_iUiCount++;
		}
	}
}




BEGIN_MESSAGE_MAP(CUiTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CUiTool::OnLbnSelchangeTextureList)
	ON_BN_CLICKED(IDC_BUTTON1, &CUiTool::OnBnClickedLoadButton)
	ON_BN_CLICKED(IDC_BUTTON3, &CUiTool::OnBnClickedCreateUiButton)
	ON_LBN_SELCHANGE(IDC_LIST2, &CUiTool::OnLbnSelchangeCloneUIList)
	ON_BN_CLICKED(IDC_BUTTON4, &CUiTool::OnBnClickedDeleteButton)
	ON_BN_CLICKED(IDC_BUTTON2, &CUiTool::OnBnClickedSaveButton)
	ON_BN_CLICKED(IDC_BUTTON6, &CUiTool::OnBnClickedLoadFileButton)
END_MESSAGE_MAP()


// CUiTool 메시지 처리기입니다.
