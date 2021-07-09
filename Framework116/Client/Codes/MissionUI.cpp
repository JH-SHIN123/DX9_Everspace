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


	return S_OK;
}

_uint CMissionUI::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);
	

	CQuestHandler::Get_Instance()->Update_Quest();

	m_wstrMissionName = CQuestHandler::Get_Instance()->Get_QusetName();
	m_iMissionCount = CQuestHandler::Get_Instance()->Get_CountRemaining();
	m_iMissionMaxCount = CQuestHandler::Get_Instance()->Get_CountMax();
	m_IsClear = CQuestHandler::Get_Instance()->Get_IsClear();



	return m_pTransform->Update_Transform();
}

_uint CMissionUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);
	
	//if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
	//	return UPDATE_ERROR;

	return _uint();
}

_uint CMissionUI::Render_GameObject()
{
	CUI::Render_GameObject();

	wstring Info = to_wstring(m_iMissionCount) + L"/" + to_wstring(m_iMissionMaxCount);

	RECT m_tUIBounds;
	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 500;
	m_tUIBounds.left += 1700;
	m_pManagement->Get_Font()->DrawText(NULL
		, m_wstrMissionName.c_str(), -1
		, &m_tUIBounds, DT_CENTER, D3DXCOLOR(200, 200, 200, 255));

	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 550;
	m_tUIBounds.left += 1650;
	m_pManagement->Get_Font()->DrawText(NULL
		, Info.c_str(), -1
		, &m_tUIBounds, DT_CENTER, D3DXCOLOR(200, 200, 200, 255));

	//wstring IsClear
	//if(m_IsClear == true)

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
	CUI::Free();
}
