#pragma once

#ifndef __PROTOCOL_H__

USING(Engine)
// Passing Data
typedef struct tagPassingData_Object {
	
#ifdef _AFX
	CString wstrPrototypeTag;
	CString wstrPrototypeTag_Mesh;
#else 
	wstring wstrPrototypeTag_Object;
	wstring wstrPrototypeTag_Mesh;
#endif // _AFX
	
	// Material Info
	D3DMATERIAL9 tMaterial;		

	// 파싱 데이터가
	// 오브젝트의 프로토타입 태그
	// 오브젝트가 쓸 매쉬
	// 재질 정보 이게 끝이라면다른 컴포넌트는 안넣는건가?

	// 아마 이거랑, 다른 컴포넌트들 넣을 수 있도록 짜야하나?
	// 아니면 이걸로 전부?
}PASSDATA_OBJECT;


typedef struct tagPassingData_Map {
	UINT eObjectType;
	_float4x4 matWorld;
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

#define __PROTOCOL_H__
#endif