#include "stdafx.h"
#include "StreamHandler.h"

#pragma region GameObject
#pragma endregion

HRESULT CStreamHandler::Load_PassData_Object(const wstring& wstrObjectPrototypePath)
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

		Add_GameObject_Prototype(szObjectClassName, pData);
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

HRESULT CStreamHandler::Add_GameObject_Prototype(const wstring& wstrClassName, const PASSDATA_OBJECT* tPassDataObject)
{
	//if (wstrClassName == L"DUMMY")
	//{
	//	if (FAILED(CManagement::Get_Instance()->Add_GameObject_Prototype(
	//		EResourceType::NonStatic,
	//		tPassDataObject->wstrPrototypeTag_Object,
	//		CDummy_Mon::Create(CManagement::Get_Instance()->Get_Device(), tPassDataObject))))
	//	{
	//		PRINT_LOG(L"Error", L"Failed To Add GameObject_Player");
	//		return E_FAIL;
	//	}
	//}

	//return S_OK;
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
