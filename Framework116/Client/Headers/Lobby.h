#pragma once
#include "Scene.h"

USING(Engine)
typedef struct tagUnitInfo
{
	tagUnitInfo() {};
	tagUnitInfo(_uint _iAtk, _uint _iDef,_uint _iMaxHp,_uint _iMaxShield,_uint _iMaxEnergy,_uint _iFireRate)
		:iAtk(_iAtk),iDef(_iDef),iHp(_iMaxHp),iMaxHp(_iMaxHp),iShield(_iMaxShield),iMaxShield(_iMaxShield)
		,iEnergy(_iMaxEnergy),iMaxEnergy(_iMaxEnergy),iFireRate(_iFireRate)
	{
	}

	_uint iAtk = 0;
	_uint iDef = 0;
	_uint iHp = 0;
	_uint iMaxHp = 0;
	_uint iShield = 0;
	_uint iMaxShield= 0;
	_uint iEnergy= 0;
	_uint iMaxEnergy = 0;
	_uint iFireRate = 0;
}UNIT_INFO;

#ifndef __LOBBY_H__
class CLobby final : public CScene
{
public:
	explicit CLobby(LPDIRECT3DDEVICE9 pDevice);
	virtual ~CLobby() = default;

public:
	virtual HRESULT Ready_Scene() override;
	virtual _uint Update_Scene(_float fDeltaTime) override;
	virtual _uint LateUpdate_Scene(_float fDeltaTime) override;
	
	HRESULT Add_Layer_Light(const wstring& LayerTag, const LIGHT_DESC* pLightDesc);
	HRESULT Add_Layer_Lobby_Model(const wstring& LayerTag);
	HRESULT Add_Layer_LobbyCam(const wstring& LayerTag);
	HRESULT Add_Layer_Skybox(const wstring & LayerTag);
	HRESULT Add_Layer_UI(const wstring & LayerTag);
	HRESULT Add_Layer_GatchaBox(const wstring & LayerTag);
	HRESULT Add_Layer_StatusBoard(const wstring & LayerTag);
	HRESULT Add_Layer_Status(const wstring & LayerTag);

private:

public:
	void Set_GotoNextScene(_bool bSet);
	void Set_IsGatcha(_bool bSet);
	void Set_IsSetPlayerModel(_bool bSet);
	void Set_SceneChange(_bool bSet);
	void Set_StartUnPacking(_bool bSet);
	void Set_SceneSelect(_bool bSet);

	_bool Get_IsGatcha()const;
	_bool Get_IsSetPlayerModel()const;
	_bool Get_GotoNextScene()const;
	_bool Get_SceneSelect()const;
	_bool Get_StartUnPacking()const;

	_uint Get_Money()const;
	UNIT_INFO* Get_UnitInfo();

	void Set_UnitInfo(UNIT_INFO _tUnitInfo);
	void Set_Money(_uint _iMoney);

	_uint GetAtkBuffItemCount() { return m_iAtkBuffItem; }
	_uint GetDefBuffItemCount() { return m_iDefBuffItem; }
	_uint GetHpBuffItemCount() { return m_iHpBuffItem; }
	_uint GetEnergyBuffItemCount() { return m_iEnergyBuffItem; }


	void SetAtkBuffItemCount(_uint iPlus) { m_iAtkBuffItem += iPlus; }
	void SetDefBuffItemCount(_uint iPlus) { m_iDefBuffItem += iPlus; }
	void SetHpBuffItemCount(_uint iPlus) { m_iHpBuffItem += iPlus; }
	void SetEnergyBuffItemCount(_uint iPlus) { m_iEnergyBuffItem += iPlus;  }


public:
	static CLobby* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual void Free() override;

private:
	class CPlayer* m_pPlayer = nullptr;

	
	_bool m_bGotoNextScene = false;
	_bool m_bIsGatcha = false;
	_bool m_bIsSetPlayerModel = false;
	_bool m_bSelectScene = false;
	_bool m_bSceneChange = false;
	_bool m_bStartUnPacking = false;

	_uint m_iMoney = 3000;
	UNIT_INFO m_tUnitInfo;

	_uint m_iAtkBuffItem = 0;
	_uint m_iDefBuffItem = 0;
	_uint m_iHpBuffItem = 0;
	_uint m_iEnergyBuffItem = 0;
};

#define __LOBBY_H__
#endif


