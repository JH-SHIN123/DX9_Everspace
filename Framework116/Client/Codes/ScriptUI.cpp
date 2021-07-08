#include "stdafx.h"
#include "..\Headers\ScriptUI.h"
#include "Collision.h"
#include "Pipeline.h"
#include "Ring.h"
#include "Player.h"
#include "MainCam.h"

CScriptUI::CScriptUI(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CScriptUI::CScriptUI(const CScriptUI & other)
	: CGameObject(other)
	, m_vUI_Protrait_Pos(other.m_vUI_Protrait_Pos)
{
}

HRESULT CScriptUI::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CScriptUI::Ready_GameObject(void * pArg/* = nullptr*/)
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
	CGameObject::Update_GameObject(fDeltaTime);

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
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
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

	m_fScriptTime += m_pManagement->Get_DeltaTime();

	if (m_fScriptTime >= 0.02f)
	{
		m_fScriptTime = 0.f;
		++m_dwScriptCount;
	}

	if (m_dwScriptCount >= m_dwScriptCountMax)
		m_dwScriptCount = m_dwScriptCountMax;

	RECT m_tUIBounds;
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
	case EScript::Tutorial_Ring_Clear:

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
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관";
		m_wstrScript = L"오, 자네가 이번에 새로 들어왔다던 신병인가?";
		break;
	case 1:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관";
		m_wstrScript = L"자네가 임무를 수행 하기 전 거쳐야 할 훈련이 하나 있지";
		break;
	case 2:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관";
		m_wstrScript = L"오늘의 훈련을 진행 할 헥터 도일 사령관이라고 하네, 잘 부탁하네 제군";
		break;
	case 3:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"자 우선 고리를 통과해 보겠나?";
		break;
	case 4:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"WASD 방향키로 조종이 가능하지";
		break;
	case 5:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"그 전에 주위를 한번 둘러보게나";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
}

void CScriptUI::Script_Tutorial_Ring_Clear()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"아니 자네 경력있는 신입 뭐 그런건가?";
		break;
	case 1:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"비행 솜씨가 꽤 뛰어나군 그래!";
		break;
	case 2:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"오늘의 훈련을 진행 할 헥터 도일 사령관이라고 하네, 잘 부탁하네 제군";
		break;
	case 3:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"자 우선 고리를 통과해 보겠나?";
		break;
	case 4:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"WASD 방향키로 조종이 가능하지";
		break;
	case 5:
		m_IsPlayerPortrait = false;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"그 전에 주위를 한번 둘러보게나";
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
		m_pTransfrom_Portrait->Set_Position(_float3(-700.f, -200.f, 0.f));
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
		EResourceType::NonStatic,
		L"GameObject_BackUI",
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
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	Safe_Release(m_pTransfrom_Name);
	Safe_Release(m_pTransfrom_Portrait);
	Safe_Release(m_pTransfrom_BlackBar_Up);
	Safe_Release(m_pTransfrom_BlackBar_Down);


	CGameObject::Free();
}
