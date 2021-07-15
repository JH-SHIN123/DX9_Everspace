#include "stdafx.h"
#include "StreamHandler.h"

#pragma region GameObject
#include"Player.h"
#include "NaviArrow.h"
#pragma endregion

HRESULT CStreamHandler::Load_PassData_Map(const TCHAR* wstrFilePath)
{
	HANDLE hFile = CreateFile(wstrFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile) {
		PRINT_LOG(L"Error", L"Failed to Load Map");
		return E_FAIL;
	}

	DWORD dwByte = 0;
	DWORD dwStrByte = 0;

	vector<PASSDATA_MAP> vecPassData;
	PASSDATA_MAP tPassMap;
	TCHAR* pTag = nullptr;

	while (true)
	{
		// Obj Prototype Tag
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		pTag = new TCHAR[dwStrByte];
		ReadFile(hFile, pTag, dwStrByte, &dwByte, nullptr);
		tPassMap.wstrPrototypeTag = pTag;
		Safe_Delete_Array(pTag);

		// Mesh Prototype Tag
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		pTag = new TCHAR[dwStrByte];
		ReadFile(hFile, pTag, dwStrByte, &dwByte, nullptr);
		tPassMap.wstrMeshName = pTag;
		Safe_Delete_Array(pTag);

		// Mesh Transform
		ReadFile(hFile, &tPassMap.matWorld, sizeof(tPassMap.matWorld), &dwByte, nullptr);
		ReadFile(hFile, &tPassMap.Pos, sizeof(tPassMap.Pos), &dwByte, nullptr);
		ReadFile(hFile, &tPassMap.Rotate, sizeof(tPassMap.Rotate), &dwByte, nullptr);
		ReadFile(hFile, &tPassMap.Scale, sizeof(tPassMap.Scale), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		vecPassData.emplace_back(tPassMap);
	}

	CloseHandle(hFile);

	for (auto& p : vecPassData)
	{
		Add_GameObject_Layer_Map(&p);
	}

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
				eResourceType, L"GameObject_LobbyUI"
				, L"Layer_UI", &UiDesc)))
			{
				PRINT_LOG(L"Error", L"Add_GameObject_InLayerTool_Failed");
				return E_FAIL;
			}
		}
	}

	fin.close();

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
	wstring wstrFilePath = L"../../Resources/Data/Collide/";
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

