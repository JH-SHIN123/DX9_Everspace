#pragma once

#ifndef __STREAM_HANDLER_H__

USING(Engine)

class CStreamHandler final
{
private:
	CStreamHandler() = default;
	~CStreamHandler() = default;

public: 
	static HRESULT Load_PassData_Map(const TCHAR* wstrFilePath);
	static HRESULT Load_PassData_UI(const wstring& wstrFilePath, const _bool _isStatic);
	static HRESULT Load_PassData_Resource(const wstring& wstrFilePath, const _bool _isStatic);
	static HRESULT Load_PassData_Collide(const wstring& wstrFileName, const wstring& wstrMeshPrototypeTag, PASSDATA_COLLIDE& OutPassData);
	static HRESULT Load_PassData_Navi(const TCHAR* wstrFilePath);

private:
	static HRESULT Add_GameObject_Layer_Map(const PASSDATA_MAP* pPassData);
	static HRESULT Add_GameObject_Layer_Route(const PASSDATA_ROUTE* pPassData);

};

#define __STREAM_HANDLER_H__
#endif // !__STREAM_HANDLER_H__
