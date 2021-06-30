#include "stdafx.h"
#include "..\Headers\Load_Prototype.h"
#include <fstream>

#pragma region GameObject
#include "Dummy_Mon.h"
#pragma endregion


CLoad_Prototype::CLoad_Prototype()
{
}


CLoad_Prototype::~CLoad_Prototype()
{
}

_bool CLoad_Prototype::Load_PassData_Object(const wstring & wstrObjectPrototypePath, PASSDATA_OBJECT& tPassDataObject)
{
	PASSDATA_OBJECT tData;

	wifstream fin;
	fin.open(wstrObjectPrototypePath);


	if (fin.fail())
	{
		PRINT_LOG(L"Path Error", L"Wrong Path");
		return false;
	}

	TCHAR szObjectProtoTypeTag[MAX_PATH] = L"";
	TCHAR szMeshProtoTypeTag[MAX_PATH] = L"";
	TCHAR szMaterial_Diffuse[MAX_PATH][4] = { L"" };
	TCHAR szMaterial_Ambient[MAX_PATH][4] = { L"" };
	TCHAR szMaterial_Specular[MAX_PATH][4] = { L"" };
	TCHAR szMaterial_Emissive[MAX_PATH][4] = { L"" };
	TCHAR szMaterial_Power[MAX_PATH] = L"";

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
		//tPassDataObject.wstrPrototypeTag_Object = szObjectProtoTypeTag;
		//tPassDataObject.wstrPrototypeTag_Mesh = szMeshProtoTypeTag;
		//tPassDataObject.tMaterial = tMaterial;

		

		if (fin.eof())
			break;
	}
	fin.close();


	return true;
}

HRESULT CLoad_Prototype::Load_PassData_Object_Static(const wstring & wstrObjectPrototypePath)
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

		Create_Object_Prototype_Static(szObjectClassName, pData);

	}

	fin.close();



	return S_OK;
}

void CLoad_Prototype::Create_Object_Prototype_Static(const wstring& wstrClassName, PASSDATA_OBJECT* tPassDataObject)
{
	if (wstrClassName == L"DUMMY")
	{
		if (FAILED(CManagement::Get_Instance()->Add_GameObject_Prototype(
			EResourceType::NonStatic,
			tPassDataObject->wstrPrototypeTag_Object,
			CDummy_Mon::Create(CManagement::Get_Instance()->Get_Device(), tPassDataObject))))
		{
			PRINT_LOG(L"Error", L"Failed To Add GameObject_Player");
			return;
		}
	}
	
}
