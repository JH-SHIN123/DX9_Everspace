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
	void Set_Damage(_int iDamage);

public:
	const STAT_INFO Get_StatusInfo();
	const _int Get_Damage();

public:
	void Set_Attack(_int iAttack);
	void Set_Deffend(_int iDeffend);
	void Set_MaxHp(_int iMaxHp);
	void Set_Hp(_int iHp);
	void Set_MaxShield(_int iMaxShield);
	void Set_Shield(_int iShield);
	void Set_MaxEnergy(_int iMaxEnergy);
	void Set_Energy(_int iEnergy);
	void Set_FireRate(_int iFreRate);
	void Set_HittedDamage(_int iHittedDamage); //Hp바에 넣을 데미지

public:
	const _int Get_Attack();
	const _int Get_Deffend();
	const _int Get_MaxHp();
	const _int Get_Hp();
	const _int Get_MaxShield();
	const _int Get_Shield();
	const _int Get_MaxEnergy();
	const _int Get_Energy();
	const _int Get_FireRate();
	_int Get_HittedDamage();

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
