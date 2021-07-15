#pragma once

#ifndef __DELIVERY_H__

#include "GameObject.h"

USING(Engine)

class CDelivery final : public CGameObject
{
public:
	explicit CDelivery(LPDIRECT3DDEVICE9 pDevice);
	explicit CDelivery(const CDelivery& other);
	virtual ~CDelivery() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	void Set_MoveStart(_bool bMove = true);

public:
	_bool Get_IsArrive();

private:
	void Update_Effect();

private:
	_uint Movement(_float fDeltaTime);
	void FindNextRoute();

public:
	static CDelivery* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool m_bArrive = false;
	_bool m_IsStart = false;

private:
	CTransform* m_pTransform = nullptr;
	CMesh* m_pMesh = nullptr;

private: // Engine Trail
	_float3 m_vLeftEngineOffset = { 0.f, 0.f, 0.f };
	_float3 m_vRightEngineOffset = { 0.f, 0.f, 0.f };
	class CEngineEffectSystem* m_pLeftEngineEffect = nullptr;
	class CEngineEffectSystem* m_pRightEngineEffect = nullptr;

private:
	vector<PASSDATA_ROUTE> m_vecNaviRoute;
	_uint m_iCurRouteIndex = 0;

private:
	// HP Bar 추가
	CTransform* m_pPlayerTransform = nullptr;
	_bool m_IsHPBar = false;
	class CHP_Bar* m_pHp_Bar = nullptr;
	class CHP_Bar_Border* m_pHP_Bar_Border = nullptr;
	_float m_fHpLength = 128.f;
	_uint Check_Degree();

public: // Add_HP_Bar
	_uint Add_Hp_Bar(_float fDeltaTime);
	void Set_Hp_Pos();
	_bool Get_Is_Hp_Bar() { return m_IsHPBar; }

	// 구ㅡ조체
	CStatus_Info* m_pInfo = nullptr;
};


#define __DELIVERY_H__ 
#endif