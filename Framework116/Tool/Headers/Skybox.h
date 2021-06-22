#pragma once
#ifndef __SKYBOX_H__

#include "GameObject.h"

/* 엔진에 있는 클래스를 전방선언할 때!! */
BEGIN(Engine)
class CCamera;
END

USING(Engine)
class CSkybox final : public CGameObject
{
public:
	explicit CSkybox(LPDIRECT3DDEVICE9 pDevice);
	explicit CSkybox(const CSkybox& other);
	virtual ~CSkybox() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CSkybox* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	CVIBuffer*  m_pVIBuffer = nullptr;
	CTransform* m_pTransform = nullptr;
	CTexture*	m_pTexture = nullptr;	
	CCamera* m_pCamera = nullptr;
};

#define __SKYBOX_H__
#endif