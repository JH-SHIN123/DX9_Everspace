#pragma once
#ifndef __SHIELD_BATTERY_H__

#include "GameObject.h"

USING(Engine)

//typedef struct tagPlanetDesc : public BASE_DESC {
//	wstring wstrTextureTag = L"";
//	TRANSFORM_DESC tTransformDesc;
//}PLANET_DESC;


class CShield_Battery final : public CGameObject
{
public:
	explicit CShield_Battery(LPDIRECT3DDEVICE9 pDevice);
	explicit CShield_Battery(const CShield_Battery& other);
	virtual ~CShield_Battery() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CShield_Battery* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CModelMesh*			m_pModelMesh = nullptr;
	CTransform*			m_pTransform = nullptr;
	CTransform*			m_pPlayerTransform = nullptr;
	//CTexture*			m_pTexture = nullptr;
	CCollideSphere*		m_pCollide = nullptr;

	_float m_fRadian = 0.f;
	_float m_fLifeTime = 0.f;
	_float m_IsAdd = true;

private:
	//따라다니게
	_float3 m_vPos;

};

#define __SHIELD_BATTERY_H__
#endif