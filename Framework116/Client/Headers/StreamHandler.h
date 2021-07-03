#pragma once

#ifndef __STREAM_HANDLER_H__

USING(Engine)

class CStreamHandler final
{
private:
	CStreamHandler() = default;
	~CStreamHandler() = default;

public: 
	static HRESULT Load_PassData_Object(const wstring& wstrObjectPrototypePath, EResourceType eType = EResourceType::NonStatic);
	static HRESULT Load_PassData_Map(const wstring& wstrFilePath);

	static HRESULT Load_PassData_UI(const wstring& wstrFilePath, const _bool _isStatic);
	static HRESULT Load_PassData_Resource(const wstring& wstrFilePath, const _bool _isStatic);
private:
	static HRESULT Add_GameObject_Prototype(const wstring& wstrClassName, PASSDATA_OBJECT* pPassDataObject, EResourceType eType);
	static HRESULT Add_GameObject_Layer(EResourceType eType, const wstring& PrototypeTag, void* pArg);

};

#define __STREAM_HANDLER_H__
#endif // !__STREAM_HANDLER_H__
