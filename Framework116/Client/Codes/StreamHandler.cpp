#include "stdafx.h"
#include "StreamHandler.h"
#include"Player.h"
#pragma region GameObject
#pragma endregion

HRESULT CStreamHandler::Load_PassData_Object(const wstring& wstrObjectPrototypePath, EResourceType eType)
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
	wifstream fin;
	fin.open(wstrFilePath.c_str());

	if (fin.fail())
	{
		PRINT_LOG(L"Path Error", wstrFilePath.c_str());
		return E_FAIL;
	}

	TCHAR szPrototypeTag[MAX_PATH] = L"";
	TCHAR matWorld_11[MAX_PATH] = L"";
	TCHAR matWorld_12[MAX_PATH] = L"";
	TCHAR matWorld_13[MAX_PATH] = L"";
	TCHAR matWorld_14[MAX_PATH] = L"";
	TCHAR matWorld_21[MAX_PATH] = L"";
	TCHAR matWorld_22[MAX_PATH] = L"";
	TCHAR matWorld_23[MAX_PATH] = L"";
	TCHAR matWorld_24[MAX_PATH] = L"";
	TCHAR matWorld_31[MAX_PATH] = L"";
	TCHAR matWorld_32[MAX_PATH] = L"";
	TCHAR matWorld_33[MAX_PATH] = L"";
	TCHAR matWorld_34[MAX_PATH] = L"";
	TCHAR matWorld_41[MAX_PATH] = L"";
	TCHAR matWorld_42[MAX_PATH] = L"";
	TCHAR matWorld_43[MAX_PATH] = L"";
	TCHAR matWorld_44[MAX_PATH] = L"";
	TCHAR RotateX[MAX_PATH] = L"";
	TCHAR RotateY[MAX_PATH] = L"";
	TCHAR RotateZ[MAX_PATH] = L"";
	TCHAR szCloneName[MAX_PATH] = L"";

	wstring wstrProtoTypeName = L"";
	_float4x4 matWorld;
	_float3 vRot;

	while (true)
	{
		fin.getline(szPrototypeTag, MAX_PATH, L'|');
		fin.getline(matWorld_11, MAX_PATH, '|');
		fin.getline(matWorld_12, MAX_PATH, '|');
		fin.getline(matWorld_13, MAX_PATH, '|');
		fin.getline(matWorld_14, MAX_PATH, '|');
		fin.getline(matWorld_21, MAX_PATH, '|');
		fin.getline(matWorld_22, MAX_PATH, '|');
		fin.getline(matWorld_23, MAX_PATH, '|');
		fin.getline(matWorld_24, MAX_PATH, '|');
		fin.getline(matWorld_31, MAX_PATH, '|');
		fin.getline(matWorld_32, MAX_PATH, '|');
		fin.getline(matWorld_33, MAX_PATH, '|');
		fin.getline(matWorld_34, MAX_PATH, '|');
		fin.getline(matWorld_41, MAX_PATH, '|');
		fin.getline(matWorld_42, MAX_PATH, '|');
		fin.getline(matWorld_43, MAX_PATH, '|');
		fin.getline(matWorld_44, MAX_PATH, '|');
		fin.getline(RotateX, MAX_PATH, '|');
		fin.getline(RotateY, MAX_PATH, '|');
		fin.getline(RotateZ, MAX_PATH, '|');
		fin.getline(szCloneName, MAX_PATH);

		if (fin.eof())
			break;

		wstrProtoTypeName = szPrototypeTag;
		matWorld._11 = (_float)_ttof(matWorld_11);
		matWorld._12 = (_float)_ttof(matWorld_12);
		matWorld._13 = (_float)_ttof(matWorld_13);
		matWorld._14 = (_float)_ttof(matWorld_14);
		matWorld._21 = (_float)_ttof(matWorld_21);
		matWorld._22 = (_float)_ttof(matWorld_22);
		matWorld._23 = (_float)_ttof(matWorld_23);
		matWorld._24 = (_float)_ttof(matWorld_24);
		matWorld._31 = (_float)_ttof(matWorld_31);
		matWorld._32 = (_float)_ttof(matWorld_32);
		matWorld._33 = (_float)_ttof(matWorld_33);
		matWorld._34 = (_float)_ttof(matWorld_34);
		matWorld._41 = (_float)_ttof(matWorld_41);
		matWorld._42 = (_float)_ttof(matWorld_42);
		matWorld._43 = (_float)_ttof(matWorld_43);
		matWorld._44 = (_float)_ttof(matWorld_44);
		vRot.x = (_float)_ttof(RotateX);
		vRot.y = (_float)_ttof(RotateY);
		vRot.z = (_float)_ttof(RotateZ);

		TRANSFORM_DESC TransformDesc;
		TransformDesc.vPosition = { matWorld._41, matWorld._42, matWorld._43 };
		TransformDesc.vRotate = { vRot.x, vRot.y, vRot.z };
		//TransformDesc.vScale = { matWorld._11, matWorld._22, matWorld._33 };
		TransformDesc.matWorld = matWorld;

		// EResourceType 파싱데이터에서 받아오기
		Add_GameObject_Layer(EResourceType::NonStatic, wstrProtoTypeName, &TransformDesc);
	}

	fin.close();

	return S_OK;
}

