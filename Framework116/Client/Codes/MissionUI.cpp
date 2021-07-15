#include "stdafx.h"
#include "..\Headers\MissionUI.h"
#include "Collision.h"
#include "Pipeline.h"
#include "Ring.h"
#include "QuestHandler.h"

CMissionUI::CMissionUI(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CMissionUI::CMissionUI(const CMissionUI & other)
	: CUI(other)
{
}

HRESULT CMissionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CMissionUI::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CUI::Ready_GameObject(pArg);

	UI_DESC Desc;
	Desc.tTransformDesc.vPosition = { 835.f, 13.f ,0.f };
	Desc.tTransformDesc.vScale = { 140.f, 30.f,0.f };
	Desc.wstrTexturePrototypeTag = L"Component_Texture_HUD_Mission_Name";

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_BackUI",
		L"GameObject_MissionUI_Name", &Desc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add ScriptUI In Layer");
		return E_FAIL;
	}

	Desc.tTransformDesc.vPosition = { 735.f, -50.f ,0.f };
	Desc.tTransformDesc.vScale = { 64.f, 64.f,0.f };
	Desc.wstrTexturePrototypeTag = L"Component_Texture_HUD_Mission_Deco";

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_BackUI",
		L"GameObject_MissionUI_Deco", &Desc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add ScriptUI In Layer");
		return E_FAIL;
	}


	m_pTransform_Name = (CTransform*)((m_pManagement->Get_GameObject(L"GameObject_MissionUI_Name"))->Get_Component(L"Com_Transform"));
	if (m_pTransform_Name == nullptr)
	{
		PRINT_LOG(L"Error", L"GameObject_MissionUI_Name is nullptr ");
		return E_FAIL;
	}
	Safe_AddRef(m_pTransform_Name);



	m_pTransform_Deco = (CTransform*)((m_pManagement->Get_GameObject(L"GameObject_MissionUI_Deco"))->Get_Component(L"Com_Transform"));
	if (m_pTransform_Name == nullptr)
	{
		PRINT_LOG(L"Error", L"GameObject_MissionUI_Deco is nullptr ");
		return E_FAIL;
	}
	Safe_AddRef(m_pTransform_Deco);


	return S_OK;
}

_uint CMissionUI::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);
	

	//CQuestHandler::Get_Instance()->Update_Quest();

	m_wstrMissionName = CQuestHandler::Get_Instance()->Get_QusetName();
	m_iMissionCount = CQuestHandler::Get_Instance()->Get_CountRemaining();
	m_iMissionMaxCount = CQuestHandler::Get_Instance()->Get_CountMax();
	m_IsClear = CQuestHandler::Get_Instance()->Get_IsClear();


	return m_pTransform->Update_Transform();
}

_uint CMissionUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);
	
	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CMissionUI::Render_GameObject()
{
	CUI::Render_GameObject();

	wstring Info = to_wstring(m_iMissionCount) + L" / " + to_wstring(m_iMissionMaxCount);
	if (L"운석을 피하라" == CQuestHandler::Get_Instance()->Get_QusetName())
	{
		wstring strCount = L"";
		if (m_iMissionCount > 10)
			strCount = L"00 : " + to_wstring(m_iMissionCount);
		else
			strCount = L"00 : 0" + to_wstring(m_iMissionCount);
		wstring strMaxCount = L"00 : " + to_wstring(m_iMissionMaxCount);
		Info = strCount + L" / " + strMaxCount;
	}
	RECT m_tUIBounds;
	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 517;
	m_tUIBounds.left += 1670;
	m_pManagement->Get_Font()->DrawText(NULL
		, L"Mission", -1
		, &m_tUIBounds, DT_CENTER, D3DXCOLOR(200, 200, 200, 255));

	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 570;
	m_tUIBounds.left += 1670;
	m_pManagement->Get_Font()->DrawText(NULL
		, m_wstrMissionName.c_str(), -1
		, &m_tUIBounds, DT_CENTER, D3DXCOLOR(200, 200, 200, 255));

	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 620;
	m_tUIBounds.left += 1670;
	m_pManagement->Get_Font()->DrawText(NULL
		, Info.c_str(), -1
		, &m_tUIBounds, DT_CENTER, D3DXCOLOR(200, 200, 200, 255));

	//wstring IsClear
	//if(m_IsClear == true)

	return _uint();
}

_uint CMissionUI::Key_Input()
{
	return _uint();
}

CMissionUI * CMissionUI::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CMissionUI* pInstance = new CMissionUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create TutorialUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMissionUI::Clone(void * pArg/* = nullptr*/)
{
	CMissionUI* pClone = new CMissionUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone TutorialUI");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMissionUI::Free()
{
	Safe_Release(m_pTransform_Name);
	Safe_Release(m_pTransform_Deco);

	CUI::Free();
}
