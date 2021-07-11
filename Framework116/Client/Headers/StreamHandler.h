#pragma once

#ifndef __STREAM_HANDLER_H__

USING(Engine)

class CStreamHandler final
{
private:
	CStreamHandler() = default;
	~CStreamHandler() = default;

public: 
	static HRESULT Load_PassData_Object(const wstring& wstrObjectPrototypePath);
	static HRESULT Load_PassData_Map(const wstring& wstrFilePath);
	static HRESULT Load_PassData_Resource(const wstring& wstrFilePath);
	static HRESULT Load_PassData_Ui(const wstring& wstrFilePath);


private:
	static HRESULT Add_GameObject_Prototype(const wstring& wstrClassName, const PASSDATA_OBJECT* tPassDataObject);
	static HRESULT Add_GameObject_Layer(EResourceType eType, const wstring& PrototypeTag, void* pArg);

};

#define __STREAM_HANDLER_H__
#endif // !__STREAM_HANDLER_H__
