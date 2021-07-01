#include "stdafx.h"
#include "StreamHandler.h"

#pragma region GameObject
#include "Player.h"
#pragma endregion


HRESULT CStreamHandler::Load_PassData_Object(const wstring & wstrObjectPrototypePath, EResourceType eType /*= EResourceType::NonStatic*/)
{
	wifstream fin;
	fin.open(wstrObjectPrototypePath);

	if (fin.fail())
	{
		wstring wstrErrorLog = wstrObjectPrototypePath + L" is wrong path";
		PRINT_LOG(L"Error", wstrErrorLog.c_str());
		return E_FAIL;
	}

	// 오브젝트 프로토타입
	TCHAR szObjectProtoTypeTag[MAX_PATH] = L"";
	// 구별 할 클래스명
	TCHAR szObjectClassName[MAX_PATH] = L"";
	// 머테리얼 정보
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
	// 컴포넌트의 개수
	TCHAR szComponentTag_Count[MAX_PATH] = L"";
	// 읽을 컴포넌트
	TCHAR szComponentTag[MAX_PATH] = L"";

	while (true)
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag_Object = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));
		pData->vecPrototypeTag_Mesh.reserve(10);

		fin.getline(szObjectProtoTypeTag, MAX_PATH, L'(');		// PrototypeTag
		fin.getline(szObjectClassName, MAX_PATH, L')');			// ClassName

		if (fin.eof())
		{
			delete pData;
			pData = nullptr;
			break;
		}

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
		pData->wstrPrototypeTag_Object = szObjectProtoTypeTag;

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

		if (FAILED(Add_GameObject_Prototype(szObjectClassName, pData, eType)))
		{
			PRINT_LOG(L"Error", L"Failed To Add_GameObject_Prototype");
			return E_FAIL;
		}

	}
	//
	fin.close();

	return S_OK;
}

HRESULT CStreamHandler::Load_PassData_Map(const wstring& wstrFilePath)
{
	HANDLE hFile = CreateFile(wstrFilePath.c_str(), GENERIC_READ
		, NULL, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"불러오기 실패!", L"실패", MB_OK);
		return E_FAIL;
	}

	PASSDATA_MAP tPassDataMap;
	DWORD		dwByte = 0;

	while (true)
	{
		ReadFile(hFile, &tPassDataMap.wstrPrototypeTag, sizeof(wstring), &dwByte, nullptr);
		ReadFile(hFile, &tPassDataMap.matWorld, sizeof(D3DXMATRIX), &dwByte, nullptr);
		ReadFile(hFile, &tPassDataMap.wstrCloneName, sizeof(wstring), &dwByte, nullptr);
		ReadFile(hFile, &tPassDataMap.Rotate, sizeof(_float3), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		TRANSFORM_DESC TransformDesc;
		TransformDesc.vPosition = { tPassDataMap.matWorld._41, tPassDataMap.matWorld._42, tPassDataMap.matWorld._43 };
		TransformDesc.vRotate = { tPassDataMap.Rotate.x, tPassDataMap.Rotate.y, tPassDataMap.Rotate.z };
		TransformDesc.vScale = { tPassDataMap.matWorld._11, tPassDataMap.matWorld._22, tPassDataMap.matWorld._33 };
		TransformDesc.matWorld = tPassDataMap.matWorld;

		// EResourceType 파싱데이터에서 받아오기
		Add_GameObject_Layer(EResourceType::NonStatic, tPassDataMap.wstrPrototypeTag, &TransformDesc);
	}

	CloseHandle(hFile);

	return S_OK;
}

