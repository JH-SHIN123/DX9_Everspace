#include "stdafx.h"
#include "..\Headers\ScriptUI.h"
#include "Collision.h"
#include "Pipeline.h"
#include "Ring.h"
#include "Player.h"

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

	return S_OK;
}

_uint CScriptUI::Update_GameObject(_float fDeltaTime)
{
	CUI::Update_GameObject(fDeltaTime);

	Script_Check();
	
	return m_pTransform->Update_Transform();
}

_uint CScriptUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CUI::LateUpdate_GameObject(fDeltaTime);
	
	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Alpha, this)))
		return UPDATE_ERROR;

	if (m_IsEndScript == true)
	{
		((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsScript(false);

		return DEAD_OBJECT;
	}

	return _uint();
}

_uint CScriptUI::Render_GameObject()
{
	CUI::Render_GameObject();

	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	m_fScriptTime += m_pManagement->Get_DeltaTime();

	if (m_fScriptTime >= 0.02f)
	{
		m_fScriptTime = 0.f;
		++m_dwScriptCount;
	}

	if (m_dwScriptCount >= m_dwScriptCountMax)
		m_dwScriptCount = m_dwScriptCountMax;

	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 850;
	m_pManagement->Get_Font()->DrawText(NULL
		, m_wstrScript.c_str(), _int(m_dwScriptCount)
		, &m_tUIBounds, DT_CENTER, D3DXCOLOR(200, 200, 200, 255));



	return _uint();
}

_uint CScriptUI::Script_Check()
{
	switch (m_eScriptMode)
	{
	case Tutorial:
		Script_Tutorial();
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
		m_IsEndScript = true;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
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

_uint CScriptUI::Set_Script(eScript eScript)
{
	m_eScriptMode = eScript;

	((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsScript(true);

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



	CUI::Free();
}
