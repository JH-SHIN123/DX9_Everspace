#pragma once
#ifndef __COMPONENT_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	explicit CComponent(LPDIRECT3DDEVICE9 pDevice);
	explicit CComponent(const CComponent& other);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Ready_Component_Prototype() = 0;	/* 프로토타입 초기화 */
	virtual HRESULT Ready_Component(void* pArg = nullptr) = 0; /* 클론 초기화 */

public:
	virtual CComponent* Clone(void* pArg = nullptr) = 0;
	virtual void Free() = 0;

protected:
	LPDIRECT3DDEVICE9 m_pDevice = nullptr;
	_bool	m_IsClone = false;
};
END

#define __COMPONENT_H__
#endif