<<<<<<< HEAD
HRESULT CStreamHandler::Load_PassData_Ui(const wstring& wstrFilePath)
{
	//wifstream fin;
	//wstring strPassFileName = L"../../Resources/Data/";
	//strPassFileName += wstrFilePath;
	//strPassFileName += L".txt";
	//fin.open(strPassFileName);

	//if (!fin.fail())
	//{
	//	TCHAR szTexturePrototypeTag[MAX_PATH] = L"";
	//	TCHAR szPosX[MAX_PATH] = L"";
	//	TCHAR szPosY[MAX_PATH] = L"";
	//	TCHAR szScaleX[MAX_PATH] = L"";
	//	TCHAR szScaleY[MAX_PATH] = L"";

	//	while (true)
	//	{
	//		fin.getline(szTexturePrototypeTag, MAX_PATH, L'|');
	//		fin.getline(szPosX, MAX_PATH, L'|');
	//		fin.getline(szPosY, MAX_PATH, L'|');
	//		fin.getline(szScaleX, MAX_PATH, L'|');
	//		fin.getline(szScaleY, MAX_PATH);

	//		if (fin.eof())
	//			break;
	//		// 코드 띄어쓰기나, 주석, 함수로분리 하자! (코드 가독성 떨어짐)

	//		// 이런거 현재 프레임워크 이해가 안된상태임
	//		// 구조분석 다시 ㄱㄱ
	//		CUI* pUi = CUI::Create(CManagement::Get_Instance()->Get_Device());
	//		TRANSFORM_DESC UiTrans;
	//		_float PosX = _ttof(szPosX);
	//		_float PosY = _ttof(szPosY);
	//		_float ScaleX = _ttof(szScaleX);
	//		_float ScaleY = _ttof(szScaleY);
	//		UiTrans.vPosition = { PosX,PosY,0 };
	//		UiTrans.vScale = { ScaleX,ScaleY,0.f };
	//		UI_DESC UiDesc;
	//		UiDesc.wstrTexturePrototypeTag = szTexturePrototypeTag;
	//		wstring strObjectName = szTexturePrototypeTag;
	//		L"Component_Texture_";
	//		strObjectName.erase(0, 18);
	//		UiDesc.tTransformDesc = UiTrans;
	//		wstring strProtoTypeTag = L"GameObject_" + strObjectName;
	//		pUi->Ready_GameObject(&UiDesc);
	//		//////////////////////////////////////////////////////////////////////////

	//		if (FAILED(CManagement::Get_Instance()->Add_GameObject_Prototype(EResourceType::NonStatic, szTexturePrototypeTag, pUi)))
	//		{
	//			PRINT_LOG(L"Error", L"Add_GameObject_Prototype Ui Failed");
	//			Safe_Release(pUi);
	//			return E_FAIL;
	//		}

	//		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(EResourceType::NonStatic, szTexturePrototypeTag
	//			, L"Layer_Ui", &UiDesc)))
	//		{
	//			PRINT_LOG(L"Error", L"Add_GameObject_InLayerTool_Failed");
	//			Safe_Release(pUi);
	//			return E_FAIL;
	//		}
	//	}
	//}

	return S_OK;
}