HRESULT CStreamHandler::Load_PassData_UI(const wstring& wstrFilePath, const _bool _isStatic)
{
	wifstream fin;
	fin.open(wstrFilePath);

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
			fin.getline(szScaleX, MAX_PATH, L'|');
			fin.getline(szScaleY, MAX_PATH);

			if (fin.eof())
				break;
			UI_DESC UiDesc;
			_float PosX = (_float)_ttof(szPosX);
			_float PosY = (_float)_ttof(szPosY);
			_float ScaleX = (_float)_ttof(szScaleX);
			_float ScaleY = (_float)_ttof(szScaleY);
			UiDesc.tTransformDesc.vPosition = { PosX,PosY,0 };
			UiDesc.tTransformDesc.vScale = { ScaleX,ScaleY,0.f };
			UiDesc.wstrTexturePrototypeTag = szTexturePrototypeTag;


			EResourceType eResourceType = (EResourceType)(!_isStatic);

			if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
				eResourceType, L"GameObject_UI"
				, L"Layer_UI", &UiDesc)))
			{
				PRINT_LOG(L"Error", L"Add_GameObject_InLayerTool_Failed");
				return E_FAIL;
			}
		}
	}


	return S_OK;
}

HRESULT CStreamHandler::Load_PassData_Resource(const wstring& wstrFilePath, const _bool _isStatic)
{
	wifstream fin;
	fin.open(wstrFilePath);
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

			PASSDATA_RESOURCE pPathInfo;
			pPathInfo.wstrFilePath = szFilePath;
			pPathInfo.wstrPrototypeTag = szPrototypeTag;

			// Texture Type
			ETextureType eType;
			if (!lstrcmp(szType, L"SINGLE"))
			{
				eType = ETextureType::Normal;
				pPathInfo.dwResourceType = (DWORD)ETextureType::Normal;
			}
			else if (!lstrcmp(szType, L"MULTI"))
			{
				eType = ETextureType::Normal;
				pPathInfo.dwResourceType = (DWORD)ETextureType::Normal;
			}
			else if (!lstrcmp(szType, L"CUBE"))
			{
				eType = ETextureType::Cube;
				pPathInfo.dwResourceType = (DWORD)ETextureType::Cube;
			}

			// Texture Count
			pPathInfo.dwTextureCount = _ttoi(szCount);

			wstring wstrTag = L"Component_Texture_";
			wstrTag += pPathInfo.wstrPrototypeTag;
			EResourceType eResourceType = (EResourceType)(!_isStatic);

			if (FAILED(CManagement::Get_Instance()->Add_Component_Prototype(
				eResourceType, wstrTag,
				CTexture::Create(CManagement::Get_Instance()->Get_Device()
					, eType, pPathInfo.wstrFilePath.c_str()
					, pPathInfo.dwTextureCount))))
			{
				wstring Err = L"Failed To Add " + wstrTag;
				PRINT_LOG(L"Error", Err.c_str());
				return E_FAIL;
			}

		}

	}
	fin.close();
	return S_OK;
}

