#pragma once
#ifndef __PRODUCT_H__

#include "GameObject.h"
enum class EProduct
{
	ATK_UP, DEF_UP, 
	MAX_HP_UP, MAX_SHIELD_UP,
	MONEY, SHIP0, SHIP1,
	PRODUCT_END
};

USING(Engine)
class CProduct final : public CGameObject
{
public:
	explicit CProduct(LPDIRECT3DDEVICE9 pDevice);
	explicit CProduct(const CProduct& other);
	virtual ~CProduct() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);
	void UpdateProduct(_float fDeltaTime);
	void Render_Product();
	void Get_Product();
	void Set_Text();
public:
	static CProduct* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

	void Set_Scene(class CLobby* pLobby) { m_pLobby = pLobby; }
	void Set_ShowProduct(_bool bSet) { m_bShowProduct = bSet; }
	void  Set_Cancel(_bool bSet) { m_bCancel = bSet; }
private:
	CTexture*	m_pTexture = nullptr;
	CTexture* m_pProductTex = nullptr;
	CVIBuffer_RectTexture* m_pProductVIBuffer = nullptr;

	CTransform* m_pTransform = nullptr;
	CGeoMesh_Torus* m_pGeoMesh = nullptr;


private:
	D3DMATERIAL9 m_tMaterial;
	_float4 vColorRGBA = { 0.004f, 0.002f, 0.005f, 0.f };
	_bool m_bHitRing = false;

	class CLobby* m_pLobby = nullptr;
	_float m_fFlyTime = 0.f;
	_bool m_bFall = false;
	_bool m_bShowProduct = false;
	EProduct m_eProduct = EProduct::PRODUCT_END;
	_float4x4 m_matProduct;
	_float m_fRotProductY = 0.f;
	_float m_fCountTime = 0.f;
	_float m_fCancelCount = 0.f;
	_bool m_bCancel = false;
};

#define __PRODUCT_H__ 
#endif