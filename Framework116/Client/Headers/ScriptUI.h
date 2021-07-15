#pragma once
#ifndef __SCRIPTUI_H__

#include "GameObject.h"
#include "UI.h"
#include "BackUI.h"

USING(Engine)

enum class EScript {
	Tutorial,
	Tutorial_Ring_Clear,
	Tutorial_Target_Clear,
	Stg2_Begin,Stg2_AfterCamProduction,
	Stg2_Finish_AsteroidFlyAway,
	Stg2_SearchTarget,
	Stg2_PlayerDead,
	End
};

enum class EScriptFlow
{
	BlackBar_Start,
	Script,
	BlackBar_End,
	Flow_End,
	End
};

class CScriptUI final : public CGameObject
{
public:
	explicit CScriptUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CScriptUI(const CScriptUI& other);
	virtual ~CScriptUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	_uint Set_NextScript();
	_uint Set_Script(EScript eScript);
	_bool Get_IsScriptEnd();

private: //대화 순서
	void BlackBar_Start(_float fDeltaTime);
	_uint Script_Check();
	void BlackBar_End(_float fDeltaTime);

private: //대화
	void Script_Tutorial();
	void Script_Tutorial_Ring_Clear();
	void Script_Tutorial_Target_Clear();
	void Script_Stg2_Begin();
	void Script_Stg2_AfterCamProduction();
	void Script_Stg2_Finish_AsteroidFlyAway();
	void Script_Stg2_Search_Target();
	void Script_Stg2_PlayerDead();
private: // 기타
	void Lock_Cursor();
	void Portrait_Check();
	HRESULT Add_Layer_UI(const wstring& LayerTag, const UI_DESC* pUIDesc);


public:
	static CScriptUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	wstring m_wstrName = L"";
	wstring m_wstrScript = L"";
	RECT m_tPortrait;
	DWORD m_dwScriptNext;	// 다음 대화
	DWORD m_dwScriptCount;	// 한글자씩 끊어서 출력
	DWORD m_dwScriptCountMax;// 최대치
	_float m_fScriptTime;	// 끊어서 출력 시간
	_bool m_IsPlayerPortrait = false;
	_float m_fShowBlackBar = 3.f;
	EScript m_eScriptMode = EScript::End;
	EScriptFlow m_eScriptFlow = EScriptFlow::End;

private:
	class CVIBuffer* m_pVIBuffer = nullptr;
	class CTransform* m_pTransform = nullptr;
	class CTexture* m_pTexture = nullptr;

private:
	wstring m_wstrTexturePrototypeTag = L"";
	TRANSFORM_DESC m_tTransformDesc;
	wstring m_wstrTextureTag_Portrait = L"";
	RECT m_tUIBounds;


private:	// m_pTransform = 대화 창
	CTransform* m_pTransfrom_BlackBar_Up = nullptr;
	CTransform* m_pTransfrom_BlackBar_Down = nullptr;
	CTransform* m_pTransfrom_Portrait = nullptr;
	CTransform* m_pTransfrom_Name = nullptr;


private:
	_float3 m_vUI_BlackBar_Up_Pos = { 0.f, 740.f, 0.f };
	_float3 m_vUI_BlackBar_Down_Pos = { 0.f, -740.f, 0.f };
	_float3 m_vUI_Protrait_Pos = { -700.f, -500.f, 0.f };

	//Sound
	_bool m_bSoundOnce = false;
	EPortraitNumber m_ePortrait = EPortraitNumber::End;


};

#define __SCRIPTUI_H__
#endif