HRESULT CStreamHandler::Load_PassData_Collide(const wstring& wstrFileName, const wstring& wstrMeshPrototypeTag, PASSDATA_COLLIDE& OutPassData)
{
	wstring wstrFilePath = L"../../Resources/Data/";
	wstrFilePath += wstrFileName;
	wstrFilePath += L".collide";

	HANDLE hFile = CreateFile(wstrFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile) {
		PRINT_LOG(L"Error", L"Failed to CreateFile Collide");
		return E_FAIL;
	}

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	TCHAR* pMeshPrototypeTag = nullptr;

	// 메시 프로토타입 태그 받아오기
	ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
	pMeshPrototypeTag = new TCHAR[dwStrByte];
	ReadFile(hFile, pMeshPrototypeTag, dwStrByte, &dwByte, nullptr);
	OutPassData.wstrMeshPrototypeTag = pMeshPrototypeTag;

	if (wstrMeshPrototypeTag != pMeshPrototypeTag) {
		PRINT_LOG(L"Warning", L"Failed to Matching with Mesh");
		Safe_Delete_Array(pMeshPrototypeTag);
		return E_FAIL;
	}
	Safe_Delete_Array(pMeshPrototypeTag);

	// 바운딩스피어 정보 로드
	size_t boundsCnt = 0;
	ReadFile(hFile, &boundsCnt, sizeof(size_t), &dwByte, nullptr);
	OutPassData.vecBoundingSphere.reserve(boundsCnt);

	BOUNDING_SPHERE tBounds;
	for (size_t i = 0; i < boundsCnt; ++i)
	{
		ReadFile(hFile, &tBounds, sizeof(BOUNDING_SPHERE), &dwByte, nullptr);
		OutPassData.vecBoundingSphere.emplace_back(tBounds);
	}

	CloseHandle(hFile);

	return S_OK;
}



HRESULT CStreamHandler::Add_GameObject_Prototype(const wstring& wstrClassName, PASSDATA_OBJECT* pPassDataObject, EResourceType eType)
{
	if (wstrClassName == L"Player")
	{
		if (FAILED(CManagement::Get_Instance()->Add_GameObject_Prototype(
			eType,
			pPassDataObject->wstrPrototypeTag_Object.c_str(),
			CPlayer::Create(CManagement::Get_Instance()->Get_Device(), pPassDataObject))))
		{
			PRINT_LOG(L"Error", L"Failed To Add GameObject_Player");
			return E_FAIL;
		}
		return S_OK;
	}
	return S_OK;
}

HRESULT CStreamHandler::Add_GameObject_Layer(EResourceType eType, wstring PrototypeTag, void* pArg)
{
	wstring wstrPrototypeTag = L"GameObject_" + PrototypeTag;
	wstring wstrLayerTag = L"Layer_" + PrototypeTag;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		eType, 
		wstrPrototypeTag,
		wstrLayerTag, pArg))) 
	{
		wstring errMsg = L"Failed to Add Layer ";
		errMsg += PrototypeTag;
		PRINT_LOG(L"Error", errMsg.c_str());
		return E_FAIL;
	}
	

	//if (wstrPrototypeTag == L"GameObject_Ring")
	//{
	//	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(eType, wstrPrototypeTag, L"Layer_Ring", pArg))) {
	//		wstring errMsg = L"Failed to Add Layer ";
	//		errMsg += PrototypeTag;
	//		PRINT_LOG(L"Error", errMsg.c_str());
	//		return E_FAIL;
	//	}
	//}

	return S_OK;
}
