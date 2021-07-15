#include "..\Headers\Status_Info.h"

USING(Engine)

CStatus_Info::CStatus_Info(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CStatus_Info::CStatus_Info(const CStatus_Info & other)
	: CComponent(other)
	, m_tStatusInfo(other.m_tStatusInfo)
{
}

void CStatus_Info::Set_StatusInfo(const STAT_INFO & tStatusInfo)
{
	m_tStatusInfo = tStatusInfo;
}

void CStatus_Info::Set_Damage(_int iDamage)
{
	m_tStatusInfo.iHp -= iDamage;
}

const STAT_INFO CStatus_Info::Get_StatusInfo()
{
	return m_tStatusInfo;
}

const _int CStatus_Info::Get_Damage()
{
	return m_tStatusInfo.iAtk;
}

void CStatus_Info::Set_Attack(_int iAttack)
{
	m_tStatusInfo.iAtk = iAttack;
}

void CStatus_Info::Set_Deffend(_int iDeffend)
{
	m_tStatusInfo.iDef = iDeffend;
}

void CStatus_Info::Set_MaxHp(_int iMaxHp)
{
	m_tStatusInfo.iMaxHp = iMaxHp;
}

void CStatus_Info::Set_Hp(_int iHp)
{
	m_tStatusInfo.iHp = iHp;
}

void CStatus_Info::Set_MaxShield(_int iMaxShield)
{
	m_tStatusInfo.iMaxShield = iMaxShield;
}

void CStatus_Info::Set_Shield(_int iShield)
{
	m_tStatusInfo.iShield = iShield;
}

void CStatus_Info::Set_MaxEnergy(_int iMaxEnergy)
{
	m_tStatusInfo.iMaxEnergy = iMaxEnergy;
}

void CStatus_Info::Set_Energy(_int iEnergy)
{
	m_tStatusInfo.iEnergy = iEnergy;
}

void CStatus_Info::Set_FireRate(_int iFreRate)
{
	m_tStatusInfo.iFireRate = iFreRate;
}

void CStatus_Info::Set_HittedDamage(_int iHittedDamage)
{
	m_tStatusInfo.iHittedDamage = iHittedDamage;
}

const _int CStatus_Info::Get_Attack()
{
	return m_tStatusInfo.iAtk;
}

const _int CStatus_Info::Get_Deffend()
{
	return m_tStatusInfo.iDef;
}

const _int CStatus_Info::Get_MaxHp()
{
	return m_tStatusInfo.iMaxHp;
}

const _int CStatus_Info::Get_Hp()
{
	return m_tStatusInfo.iHp;
}

const _int CStatus_Info::Get_MaxShield()
{
	return m_tStatusInfo.iMaxShield;
}

const _int CStatus_Info::Get_Shield()
{
	return m_tStatusInfo.iShield;
}

const _int CStatus_Info::Get_MaxEnergy()
{
	return m_tStatusInfo.iMaxEnergy;
}

const _int CStatus_Info::Get_Energy()
{
	return m_tStatusInfo.iEnergy;
}

const _int CStatus_Info::Get_FireRate()
{
	return m_tStatusInfo.iFireRate;
}

_int CStatus_Info::Get_HittedDamage()
{
	return m_tStatusInfo.iHittedDamage;
}

HRESULT CStatus_Info::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	return S_OK;
}

HRESULT CStatus_Info::Ready_Component(void * pArg)
{
	CComponent::Ready_Component(pArg);

	m_tStatusInfo = *((STAT_INFO*)pArg);

	return S_OK;
}

CStatus_Info * CStatus_Info::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CStatus_Info* pInstance = new CStatus_Info(pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CStatus_Info::Clone(void * pArg)
{
	CStatus_Info* pClone = new CStatus_Info(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Texture");
		Safe_Release(pClone);
	}

	return pClone;
}

void CStatus_Info::Free()
{
	CComponent::Free();
}
