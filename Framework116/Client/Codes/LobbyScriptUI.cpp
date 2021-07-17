#include "stdafx.h"
#include "..\Headers\LobbyScriptUI.h"
#include "Collision.h"
#include "Pipeline.h"
#include "Ring.h"
#include "Player.h"
#include"LobbyCam.h"
#include "QuestHandler.h"
#include"Lobby.h"

CLobbyScriptUI::CLobbyScriptUI(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CLobbyScriptUI::CLobbyScriptUI(const CLobbyScriptUI & other)
	: CGameObject(other)
{
}

HRESULT CLobbyScriptUI::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLobbyScriptUI::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	UI_DESC* uiDescPtr = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (uiDescPtr = dynamic_cast<UI_DESC*>(ptr))
		{
			m_wstrTexturePrototypeTag = uiDescPtr->wstrTexturePrototypeTag;
			m_tTransformDesc = uiDescPtr->tTransformDesc;
		}
	}

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_RectTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		m_wstrTexturePrototypeTag,
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		(void*)&m_tTransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}


	UI_DESC HUD_DESC;
	HUD_DESC.tTransformDesc.vPosition = { 0.f, 740.f, 0.f };
	HUD_DESC.tTransformDesc.vScale = { 1920.f, 350.f, 0.f };
	HUD_DESC.wstrTexturePrototypeTag = L"Component_Texture_ScriptUI_BlackBar";
	if (FAILED(Add_Layer_UI(L"Layer_HUD_BlackBar", &HUD_DESC)))
		return E_FAIL;

	HUD_DESC.tTransformDesc.vPosition = { 0.f, -740.f, 0.f };
	if (FAILED(Add_Layer_UI(L"Layer_HUD_BlackBar", &HUD_DESC)))
		return E_FAIL;


	m_pTransfrom_BlackBar_Up =
		(CTransform*)m_pManagement->Get_Component(L"Layer_HUD_BlackBar", L"Com_Transform");
	Safe_AddRef(m_pTransfrom_BlackBar_Up);

	m_pTransfrom_BlackBar_Down =
		(CTransform*)m_pManagement->Get_Component(L"Layer_HUD_BlackBar", L"Com_Transform", 1);
	Safe_AddRef(m_pTransfrom_BlackBar_Down);

	m_eScriptFlow = EScriptFlow::BlackBar_Start;

	return S_OK;
}

_uint CLobbyScriptUI::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	if (m_pLobby->Get_StartUnPacking())
		BlackBar_End(fDeltaTime);
	else if (m_pLobby->Get_IsGatcha())
		BlackBar_Start(fDeltaTime);
	else 
		BlackBar_End(fDeltaTime);


	return m_pTransform->Update_Transform();
}

_uint CLobbyScriptUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
		return UPDATE_ERROR;
	


	return _uint();
}

_uint CLobbyScriptUI::Render_GameObject()
{
	CGameObject::Render_GameObject();

	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = transformDesc.vScale.x;
	matView._22 = transformDesc.vScale.y;
	matView._41 = transformDesc.vPosition.x;
	matView._42 = transformDesc.vPosition.y;
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);

	m_pVIBuffer->Render_VIBuffer();
	//m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	return _uint();
}




void CLobbyScriptUI::BlackBar_Start(_float fDeltaTime)
{
	if (m_vUI_BlackBar_Up_Pos.y <= 510.f)
	{
		return ;
	}
	_float fSpeedPerSec = 0.f;
	
	fSpeedPerSec = 10.f;

	_float3 vDir = { 0.f, 1.f, 0.f };

	m_vUI_BlackBar_Up_Pos -= vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Up->Set_Position(m_vUI_BlackBar_Up_Pos);
	m_vUI_BlackBar_Down_Pos += vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Down->Set_Position(m_vUI_BlackBar_Down_Pos);

}

void CLobbyScriptUI::BlackBar_End(_float fDeltaTime)
{
	if (m_vUI_BlackBar_Up_Pos.y >= 740.f)
	{
		return;
	}
	else if (m_vUI_BlackBar_Up_Pos.y <= 0.f)
	{
		return;
	}
	_float fSpeedPerSec = 0.f;
	
		fSpeedPerSec = 10.f;
	_float3 vDir = { 0.f, 1.f, 0.f };

	m_vUI_BlackBar_Up_Pos += vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Up->Set_Position(m_vUI_BlackBar_Up_Pos);
	m_vUI_BlackBar_Down_Pos -= vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Down->Set_Position(m_vUI_BlackBar_Down_Pos);

}

HRESULT CLobbyScriptUI::Add_Layer_UI(const wstring & LayerTag, const UI_DESC * pUIDesc)
{
	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::NonStatic,
		L"GameObject_LobbyBackUI",
		LayerTag,
		(void*)pUIDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add UI In Layer");
		return E_FAIL;
	}
	return S_OK;
}





_bool CLobbyScriptUI::Get_IsScriptEnd()
{
	if(m_eScriptFlow == EScriptFlow::Flow_End)
		return true;

	return false;
}

CLobbyScriptUI * CLobbyScriptUI::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLobbyScriptUI* pInstance = new CLobbyScriptUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create TutorialUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLobbyScriptUI::Clone(void * pArg/* = nullptr*/)
{
	CLobbyScriptUI* pClone = new CLobbyScriptUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone TutorialUI");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLobbyScriptUI::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransfrom_BlackBar_Up);
	Safe_Release(m_pTransfrom_BlackBar_Down);


	CGameObject::Free();
}
