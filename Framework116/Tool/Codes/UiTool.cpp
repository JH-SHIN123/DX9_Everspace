// ../Codes/UiTool.cpp : 구현 파일입니다.
//

#include"pch.h"
#include "stdafx.h"
#include "Tool.h"
#include "../Headers/UiTool.h"
#include "afxdialogex.h"

// CUiTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUiTool, CDialog)

CUiTool::CUiTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_UITOOL, pParent)
{
}

CUiTool::~CUiTool()
{
}

void CUiTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, CTextureList);
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
	if (0 < CTextureList.GetCurSel())
	{
		static int iCount = 0;
		CUI* pUi = CUI::Create(m_pManageMent->Get_Device());

		TRANSFORM_DESC UiTrans;
		UiTrans.vPosition = { 0,0,0 };
		UiTrans.vScale = { 5.f,5.f,0.f };
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
			, L"Layer_Ui", iCount,&UiDesc)))
		{
			PRINT_LOG(L"Error", L"Add_GameObject_InLayerTool_Failed");
		}
		iCount++;
	}
}


BEGIN_MESSAGE_MAP(CUiTool, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CUiTool::OnLbnSelchangeTextureList)
	ON_BN_CLICKED(IDC_BUTTON1, &CUiTool::OnBnClickedLoadButton)
	ON_BN_CLICKED(IDC_BUTTON3, &CUiTool::OnBnClickedCreateUiButton)
END_MESSAGE_MAP()


// CUiTool 메시지 처리기입니다.
