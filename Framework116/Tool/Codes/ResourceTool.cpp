// ../Codes/ResourceTool.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "Tool.h"
#include "../Headers/ResourceTool.h"
#include "afxdialogex.h"
#include"GameObject.h"

// CResourceTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CResourceTool, CDialog)

CResourceTool::CResourceTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_RESOURCETOOL, pParent)
	, m_strPassFileName(_T(""))
{
	
}

CResourceTool::~CResourceTool()
{
}

void CResourceTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DROPFILELIST, CDropFileList);
	DDX_Control(pDX, IDC_LIST1, CTextureIndexList);
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Text(pDX, IDC_PASSEDIT, m_strPassFileName);
}

void CResourceTool::OnDropFiles(HDROP hDropInfo)
{
	m_pManagement = CManagement::Get_Instance();
	m_pDevice = CManagement::Get_Instance()->Get_Device();
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	TCHAR szFileName[MAX_PATH] = L""; //뭐 반환한다 했는지 기억하는게 좋을 것이야. 
	int iSize = DragQueryFile(hDropInfo, -1, nullptr, 0);
	for (auto& pPathInfo : m_ListResource)
		Safe_Delete(pPathInfo);
	m_ListResource.clear();
	for (int i = 0; i < iSize; ++i)
	{
		DragQueryFile(hDropInfo, i, szFileName, MAX_PATH);
		// 여기서 그시기 하면 되겠군. 
		MultiTextureInfo_Extraction(szFileName, m_ListResource);
	}
	CDropFileList.ResetContent();
	for (auto& pPathInfo : m_ListResource)
	{
		TCHAR szDrawID[32] = L"";
		_itow_s(pPathInfo->dwTextureCount, szDrawID, 10);
		TCHAR szType[32] = L"";
		switch (pPathInfo->dwResourceType)
		{
		case (DWORD)ETextureType::Cube:
			swprintf_s(szType, L"CUBE");
			break;
		case (DWORD)ETextureType::SINGLE:
			swprintf_s(szType, L"SINGLE");
			break;
		case (DWORD)ETextureType::MULTI:
			swprintf_s(szType, L"MULTI");
			break;
		}
		wstring wstrPathCombine = pPathInfo->wstrFilePath + L"|" + pPathInfo->wstrPrototypeTag + L"|" 
			+ szType + L"|" + szDrawID;
		CDropFileList.AddString(wstrPathCombine.c_str());
	}
	for (auto& pPathInfo : m_ListResource)
	{
		ETextureType eType = ETextureType::SINGLE;
		switch (pPathInfo->dwResourceType)
		{
		case (DWORD)ETextureType::Cube:
			eType = ETextureType::Cube;
			break;
		case (DWORD)ETextureType::SINGLE:
			eType = ETextureType::SINGLE;
			break;
		case (DWORD)ETextureType::MULTI:
			eType = ETextureType::MULTI;
			break;
		}
		TCHAR szType[32] = L"";
		switch (pPathInfo->dwResourceType)
		{
		case (DWORD)ETextureType::Cube:
			swprintf_s(szType, L".dds");
			break;
		case (DWORD)ETextureType::SINGLE:
			swprintf_s(szType, L".png");
			break;
		case (DWORD)ETextureType::MULTI:
			swprintf_s(szType, L".png");
			break;
		}
		wstring wstrTag = L"Component_Texture_";
		wstrTag += pPathInfo->wstrPrototypeTag;
		wstrTag += L"%d";
		wstrTag += szType;
		TCHAR szTagBuf[MAX_PATH] = {};
		TCHAR szPathBuf[MAX_PATH] = {};
		for (int i = 0; i < pPathInfo->dwTextureCount; i++)
		{
			swprintf_s(szTagBuf, wstrTag.c_str(),i);
			swprintf_s(szPathBuf, pPathInfo->wstrFilePath, i);
			if (FAILED(m_pManagement->Add_Component_Prototype(
				EResourceType::NonStatic,
				szTagBuf,CTexture::Create(m_pManagement->Get_Device()
				, eType, szPathBuf,pPathInfo->dwTextureCount
				))))
			{
				wstring Err = L"Failed To Add " + wstrTag;
				PRINT_LOG(L"Error", Err.c_str());
				return;
			}
			
		}
	}
	if (!m_pVIBuffer)
	{
	m_pVIBuffer = CVIBuffer_RectTexture::Create(m_pManagement->Get_Device());
	m_pVIBuffer->Ready_Component();
	}
	if (!m_pCube)
	{
	m_pCube = CVIBuffer_CubeTexture::Create(m_pDevice);
	m_pCube->Ready_Component();
	}

	CDropFileList.SetHorizontalExtent(800);
	UpdateData(FALSE);
	CDialog::OnDropFiles(hDropInfo);
}

