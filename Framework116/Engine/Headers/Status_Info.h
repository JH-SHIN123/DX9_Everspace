#pragma once
#ifndef __STATUS_INFO_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStatus_Info final : public CComponent
{
private:
	explicit CStatus_Info(LPDIRECT3DDEVICE9 pDevice);
	explicit CStatus_Info(const CStatus_Info& other);
	virtual ~CStatus_Info() = default;

public:
	void Set_StatusInfo(const STAT_INFO& tStatusInfo);
	void Set_Damage(_uint iDamage);

public:
	const STAT_INFO Get_StatusInfo();
	const _uint Get_Damage();

public:
	void Set_Attack(_uint iAttack);
	void Set_Deffend(_uint iDeffend);
	void Set_MaxHp(_uint iMaxHp);
	void Set_Hp(_uint iHp);
	void Set_MaxShield(_uint iMaxShield);
	void Set_Shield(_uint iShield);
	void Set_MaxEnergy(_uint iMaxEnergy);
	void Set_Energy(_uint iEnergy);
	void Set_FireRate(_uint iFreRate);

public:
	const _uint Get_Attack();
	const _uint Get_Deffend();
	const _uint Get_MaxHp();
	const _uint Get_Hp();
	const _uint Get_MaxShield();
	const _uint Get_Shield();
	const _uint Get_MaxEnergy();
	const _uint Get_Energy();
	const _uint Get_FireRate();

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;

public:
	static CStatus_Info* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;


private:
	STAT_INFO m_tStatusInfo;

};

END
#define __STATUS_INFO_H__
#endif // !__STATUS_INFO_H__