HRESULT CStreamHandler::Load_PassData_Resource(const wstring& wstrFilePath)
{
	//// 함수로 구분짓기 (코드 가독성 너무 떨어짐)
	//// 파일경로 풀로 받는게 더 유연성있음
	//wifstream fin;
	//wstring strPassFileName = L"../../Resources/Data/";
	//strPassFileName += wstrFilePath;
	//strPassFileName += L".txt";
	//fin.open(strPassFileName);

	//if (!fin.fail())
	//{
	//	TCHAR szFilePath[MAX_PATH] = L"";
	//	TCHAR szPrototypeTag[MAX_PATH] = L"";

	//	TCHAR szType[MAX_PATH] = L"";
	//	TCHAR szCount[MAX_PATH] = L"";

	//	while (true)
	//	{
	//		fin.getline(szFilePath, MAX_PATH, L'|');
	//		fin.getline(szPrototypeTag, MAX_PATH, L'|');
	//		fin.getline(szType, MAX_PATH, L'|');
	//		fin.getline(szCount, MAX_PATH);

	//		if (fin.eof())
	//			break;

	//		PASSDATA_RESOURCE pPathInfo;
	//		pPathInfo.wstrFilePath = szFilePath;
	//		pPathInfo.wstrPrototypeTag = szPrototypeTag;

	//		// 텍스쳐 타입 안맞음 (엔진쪽 건들어서 그럼)
	//		// Texture Type
	//		ETextureType eType = ETextureType::SINGLE;
	//		if (!lstrcmp(szType, L"SINGLE"))
	//			pPathInfo.dwResourceType = (DWORD)ETextureType::SINGLE;
	//		else if (!lstrcmp(szType, L"MULTI"))
	//			pPathInfo.dwResourceType = (DWORD)ETextureType::MULTI;
	//		else if (!lstrcmp(szType, L"CUBE"))
	//			pPathInfo.dwResourceType = (DWORD)ETextureType::Cube;

	//		// Texture Count
	//		pPathInfo.dwTextureCount = _ttoi(szCount);

	//		TCHAR szType[32] = L"";
	//		switch (pPathInfo.dwResourceType)
	//		{
	//		case (DWORD)ETextureType::Cube:
	//			eType = ETextureType::Cube;
	//			swprintf_s(szType, L".dds");
	//			break;
	//		case (DWORD)ETextureType::SINGLE:
	//			eType = ETextureType::SINGLE;
	//			swprintf_s(szType, L".png");
	//			break;
	//		case(DWORD)ETextureType::MULTI:
	//			eType = ETextureType::MULTI;
	//			swprintf_s(szType, L".png");
	//			break;
	//		}

	//		// 멀티텍스쳐 사용하는 구조 다시보고 오기
	//		wstring wstrTag = L"Component_Texture_";
	//		wstrTag += pPathInfo.wstrPrototypeTag;
	//		wstrTag += L"%d";
	//		wstrTag += szType;
	//		TCHAR szTagBuf[MAX_PATH] = {};
	//		TCHAR szPathBuf[MAX_PATH] = {};
	//		for (int i = 0; i < pPathInfo.dwTextureCount; i++)
	//		{
	//			swprintf_s(szTagBuf, wstrTag.c_str(), i);
	//			swprintf_s(szPathBuf, pPathInfo.wstrFilePath.c_str(), i);
	//			if (wstrFilePath == L"Static")
	//			{
	//				// 1장짜리 멀티텍스쳐가 여러개생겨버림
	//				if (FAILED(CManagement::Get_Instance()->Add_Component_Prototype(
	//					EResourceType::Static, szTagBuf,
	//					CTexture::Create(CManagement::Get_Instance()->Get_Device()
	//						, eType, szPathBuf))))
	//				{
	//					wstring Err = L"Failed To Add " + wstrTag;
	//					PRINT_LOG(L"Error", Err.c_str());
	//					return E_FAIL;
	//				}
	//			}
	//			else
	//			{

	//				if (FAILED(CManagement::Get_Instance()->Add_Component_Prototype(
	//					EResourceType::NonStatic, szTagBuf,
	//					CTexture::Create(CManagement::Get_Instance()->Get_Device()
	//						, eType, szPathBuf))))
	//				{
	//					wstring Err = L"Failed To Add " + wstrTag;
	//					PRINT_LOG(L"Error", Err.c_str());
	//					return E_FAIL;
	//				}
	//			}
	//		}
	//	}

	//}
	//fin.close();

	return S_OK;
}

HRESULT CStreamHandler::Add_GameObject_Prototype(const wstring& wstrClassName, const PASSDATA_OBJECT* tPassDataObject)
=======
HRESULT CStreamHandler::Add_GameObject_Prototype(const wstring & wstrClassName, PASSDATA_OBJECT * pPassDataObject, EResourceType eType)
>>>>>>> SJH_Clien
{
	if (wstrClassName == L"Player")
	{
		if (FAILED(CManagement::Get_Instance()->Add_GameObject_Prototype(
			eType,
			pPassDataObject->wstrPrototypeTag_Object,
			CPlayer::Create(CManagement::Get_Instance()->Get_Device(), pPassDataObject))))
		{
			PRINT_LOG(L"Error", L"Failed To Add GameObject_Player");
			return E_FAIL;
		}
		return S_OK;
	}

	return S_OK;
}

HRESULT CStreamHandler::Add_GameObject_Layer(EResourceType eType, const wstring& PrototypeTag, void* pArg)
{
	if (PrototypeTag == L"") 
	{
		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(eType, PrototypeTag, L"Layer_Dummy", pArg))) {
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += PrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}

	return S_OK;
}