void CResourceTool::MultiTextureInfo_Extraction(const wstring wstrFilePath, list<PASSDATA_RESOURCE*>& listPathInfo)
{
	CFileFind Find;
	wstring wstrNextPath = wstrFilePath + L"\\*.*";

	//FindFile -파일의 존재 유무를 확인하는 함수. 만약 다음 파일에 접근하고 싶다면 !? FindNextFile 함수를 사용해야 한다. 
	// 이 함수는 단순 존재만 확인 가능. 
	BOOL bContinue = Find.FindFile(wstrNextPath.c_str());
	while (bContinue)
	{
		// 다음 파일에 접근할 수 있는 함수. 만약 파일의 끝 (즉 없거나 맨 끝이거나)에 도달하면 !? false, 
		bContinue = Find.FindNextFile();

		if (Find.IsDots())
			continue;
		else if (Find.IsDirectory())
			MultiTextureInfo_Extraction(Find.GetFilePath().GetString(), listPathInfo);
		else
		{
			// 내가 진짜 파일을 찾았다 라는 소리가 된다 
			if (Find.IsSystem())
				continue;
			PASSDATA_RESOURCE* pPathInfo = new PASSDATA_RESOURCE;
			TCHAR szFileDirectory[MAX_PATH] = L"";
			//Find. GetFilePath파일의 경로를 반환해주는 함수. 
			//D:\박병건\116A\FrameWork\Texture\Stage\Terrain\Tile\Tile0.png
			lstrcpy(szFileDirectory, Find.GetFilePath().GetString());
			wstring wstrTextureName = szFileDirectory;
			//Tile0
			wstring FileType = PathFindExtension(wstrTextureName.c_str());
			if (L".png" == FileType)
			{
				if(bContinue >=1)
					pPathInfo->dwResourceType = (DWORD)ETextureType::MULTI;
				else 
					pPathInfo->dwResourceType = (DWORD)ETextureType::SINGLE;
			}
			else if (L".dds" == FileType)
			{
				pPathInfo->dwResourceType = (DWORD)ETextureType::Cube;
			}
			else
				continue;
			PathRemoveFileSpec(szFileDirectory);
			//D:\박병건\116A\FrameWork\Texture\Stage\Terrain\Tile
			pPathInfo->dwTextureCount = MultiTextureCount(szFileDirectory);
			// 경로부터. 
			//Tile0
			//D:\박병건\116A\FrameWork\Texture\Stage\Terrain\Tile\Tile0.png
		
			wstrTextureName = wstrTextureName.substr(0, wstrTextureName.length() - 5)+L"%d" +FileType;
			// Tile0 -> Tile%d.png
			
			PathCombine(szFileDirectory, szFileDirectory, wstrTextureName.c_str());
			//D:\박병건\116A\FrameWork\Texture\Stage\Terrain\Tile\Tile%d.png

			pPathInfo->wstrFilePath = ConvertRelativePath(szFileDirectory);
			PathRemoveFileSpec(szFileDirectory);
			pPathInfo->wstrPrototypeTag = PathFindFileName(szFileDirectory);
			
			listPathInfo.emplace_back(pPathInfo);
			
			return;
		}
	}
}

