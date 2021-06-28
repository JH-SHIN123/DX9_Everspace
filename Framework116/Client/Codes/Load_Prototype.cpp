#include "stdafx.h"
#include "..\Headers\Load_Prototype.h"
#include <fstream>


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
