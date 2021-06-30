#pragma once

#ifndef __PROTOCOL_H__

USING(Engine)
// Passing Data
typedef struct tagPassingData_Object {

#ifdef _AFX
	CString wstrPrototypeTag;
	vector<CString> vecPrototypeTag_Mesh;
#else 
	wstring wstrPrototypeTag_Object;
	vector<wstring> vecPrototypeTag_Mesh;
#endif // _AFX

	// Material Info
	D3DMATERIAL9 tMaterial;

}PASSDATA_OBJECT;

typedef struct tagPassingData_Map {
	wstring wstrPrototypeTag;
	_float4x4 matWorld;
	_float3 Rotate;
	int iNodeOrder;
	wstring wstrCloneName;
}PASSDATA_MAP;

typedef struct tagPassingData_Route {
	_float3 vNodePos;
	int iNodeOrder;
}PASSDATA_ROUTE;

typedef struct tagPassingData_Resource {
#ifdef _AFX
	CString wstrPrototypeTag;
	CString wstrFilePath;
#else 
	wstring wstrPrototypeTag;
	wstring wstrFilePath;
#endif // _AFX

	DWORD dwResourceType;
	DWORD dwTextureCount;
}PASSDATA_RESOURCE;

typedef struct tagPassingData_UI {
	UINT eObjectType;
	_float4x4 matWorld;
}PASSDATA_UI;

#define __PROTOCOL_H__
#endif