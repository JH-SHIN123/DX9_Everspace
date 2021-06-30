#pragma once

#ifndef __DUMMY_H__

#include "GameObject.h"

typedef struct tagDummyDesc : public BASE_DESC{
	wstring wstrMeshPrototypeTag;
	TRANSFORM_DESC tTransformDesc;
}DUMMY_DESC;

USING(Engine)
class CDummy final : public CGameObject
{
public:
	explicit CDummy(LPDIRECT3DDEVICE9 pDevice);
	explicit CDummy(const CDummy& other);
	virtual ~CDummy() = default;

public:
	const wstring& Get_MeshPrototypeTag() const { return m_wstrMeshPrototypeTag; }

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CDummy* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	class CGeometryMesh* m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	wstring m_wstrMeshPrototypeTag = L"";
};

#define __DUMMY_H__
#endif