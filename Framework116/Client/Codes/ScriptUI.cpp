#include "stdafx.h"
#include "..\Headers\ScriptUI.h"
#include "Collision.h"
#include "Pipeline.h"
#include "Ring.h"
#include "Player.h"
#include "MainCam.h"

CScriptUI::CScriptUI(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CScriptUI::CScriptUI(const CScriptUI & other)
	: CUI(other)
{
}

HRESULT CScriptUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CScriptUI::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CUI::Ready_GameObject(pArg);


	UI_DESC HUD_DESC;
	HUD_DESC.tTransformDesc.vPosition = { 0.f, 740.f, 0.f };
	HUD_DESC.tTransformDesc.vScale = { 1920.f, 300.f, 0.f };
	HUD_DESC.wstrTexturePrototypeTag = L"Component_Texture_ScriptUI_BlackBar";
	if (FAILED(Add_Layer_UI(L"Layer_HUD_BlackBar", &HUD_DESC)))
		return E_FAIL;

	HUD_DESC.tTransformDesc.vPosition = { 0.f, 740.f, 0.f };
	if (FAILED(Add_Layer_UI(L"Layer_HUD_BlackBar", &HUD_DESC)))
		return E_FAIL;

	HUD_DESC.tTransformDesc.vPosition = { -700.f, -700.f, 0.f };
	HUD_DESC.tTransformDesc.vScale = { 240.f, 320.f, 0.f };
	HUD_DESC.wstrTexturePrototypeTag = L"Component_Texture_Player_Portrait";
	if (FAILED(Add_Layer_UI(L"Layer_HUD_Portrait", &HUD_DESC)))
		return E_FAIL;



	m_pTransfrom_BlackBar_Up = (CTransform*)m_pManagement->Get_Component(L"Layer_HUD_BlackBar", L"Com_Transform");
	Safe_AddRef(m_pTransfrom_BlackBar_Up);

	m_pTransfrom_BlackBar_Down = (CTransform*)m_pManagement->Get_Component(L"Layer_HUD_BlackBar", L"Com_Transform", 1);
	Safe_AddRef(m_pTransfrom_BlackBar_Down);

	m_pTransfrom_Portrait = (CTransform*)m_pManagement->Get_Component(L"Layer_HUD_Portrait", L"Com_Transform");
	Safe_AddRef(m_pTransfrom_Portrait);


	// 여기서 카메라 잠그고 플레이어 잠금
	((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsScript(true);
	((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::Lock);


	return S_OK;
}

_uint CScriptUI::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);

	switch (m_eScriptFlow)
	{
	case EScriptFlow::BlackBar_Start:
		BlackBar_Start(fDeltaTime);
		break;
	case EScriptFlow::Script:
		Script_Check();
		break;
	case EScriptFlow::BlackBar_End:
		BlackBar_End(fDeltaTime);
		break;
	default:
		break;
	}


	return m_pTransform->Update_Transform();
}

_uint CScriptUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;

	if (m_eScriptFlow == EScriptFlow::Flow_End)
	{
		((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsScript(false);
		((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::Stage1_Ring);

		return DEAD_OBJECT;
	}

	return _uint();
}

_uint CScriptUI::Render_GameObject()
{
	CUI::Render_GameObject();

	//m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();

	m_fScriptTime += m_pManagement->Get_DeltaTime();

	if (m_fScriptTime >= 0.02f)
	{
		m_fScriptTime = 0.f;
		++m_dwScriptCount;
	}

	if (m_dwScriptCount >= m_dwScriptCountMax)
		m_dwScriptCount = m_dwScriptCountMax;


	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 900;
	m_pManagement->Get_Font()->DrawText(NULL
		, m_wstrScript.c_str(), _int(m_dwScriptCount)
		, &m_tUIBounds, DT_CENTER, D3DXCOLOR(200, 200, 200, 255));

	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 950;
	m_tUIBounds.right -= 1200;
	m_pManagement->Get_Font()->DrawText(NULL
		, m_wstrName.c_str(), -1
		, &m_tUIBounds, DT_CENTER, D3DXCOLOR(200, 200, 200, 255));


	//m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);


	return _uint();
}

_uint CScriptUI::Script_Check()
{

	switch (m_eScriptMode)
	{
	case EScript::Tutorial:
		Script_Tutorial();
		break;
	default:
		break;
	}


	return _uint();
}

void CScriptUI::Script_Tutorial()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_IsPlayerPortrait = true;
		m_wstrName = L"김쥬신";
		m_wstrScript = L"가나다라마바사아자차카타파하";
		break;
	case 1:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관";
		m_wstrScript = L"으하하 메롱";
		break;

	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
}

void CScriptUI::BlackBar_Start(_float fDeltaTime)
{
	_float fSpeedPerSec = 3.f;
	_float3 vDir = { 0.f, 1.f, 0.f };

	m_vUI_BlackBar_Up_Pos -= vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Up->Set_Position(m_vUI_BlackBar_Up_Pos);
	m_vUI_BlackBar_Down_Pos += vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Down->Set_Position(m_vUI_BlackBar_Down_Pos);

	if (m_vUI_BlackBar_Up_Pos.y <= 540.f)
	{
		m_eScriptFlow = EScriptFlow::Script;
		m_pTransfrom_Portrait->Set_Position(m_vUI_Protrait_Pos);
	}
}

void CScriptUI::BlackBar_End(_float fDeltaTime)
{
	_float fSpeedPerSec = 3.f;
	_float3 vDir = { 0.f, 1.f, 0.f };

	m_vUI_BlackBar_Up_Pos += vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Up->Set_Position(m_vUI_BlackBar_Up_Pos);
	m_vUI_BlackBar_Down_Pos -= vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Down->Set_Position(m_vUI_BlackBar_Down_Pos);

	if (m_vUI_BlackBar_Up_Pos.y >= 740.f)
	{
		m_eScriptFlow = EScriptFlow::Flow_End;
		m_pTransfrom_Portrait->Set_Position(_float3(0.f,-800.f,0.f));
	}
}

HRESULT CScriptUI::Add_Layer_UI(const wstring & LayerTag, const UI_DESC * pUIDesc)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_UI",
		LayerTag,
		(void*)pUIDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}
	return S_OK;
}

_uint CScriptUI::Set_NextScript()
{
	if (m_dwScriptCount >= m_dwScriptCountMax)
	{
		++m_dwScriptNext;
		m_fScriptTime = 0.f;
		m_dwScriptCount = 0;
	}

	else if (m_dwScriptCount < m_dwScriptCountMax)
		m_dwScriptCount = m_dwScriptCountMax;

	return _uint();
}

_uint CScriptUI::Set_Script(EScript eScript)
{
	m_eScriptMode = eScript;
	m_eScriptFlow = EScriptFlow::BlackBar_Start;

	return _uint();
}

CScriptUI * CScriptUI::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CScriptUI* pInstance = new CScriptUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create TutorialUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CScriptUI::Clone(void * pArg/* = nullptr*/)
{
	CScriptUI* pClone = new CScriptUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone TutorialUI");
		Safe_Release(pClone);
	}

	return pClone;
}

void CScriptUI::Free()
{
	Safe_Release(m_pTransfrom_Name);
	Safe_Release(m_pTransfrom_Portrait);
	Safe_Release(m_pTransfrom_BlackBar_Up);
	Safe_Release(m_pTransfrom_BlackBar_Down);


	CUI::Free();
}
