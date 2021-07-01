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

private:
	static void Add_GameObject_Prototype(const wstring& wstrClassName, const PASSDATA_OBJECT* tPassDataObject);

};

#define __STREAM_HANDLER_H__
#endif // !__STREAM_HANDLER_H__
