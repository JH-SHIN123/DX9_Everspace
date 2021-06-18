#pragma once
#ifndef __COMPONENT_MANAGER_H__

#include "Base.h"
#include "VIBuffer_TriColor.h"
#include "VIBuffer_RectColor.h"
#include "VIBuffer_RectTexture.h"
#include "VIBuffer_TerrainColor.h"
#include "VIBuffer_TerrainTexture.h"
#include "VIBuffer_CubeTexture.h"
#include "Transform.h"
#include "Texture.h"

BEGIN(Engine)
class CComponent_Manager : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)

private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Add_Component_Prototype(EResourceType eType, const wstring& PrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(EResourceType eType, const wstring& PrototypeTag, void* pArg = nullptr);
	void Clear_NonStatic_Resources();

public:
	virtual void Free() override;

private:
	typedef unordered_map<wstring, class CComponent*> COMPONENTS;
	COMPONENTS m_Components[(_uint)EResourceType::End];	/* 프로토타입 보관 */
};
END

#define __COMPONENT_MANAGER_H__
#endif