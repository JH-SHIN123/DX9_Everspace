#pragma once
#include "UI.h"
class CLobbyUI final :
	public CUI
{
private:
	explicit CLobbyUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CLobbyUI(const CLobbyUI& other);
	virtual ~CLobbyUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;


private:
	void Update_Bounds();
	void Check_Picking();
	void Key_Check(_float fDeltaTime);
	void OnMouseButton();

private:
	//For.SceneSelect
	void Render_Cursor();
	void Render_Nodes();
	void Update_SceneSelect(_float fDeltaTime);

	//For.ItemSlot
	_bool Render_ItemSlot();
	void Add_Font_InLayer(wstring strLayerTag,CGameObject*& pFont,wstring str,_float3 vPos,_float3 vScale,D3DXCOLOR tColor);
public:
	void Set_GotoNextScene(_bool bSet) { m_bGotoNextScene = bSet; }
	void Set_StartUnPacking(_bool bSet) { m_bStartUnPacking = bSet; }
	void Set_CancelSceneSelect(_bool bSet) { m_bCancelSceneChange = bSet; }
	void Update_ItemFrame(_float fDeltaTime);

	_bool Get_CancelSceneSelect() { return m_bCancelSceneChange; }
	wstring Get_strTexturePrototypeTag() { return m_wstrTexturePrototypeTag; }
	_uint Get_SelectedScene() { return m_iSelect; }
public:
	static CLobbyUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
public:
	void Set_Scene(class CLobby* _pUI);
	class CLobby* Get_Scene() { return m_pLobby; }

	//for Skip
	void Set_GatchaBox(class CGatchaBox* pBox) { m_pBox = pBox; }
	void Set_Product(class CProduct* pProduct) { m_pProduct = pProduct; }
	void Set_Model(class CLobbyModel* pModel) { m_pModel = pModel; }
	
private:
	class CController* m_pController = nullptr;

	//for KeyCheck
	_float m_fDeltaTime = 0;
	DWORD m_dwIdx = 0;
	class CLobby* m_pLobby = nullptr;
	//for Skip
	class CGatchaBox* m_pBox = nullptr;
	class CProduct* m_pProduct = nullptr;
	class CLobbyModel* m_pModel = nullptr;
	_bool m_bCancel = false;

	_bool m_bGotoNextScene = false;
	_bool m_bDead = false;
	_bool m_bStartUnPacking = false;
	//for SceneSelect
	_float3 m_vCursorPos = {0.f,0.f,0.f};
	_float3 m_vNodeScale = { 0.f,0.f,0.f};
	_float3 m_vFirstNode = { 0.f,0.f,0.f };
	_float3 m_vSecondNode = { 0.f,0.f,0.f };
	_float3 m_vThirdNode = { 0.f,0.f,0.f };
	_uint m_iSelect = 0;
	_float m_fClicked = 0.f;
	_bool m_bClicked = false;

	_bool m_bCancelSceneChange = false;
	//For.ItemFrame
	_bool m_bStart = false;
	_float3 m_vStartPos = {0,0,0};

	CGameObject* m_pFontAtkUpCount = nullptr;
	CGameObject* m_pFontDefUpCount = nullptr;
	CGameObject* m_pFontHpUpCount = nullptr;
	CGameObject* m_pFontEnergyUpCount = nullptr;
	CGameObject* m_pFontMissileCount = nullptr;
	CGameObject* m_pFontVMaxCount = nullptr;
};

