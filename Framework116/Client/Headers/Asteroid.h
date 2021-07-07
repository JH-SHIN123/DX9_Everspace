#pragma once
#ifndef __ASTEROID_H__

#include "GameObject.h"

USING(Engine)

typedef struct tagAsteroidDesc : public BASE_DESC {
	const TCHAR* pMeshTag = nullptr;
	TRANSFORM_DESC tTransformDesc;
}ASTEROID_DESC;

class CAsteroid final : public CGameObject
{
public:
	explicit CAsteroid(LPDIRECT3DDEVICE9 pDevice);
	explicit CAsteroid(const CAsteroid& other);
	virtual ~CAsteroid() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CAsteroid* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform* m_pTransform = nullptr;
	CModelMesh* m_pMesh = nullptr;
	CCollideSphere* m_pCollide = nullptr;

private:
	_float3 m_vRandomRotateDir = { 0.f,0.f,0.f };
	_float m_fMoveUpDeltaT = 0.f;
};

#define __ASTEROID_H__ 
#endif