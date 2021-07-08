#pragma once

#ifndef __PROTOCOL_H__

USING(Engine)
// Passing Data
typedef struct tagPassingData_Object {
	
#ifdef _AFX
	CString wstrPrototypeTag;
	vector<CString> vecPrototypeTag_Mesh; // vecPrototypeTag_Component
#else 
	wstring wstrPrototypeTag_Object;
	vector<wstring> vecPrototypeTag_Mesh; // vecPrototypeTag_Component
#endif // _AFX

	// Material Info
	D3DMATERIAL9 tMaterial;

}PASSDATA_OBJECT;

typedef struct tagPassingData_Map {
#ifdef _AFX
	CString wstrPrototypeTag;
	CString wstrMeshName;
#else 
	wstring wstrPrototypeTag;
	wstring wstrMeshName;
#endif // _AFX

	_float4x4 matWorld;
	_float3 Pos;
	_float3 Rotate;
	_float3 Scale;

	int iNodeOrder;
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

typedef struct tagPassingData_Collide {
#ifdef _AFX
	CString wstrMeshPrototypeTag;
#else 
	wstring wstrMeshPrototypeTag;
#endif // _AFX

	vector<BOUNDING_SPHERE> vecBoundingSphere;
}PASSDATA_COLLIDE;

#define __PROTOCOL_H__
#endif