DWORD CResourceTool::MultiTextureCount(const wstring wstrFilePath)
{
		CFileFind Find;
		//D:\박병건\116A\FrameWork\Texture\Stage\Terrain\Tile\*.*
		wstring wstrNextFile = wstrFilePath + L"\\*.*";
		BOOL bContinue = Find.FindFile(wstrNextFile.c_str());
		DWORD iCount = 0;
		while (bContinue)
		{
			bContinue = Find.FindNextFile();
			wstring FileType = PathFindExtension(Find.GetFilePath());
			if (Find.IsSystem() || Find.IsDots())
				continue;
			if (FileType != L".png" && FileType != L".dds")
				continue;
			++iCount;
		}
		return iCount;
}

CString CResourceTool::ConvertRelativePath(const CString & wstrabsPath)
{
		TCHAR szCurrentDir[MAX_PATH] = L"";
		GetCurrentDirectory(MAX_PATH, szCurrentDir);
		TCHAR szRelativePath[MAX_PATH] = L"";
		PathRelativePathTo(szRelativePath, szCurrentDir, FILE_ATTRIBUTE_DIRECTORY, wstrabsPath.GetString(), FILE_ATTRIBUTE_DIRECTORY);

		return CString(szRelativePath);
}
void CResourceTool::Render_Texture()
{
	UpdateData(TRUE);
	int iSelect = CTextureIndexList.GetCurSel();
	CString strFileName = L"";

	DWORD dwIndex = _ttoi(strFileName.GetString());
	TCHAR szType[32] = L"";
	ETextureType eType;
	swprintf_s(szType, L".png");
	eType = ETextureType::SINGLE;
	wstring wstrTag = L"Component_Texture_";
	wstrTag += m_tPicturePathInfo.wstrPrototypeTag;
	wstrTag += L"%d";
	wstrTag += szType;
	TCHAR szTagBuf[MAX_PATH] = {};
	TCHAR szPathBuf[MAX_PATH] = {};

	swprintf_s(szTagBuf, wstrTag.c_str(), iSelect);
	swprintf_s(szPathBuf, m_tPicturePathInfo.wstrFilePath, iSelect);

	wstring strTexTag = m_tPicturePathInfo.wstrPrototypeTag.GetString();
	if (m_pTex)
		Safe_Release(m_pTex);
	m_pTex = CTexture::Create(m_pManagement->Get_Device(), eType,
		szPathBuf);

	_float4x4 matWorld, matScale, matRot, matTrans;
	D3DXMatrixScaling(&matScale, 5.f,5.f,5.f);

	D3DXMatrixRotationX(&matRot, D3DXToRadian(45.f));
	D3DXMatrixTranslation(&matTrans, 1.f, 1.f, 3.f);
	matWorld = matScale*matRot*matTrans;
	m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
	if (FAILED(m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE)))
	{
		PRINT_LOG(L"Error", L"Failed To Set Lighting false");
	}

	m_pDevice->BeginScene();
	m_pDevice->Clear(
		0, nullptr,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 0);
	m_pTex->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	m_pDevice->EndScene();
	m_pDevice->Present(nullptr, nullptr, m_Picture.m_hWnd, nullptr);

	m_Picture.Invalidate();
}
void CResourceTool::Render_Cube()
{
	UpdateData(TRUE);
	int iSelect = CTextureIndexList.GetCurSel();
	CString strFileName = L"";

	DWORD dwIndex = _ttoi(strFileName.GetString());
	TCHAR szType[32] = L"";
	ETextureType eType;
	swprintf_s(szType, L".dds");
	eType = ETextureType::Cube;
	
	wstring wstrTag = L"Component_Texture_";
	wstrTag += m_tPicturePathInfo.wstrPrototypeTag;
	wstrTag += L"%d";
	wstrTag += szType;
	TCHAR szTagBuf[MAX_PATH] = {};
	TCHAR szPathBuf[MAX_PATH] = {};

	swprintf_s(szTagBuf, wstrTag.c_str(), iSelect);
	swprintf_s(szPathBuf, m_tPicturePathInfo.wstrFilePath, iSelect);

	wstring strTexTag = m_tPicturePathInfo.wstrPrototypeTag.GetString();

	
	_float4x4 matWorld, matScale, matRot, matTrans;
	D3DXMatrixScaling(&matScale, 5.f, 5.f,5.f);

	D3DXMatrixRotationX(&matRot, D3DXToRadian(45.f));
	D3DXMatrixTranslation(&matTrans, 1.f, 1.f, 3.f);
	matWorld = matScale*matRot*matTrans;
	m_pDevice->SetTransform(D3DTS_WORLD, &matWorld);
	if (m_pTex)
		Safe_Release(m_pTex);

	m_pTex = CTexture::Create(m_pManagement->Get_Device(), eType,
		szPathBuf);
	if (FAILED(m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE)))
	{
		PRINT_LOG(L"Error", L"Failed To Set Lighting false");
	}

	m_pDevice->BeginScene();
	m_pDevice->Clear(
		0, nullptr,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
		D3DCOLOR_ARGB(255, 0, 0, 255), 1.f, 0);
	m_pTex->Set_Texture(0);
	m_pCube->Render_VIBuffer();
	m_pDevice->EndScene();
	m_pDevice->Present(nullptr, nullptr, m_Picture.m_hWnd, nullptr);

	m_Picture.Invalidate();
}
void CResourceTool::OnLbnSelchangeDropfilelist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	int iSelect = CDropFileList.GetCurSel();

	CTextureIndexList.ResetContent();
	auto&iter = m_ListResource.begin();
	for (_uint i = 0; i < iSelect; i++)
		iter++;
	m_tPicturePathInfo = *(*iter);
	for (int i = 0; i < (*iter)->dwTextureCount; i++)
	{
		TCHAR szIndex[32] = L"";
		_itow_s(i, szIndex, 10);
		CTextureIndexList.AddString(szIndex);
	}
	UpdateData(FALSE);
}
void CResourceTool::OnLbnSelchangeIndexList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_tPicturePathInfo.dwResourceType == (DWORD)ETextureType::SINGLE ||
		m_tPicturePathInfo.dwResourceType == (DWORD)ETextureType::MULTI)
		Render_Texture();
	else
		Render_Cube();
	
}

