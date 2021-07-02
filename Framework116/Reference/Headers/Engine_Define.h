#pragma once
#ifndef __ENGINE_DEFINE_H__

#define UNIT			100.f

#define NO_EVENT		0
#define UPDATE_ERROR	1
#define DEAD_OBJECT		2
#define CHANGE_SCENE	3
#define RENDER_ERROR	4
#define LOADING_ERROR	5

#define BEGIN(Name) namespace Name {
#define END }
#define USING(Name) using namespace Name;

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif

#define PRINT_LOG(caption, message)				\
::MessageBox(0, message, caption, MB_OK)

#define NO_COPY(ClassName)						\
ClassName(const ClassName&) = delete;			\
ClassName& operator=(const ClassName&)  = delete;


#define DECLARE_SINGLETON(ClassName)			\
		NO_COPY(ClassName)						\
public:											\
	static ClassName* ClassName::Get_Instance();\
private:										\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)			\
ClassName* ClassName::m_pInstance = nullptr; 	\
ClassName* ClassName::Get_Instance()			\
{												\
	if (nullptr == m_pInstance)					\
		m_pInstance = new ClassName;			\
	return m_pInstance;							\
}

#define __ENGINE_DEFINE_H__
#endif