HRESULT CStreamHandler::Load_PassData_Navi(const TCHAR* wstrFilePath)
{
	HANDLE hFile = CreateFile(wstrFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile) {
		PRINT_LOG(L"Error", L"Failed to Load Map");
		return E_FAIL;
	}

	DWORD dwByte = 0;

	vector<PASSDATA_ROUTE> vecRoutes;
	PASSDATA_ROUTE tRoute;

	while (true)
	{
		// Mesh Transform
		ReadFile(hFile, &tRoute, sizeof(tRoute), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		vecRoutes.emplace_back(tRoute);
	}

	CloseHandle(hFile);

	for (auto& p : vecRoutes)
		Add_GameObject_Layer_Route(&p);

	return S_OK;
}

HRESULT CStreamHandler::Get_PassData_Navi(vector<PASSDATA_ROUTE>& vecOutRoutes, const TCHAR* wstrFilePath)
{
	HANDLE hFile = CreateFile(wstrFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (INVALID_HANDLE_VALUE == hFile) {
		PRINT_LOG(L"Error", L"Failed to Load Map");
		return E_FAIL;
	}

	DWORD dwByte = 0;

	vector<PASSDATA_ROUTE> vecRoutes;
	PASSDATA_ROUTE tRoute;

	while (true)
	{
		// Mesh Transform
		ReadFile(hFile, &tRoute, sizeof(tRoute), &dwByte, nullptr);

		if (0 == dwByte)
			break;

		vecRoutes.emplace_back(tRoute);
	}

	CloseHandle(hFile);

	// 데이터 저장
	vecOutRoutes.swap(vecRoutes);

	return S_OK;
}

HRESULT CStreamHandler::Add_GameObject_Layer_Map(const PASSDATA_MAP* pPassData)
{
	if (nullptr == pPassData) {
		PRINT_LOG(L"Error", L"(Load Map) Failed To Add Layer");
		return E_FAIL;
	}

	wstring wstrPrototypeTag = pPassData->wstrPrototypeTag;

	if (wstrPrototypeTag == L"GameObject_Player")
	{
		if (CManagement::Get_Instance()->Get_GameObject(L"Layer_Player"))
		{
			CTransform* pTransform = (CTransform*)CManagement::Get_Instance()->Get_GameObject(L"Layer_Player")->Get_Component(L"Com_Transform");
			pTransform->Set_Position(pPassData->Pos);
			pTransform->Set_Rotate(pPassData->Rotate);
			pTransform->Set_Scale(pPassData->Scale);
		}
		else 
		{
			GAMEOBJECT_DESC tDesc;
			tDesc.tTransformDesc.matWorld = pPassData->matWorld;
			tDesc.tTransformDesc.vPosition = pPassData->Pos;
			tDesc.tTransformDesc.vRotate = pPassData->Rotate;
			tDesc.tTransformDesc.vScale = pPassData->Scale;
			tDesc.wstrMeshName = pPassData->wstrMeshName;

			if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
				EResourceType::Static,
				wstrPrototypeTag,
				L"Layer_Player",
				&tDesc)))
			{
				wstring errMsg = L"Failed to Add Layer ";
				errMsg += wstrPrototypeTag;
				PRINT_LOG(L"Error", errMsg.c_str());
				return E_FAIL;
			}
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Ring")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			wstrPrototypeTag,
			L"Layer_Ring",
			&tDesc)))
		{
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += wstrPrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Planet" || wstrPrototypeTag == L"GameObject_Planet_Ice")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			wstrPrototypeTag,
			L"Layer_Planet",
			&tDesc)))
		{
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += wstrPrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Asteroid_A" || wstrPrototypeTag == L"GameObject_Asteroid_B")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Asteroid",
			L"Layer_Asteroid",
			&tDesc)))
		{
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += wstrPrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Asteroid_C" || wstrPrototypeTag == L"GameObject_Asteroid_D")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale * 0.1f;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Asteroid",
			L"Layer_Asteroid",
			&tDesc)))
		{
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += wstrPrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Rock_Cloud")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Asteroid",
			L"Layer_Asteroid_Deco",
			&tDesc)))
		{
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += wstrPrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Drone")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Drone",
			L"Layer_Drone",
			&tDesc)))
		{
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += wstrPrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Monster")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			wstrPrototypeTag,
			L"Layer_Monster",
			&tDesc)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Monster In Layer");
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Sniper")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			wstrPrototypeTag,
			L"Layer_Sniper",
			&tDesc)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Monster In Layer");
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Boss")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Boss_Monster",
			L"Layer_Boss_Monster",
			&tDesc)))
		{
			PRINT_LOG(L"Error", L"Failed To Add Monster In Layer");
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Broken_Plane")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Broken_Plane",
			L"Layer_Broken_Plane",
			&tDesc)))
		{
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += wstrPrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}
	else if (wstrPrototypeTag == L"GameObject_Delivery")
	{
		GAMEOBJECT_DESC tDesc;
		tDesc.tTransformDesc.matWorld = pPassData->matWorld;
		tDesc.tTransformDesc.vPosition = pPassData->Pos;
		tDesc.tTransformDesc.vRotate = pPassData->Rotate;
		tDesc.tTransformDesc.vScale = pPassData->Scale;
		tDesc.wstrMeshName = pPassData->wstrMeshName;

		if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
			EResourceType::NonStatic,
			L"GameObject_Delivery",
			L"Layer_Delivery",
			&tDesc)))
		{
			wstring errMsg = L"Failed to Add Layer ";
			errMsg += wstrPrototypeTag;
			PRINT_LOG(L"Error", errMsg.c_str());
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CStreamHandler::Add_GameObject_Layer_Route(const PASSDATA_ROUTE* pPassData)
{
	if (nullptr == pPassData)
		return E_FAIL;

	NAVI_ARROW_DESC tDesc;
	tDesc.vPos = pPassData->vNodePos;
	tDesc.vDir = pPassData->vNodeDir;

	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_NaviArrow",
		L"Layer_NaviArrow",
		(void*)&tDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add GameObject_NaviArrow In Layer");
		return E_FAIL;
	}

	return S_OK;
}