void CResourceTool::OnBnClickedSaveButton()
{
	wofstream fout;
	UpdateData(TRUE);
	wstring strPassFileName = L"../../Data/";
	strPassFileName += m_strPassFileName;
	strPassFileName += L".txt";
	fout.open(strPassFileName);

	if (!fout.fail())
	{
		for (auto& pPathInfo : m_ListResource)
		{
			TCHAR szType[32] = L"";
			switch (pPathInfo->dwResourceType)
			{
			case (DWORD)ETextureType::Cube:
				swprintf_s(szType, L"CUBE");
				break;
			case (DWORD)ETextureType::SINGLE:
				swprintf_s(szType, L"SINGLE");
				break;
			case (DWORD)ETextureType::MULTI:
				swprintf_s(szType, L"MULTI");
				break;
			}
			fout << pPathInfo->wstrFilePath.GetString() << "|" << pPathInfo->wstrPrototypeTag.GetString()
				<< "|" << szType << "|" << pPathInfo->dwTextureCount << endl;
		}
		fout.close();
	}


}

void CResourceTool::OnBnClickedLoadButton()
{
	if (!m_pManagement)
	{
	m_pManagement = CManagement::Get_Instance();
	m_pManagement->AddRef();
	}
	if (!m_pDevice)
	{
	m_pDevice = CManagement::Get_Instance()->Get_Device();
	m_pDevice->AddRef();
	}
	CDropFileList.ResetContent();
	for (auto& pPathInfo : m_ListResource)
		Safe_Delete(pPathInfo);
	m_ListResource.clear();

	wifstream fin;
	UpdateData(TRUE);
	wstring strPassFileName = L"../../Data/";
	strPassFileName += m_strPassFileName;
	strPassFileName += L".txt";
	fin.open(strPassFileName);

	if (!fin.fail())
	{
		TCHAR szFilePath[MAX_PATH] = L"";
		TCHAR szPrototypeTag[MAX_PATH] = L"";
		TCHAR szType[MAX_PATH] = L"";
		TCHAR szCount[MAX_PATH] = L"";

		while (true)
		{
			fin.getline(szFilePath, MAX_PATH, L'|');
			fin.getline(szPrototypeTag, MAX_PATH, L'|');
			fin.getline(szType, MAX_PATH, L'|');
			fin.getline(szCount, MAX_PATH);

			if (fin.eof())
				break;

			PASSDATA_RESOURCE* pPathInfo = new PASSDATA_RESOURCE;
			pPathInfo->wstrFilePath = szFilePath;
			pPathInfo->wstrPrototypeTag = szPrototypeTag;
			if(!lstrcmp(szType,L"SINGLE"))
				pPathInfo->dwResourceType = (_uint)ETextureType::SINGLE;
			else if (!lstrcmp(szType, L"MULTI"))
				pPathInfo->dwResourceType = (_uint)ETextureType::MULTI;
			else if (!lstrcmp(szType, L"CUBE"))
				pPathInfo->dwResourceType = (_uint)ETextureType::Cube;
			pPathInfo->dwTextureCount = _ttoi(szCount);
			m_ListResource.emplace_back(pPathInfo);
			wstring wstrPathCombine = pPathInfo->wstrFilePath + L"|" + pPathInfo->wstrPrototypeTag
				+ L"|" + szType+ L"|" + szCount;
			CDropFileList.AddString(wstrPathCombine.c_str());
		}
		fin.close();
		if (!m_pVIBuffer)
		{
			m_pVIBuffer = CVIBuffer_RectTexture::Create(m_pManagement->Get_Device());
			m_pVIBuffer->Ready_Component();
		}
		if (!m_pCube)
		{
			m_pCube = CVIBuffer_CubeTexture::Create(m_pDevice);
			m_pCube->Ready_Component();
		}
		for (auto& pPathInfo : m_ListResource)
		{
			ETextureType eType = ETextureType::SINGLE;
			switch (pPathInfo->dwResourceType)
			{
			case (DWORD)ETextureType::Cube:
				eType = ETextureType::Cube;
				break;
			case (DWORD)ETextureType::SINGLE:
				eType = ETextureType::SINGLE;
				break;
			case(DWORD)ETextureType::MULTI:
				eType = ETextureType::MULTI;
				break;
			}
			TCHAR szType[32] = L"";
			switch (pPathInfo->dwResourceType)
			{
			case (DWORD)ETextureType::Cube:
				swprintf_s(szType, L".dds");
				break;
			case (DWORD)ETextureType::SINGLE:
				swprintf_s(szType, L".png");
				break;
			case (DWORD)ETextureType::MULTI:
				swprintf_s(szType, L".png");
				break;
			}
			wstring wstrTag = L"Component_Texture_";
			wstrTag += pPathInfo->wstrPrototypeTag;
			wstrTag += L"%d";
			wstrTag += szType;
			TCHAR szTagBuf[MAX_PATH] = {};
			TCHAR szPathBuf[MAX_PATH] = {};
			for (int i = 0; i < pPathInfo->dwTextureCount; i++)
			{
				swprintf_s(szTagBuf, wstrTag.c_str(), i);
				swprintf_s(szPathBuf, pPathInfo->wstrFilePath, i);
				if (FAILED(m_pManagement->Add_Component_Prototype(
					EResourceType::NonStatic,
					szTagBuf, CTexture::Create(m_pManagement->Get_Device()
						, eType, szPathBuf, pPathInfo->dwTextureCount
					))))
				{
					wstring Err = L"Failed To Add " + wstrTag;
					PRINT_LOG(L"Error", Err.c_str());
					return;
				}

			}
		}
		CDropFileList.SetHorizontalExtent(800);
		UpdateData(FALSE);
	}
}


BEGIN_MESSAGE_MAP(CResourceTool, CDialog)
	ON_WM_DROPFILES()
	ON_LBN_SELCHANGE(IDC_DROPFILELIST, &CResourceTool::OnLbnSelchangeDropfilelist)
	ON_LBN_SELCHANGE(IDC_LIST1, &CResourceTool::OnLbnSelchangeIndexList)
	ON_BN_CLICKED(IDC_BUTTON3, &CResourceTool::OnBnClickedSaveButton)
	ON_BN_CLICKED(IDC_BUTTON1, &CResourceTool::OnBnClickedLoadButton)
END_MESSAGE_MAP()


// CResourceTool 메시지 처리기입니다.
