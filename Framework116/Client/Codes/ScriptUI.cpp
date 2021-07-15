#include "stdafx.h"
#include "..\Headers\ScriptUI.h"
#include "Collision.h"
#include "Pipeline.h"
#include "Ring.h"
#include "Player.h"
#include "MainCam.h"
#include "QuestHandler.h"


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
	HUD_DESC.tTransformDesc.vScale = { 1920.f, 350.f, 0.f };
	HUD_DESC.wstrTexturePrototypeTag = L"Component_Texture_ScriptUI_BlackBar";
	if (FAILED(Add_Layer_UI(L"Layer_HUD_BlackBar", &HUD_DESC)))
		return E_FAIL;

	HUD_DESC.tTransformDesc.vPosition = { 0.f, -740.f, 0.f };
	if (FAILED(Add_Layer_UI(L"Layer_HUD_BlackBar", &HUD_DESC)))
		return E_FAIL;

	HUD_DESC.tTransformDesc.vPosition = { -700.f, -700.f, 0.f };
	HUD_DESC.tTransformDesc.vScale = { 240.f, 320.f, 0.f };
	HUD_DESC.wstrTexturePrototypeTag = L"Component_Texture_Portrait";
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
	((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsCameraMove(true);
	((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::Lock);


	// 몬스터 공격/이동 멈춰
	CQuestHandler::Get_Instance()->Lock_MonsterAI(true);

	return S_OK;
}

_uint CScriptUI::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	Lock_Cursor();

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

	
	((CBackUI*)m_pManagement->Get_GameObject(L"Layer_HUD_Portrait"))->Change_TextureNumber(m_ePortrait);

	return m_pTransform->Update_Transform();
}

_uint CScriptUI::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
		return UPDATE_ERROR;

	// 대화가 끝나면
	if (m_eScriptFlow == EScriptFlow::Flow_End)
	{
		((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsScript(false);

		// 스크립트 모드 종료
		switch (m_eScriptMode)
		{
		case EScript::Tutorial:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::Stage1_Ring);
			break;
		case EScript::Tutorial_Ring_Clear:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::End);
			((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsCameraMove(false);
			CQuestHandler::Get_Instance()->Lock_MonsterAI(false);
			break;

		case EScript::Tutorial_Target_Clear:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::End);
			((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsCameraMove(false);
			CQuestHandler::Get_Instance()->Lock_MonsterAI(false);
			break;
		case  EScript::Stg2_Begin:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::Stage2_Asteroid);
			break;
		case EScript::Stg2_AfterCamProduction:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::End);
			((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsCameraMove(false);
			CQuestHandler::Get_Instance()->Lock_MonsterAI(false);
			break;
		case EScript::Stg2_Finish_AsteroidFlyAway:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::Stage2_FinishAsteroid);
			break;
		case EScript::Stg2_SearchTarget:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::End);
			((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Set_IsCameraMove(false);
			CQuestHandler::Get_Instance()->Lock_MonsterAI(false);
			break;
		case EScript::Stage3_Opening:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::Stage3_Delivery);
			break;
		case EScript::Stage3_Boss_Opening:
			((CMainCam*)m_pManagement->Get_GameObject(L"Layer_Cam"))->Set_IsSoloMove(ESoloMoveMode::Stage3_Boss);
			break;
		default:
			break;
		}

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

	Portrait_Check();

	GetClientRect(g_hWnd, &m_tUIBounds);
	m_tUIBounds.top += 930;
	m_tUIBounds.right -= 1400;
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
		Script_Tutorial_Ring_Clear();
		break;
	case EScript::Tutorial_Target_Clear:
		Script_Tutorial_Target_Clear();
		break;
	case EScript::Stg2_Begin:
		Script_Stg2_Begin();
		break;
	case EScript::Stg2_AfterCamProduction:
		Script_Stg2_AfterCamProduction();
		break;
	case EScript::Stg2_Finish_AsteroidFlyAway:
		Script_Stg2_Finish_AsteroidFlyAway();
		break;
	case EScript::Stg2_PlayerDead:
		Script_Stg2_PlayerDead();
		break;
	case EScript::Stg2_SearchTarget:
		Script_Stg2_Search_Target();
		break;
	case EScript::Stg2_Clear:
		Script_Stg2_Clear();
		break;
	case EScript::Stage3_Opening:
		Script_Stage3_Opening();
		break;
	case EScript::Stage3_Boss_Opening:
		Script_Stage3_Boss_Opening();
		break;
	case EScript::Stage3_Boss_Clear:
		Script_Stage3_Boss_Clear();
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
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrName = L"사령관";
		m_wstrScript = L"오, 자네가 이번에 새로 들어왔다던 신병인가?";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrName = L"사령관";
		m_wstrScript = L"자네가 임무를 수행 하기 전 거쳐야 할 훈련이 하나 있지";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrName = L"사령관";
		m_wstrScript = L"오늘의 훈련을 진행 할 헥터 도일 사령관이라고 하네, 잘 부탁하네 제군";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"자 우선 고리를 통과해 보겠나?";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrName = L"사령관 헥터 도일";
		m_wstrScript = L"그 전에 주위를 한번 둘러보게나";
		break;
	case 5:
		m_ePortrait = EPortraitNumber::End;
		m_wstrName = L"";
		m_wstrScript = L"(WASD 키를 이용하여 움직일 수 있습니다.)";
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
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"아니 자네 경력있는 신입 뭐 그런건가?";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"비행 솜씨가 꽤 뛰어나군 그래!";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"하하하 과찬이십니다.";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"주제를 넘을지 모르겠지만, 다음 임무는 무엇입니까?";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"이번에는 자유로이 움직이며 과녁을 모두 쏘면 된다네 그리 어렵진 않을걸세";
		break;
	case 5:
		m_ePortrait = EPortraitNumber::End;
		m_wstrScript = L"(마우스 좌클릭을 이용하여 무기를 발사 할 수 있습니다.)";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();

	Portrait_Check();
}

void CScriptUI::Script_Tutorial_Target_Clear()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"모두 완수하였습니다!";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"썩 괜찮은 재능이군 그래";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"좋든 나쁘든 자네는 바로 작전에 투입될 예정이었네";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"자네도 알고 있을거라고 믿네";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"군번줄에 기스도 안난 신병이 바로 조종법을 익히는게 무엇을 의미 하는지";
		break;	
	case 5:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"...";
		break;
	case 6:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"당연히 알고있습니다";
		break;
	case 7:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"죽음을 각오하게";
		break;
	case 8:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"작전 내용을 그쪽으로 전송하였네, 확인해 보게";
		break;
	case 9:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"-작전명 : 신세계 프로젝트 ";
		break;
	case 10:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"-작전 개요 : 적대 세력을 말살하고 안전지를 확보하라 ";
		break;
	case 11:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"-작전지 : 노르망디 대은하";
		break;
	case 12:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"-좌표 : ×××, ×××, ×××";
		break;
	case 13:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"항로 설정 완료, 명령만 내려주십시오.";
		break;
	case 14:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"좋아, 바로 출발 하도록";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();

	Portrait_Check();
}

void CScriptUI::Script_Stg2_Begin()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L".....";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"...!!";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"휘이이이이이이익!!";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::End;
		m_wstrScript = L"무전기에서 호루라기 소리가 들린다!";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"각 소대 그대로 들어!";
		break;
	case 5:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"20XX년 X월 X일 금일 저녁점호는 본 사령관이 직접 실시한다.";
		break;
	case 6:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"각 소대 보고하도록!";
		break;
	case 7:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"이병 잭 한마!";
		break;
	case 8:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"신 세계 프로젝트를 수행하기 위해 현재 노르망디 대은하로 순항 중에 있습니다!";
		break;
	case 9:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"-좌표 : @ @ @, $ $ $ , ×××";
		break;
	case 10:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"-기체 : 이상 무!";
		break;
	case 11:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"20XX년 X월 X일 금일 저녁점호보고 끝!";
		break;
	case 12:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"필승!";
		break;
	case 13:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L".....필승...";
		break;
	case 14:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"잭 한마 신병.";
		break;
	case 15:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"이병 잭 한마!";
		break;
	case 16:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"신세계 프로젝트는 우리군에 아주 중요한 인물을 구출해내 무사히 복귀시키고";
		break;
	case 17:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"적국과의 평화 회담을 진행해 이 전쟁을 끝내는 것이 최종목표이다.";
		break;
	case 18:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"아마 적국의 급진파 세력이 방해를 시도해올것이다.";
		break;
	case 19:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"만약  항해 중 적국의 병사를 만난다면....";
		break;
	case 20:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"모조리 말살하도록!!";
		break;
	case 21:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"예!알겠습니다!";
		break;
	case 22:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"그리고 또한가지,";
		break;
	case 23:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"현재 자네의 위치쪽으로 향하는 유성군이 확인되었다.";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
}

void CScriptUI::Script_Stg2_AfterCamProduction()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"모두 10m를 넘는 거대한 운석들이다.충돌한다면 추락을 피할 수는 없겠지...";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"자네가 훈련때 보여준 능숙한 솜씨면 무사히 넘어갈수 있을거라 생각한다.";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"무운을 빌지.";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::End;
		m_wstrScript = L"통신이 끊겼다....";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
}

void CScriptUI::Script_Stg2_Finish_AsteroidFlyAway()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"무사해서 다행이군.";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"훌륭한 실력이었다만.";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"속히 작전지로 이동하길 바란다.";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"....";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Admiral;

		m_wstrScript = L"쳇!!";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
}

void CScriptUI::Script_Stg2_Search_Target()
{

	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"목표가 근방이다만,";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"역시나 훼방을 놓으러 왔군!!";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"쓰레기들을 처리하고 속히 작전지로 이동해 목표를 구출하도록!!";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
}

void CScriptUI::Script_Stg2_PlayerDead()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"메이데이!!메이데이!!";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"기체가 손상되어 구조를 요청한다!!";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"끄아아아악!!!!";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::End;
		m_wstrScript = L".....";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
}

void CScriptUI::Script_Stg2_Clear()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"구조하러 왔습니다!! 괜찮으십니까?";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Politician;
		m_wstrScript = L"오오...주여..";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Politician;
		m_wstrScript = L"이대로 꼼짝없이 죽는줄 알았습니다...";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"이제 괜찮습니다!!어서 옮겨 타 본국으로 귀환하시지요!!";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Politician;
		m_wstrScript = L"네!!";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();
}

void CScriptUI::Script_Stage3_Opening()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"좋아 무사히 가이아 행성 구역에 도착했군, 앞으로 얼마 안남았네";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"여기가 정찰조들이 무전한 가이아 행성 구역입니까?";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"근데 어째 정찰조들은 보이지 않는 것 같습니다?";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"...";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"이런!";
		break;
	case 5:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"감청이다! 정찰조는 당했다고 밖에 생각이 들지 않는다!";
		break;
	case 6:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"적이 매복해 있을 것이다, 포위 당하기 전에 소탕한다.";
		break;
	case 7:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"2인 1조로 행동한다, 수송기체는 느리니 자네가 호위를 하는 식이 맞겠군.";
		break;
	case 8:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"감청의 시도가 있을 수 있으니 다른 무전 채널을 이용한다.";
		break;
	case 9:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"알겠습니다!";
		break;
	case 10:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"잘 부탁하네 신병.";
		break;
	case 11:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"맏겨만 주십시오!";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();

	Portrait_Check();
}

void CScriptUI::Script_Stage3_Boss_Opening()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"앗! 저건!";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"왜 그러십니까?";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"전방에 적군 기체로 판별되는 함선이 있다!";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"제길... 본부와 교신이 되지않아..!";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"별 다른 방법이 없습니다.";
		break;
	case 5:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"저는!! 임무를 끝까지 완수 하라는 명령을 받았기 때문에!!!";
		break;
	case 6:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"끝까지 호위를 하겠습니다.";
		break;
	case 7:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"자네는 겁도 없나?";
		break;
	case 8:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"이 임무를 완수하면 내가 맥주라도 쏘겠네, 약속하지";
		break;
	case 9:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"하하 각오하십시오.";
		break;
	case 10:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"자! 임무를 완수해라 잭 한마!!";
		break;
	case 11:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"맏겨만 주십시오!";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();

	Portrait_Check();
}

void CScriptUI::Script_Stage3_Boss_Clear()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"맥주 사주십시오.";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"기가 차는군 하하하!";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"우선 본부와 교신을 해보겠네";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"본부, 화물 운송을 완료 하였다.";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"대단해!! 아주 잘했네 제군들!!";
		break;
	case 5:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"잭 한마 이병의 호위가 없었더라면 저희는 죽었을 겁니다.";
		break;
	case 6:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"화물에 들어있는 탄약까지 전부 빼앗기고 말입니다...";
		break;
	case 7:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"임무를 무사히 완수한 잭 한마 이병에게 표창을 하셨으면 합니다.";
		break;
	case 8:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"잘 알았네. 잭 한마 이병! 임무를 완수해주어 고맙네!";
		break;
	case 9:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"과찬 이십니다.";
		break;
	case 10:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"그것보다 수송대장이 맥주 산다고 합니다.";
		break;
	case 11:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"고맙네 수송대장!";
		break;
	case 12:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"...아뿔싸...!!!";
		break;
	case 13:
		m_ePortrait = EPortraitNumber::Admiral;
		m_wstrScript = L"하하하, 곧 구조선이 도착할것이니 조금만 기다리게나";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();

	Portrait_Check();

}

void CScriptUI::Script_Stage3_Player_Dead()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"이봐! 괜찮나..?";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"들리면 대답해!";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"이봐! 야! 대답하라고!!!!";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"...";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"야!! 잭 한마!!!!!";
		break;
	case 5:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"으아아아아아아!!!!!!!!!!!!!!";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();

	Portrait_Check();
}

void CScriptUI::Script_Stage3_Delivery_Dead()
{
	switch (m_dwScriptNext)
	{
	case 0:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"어..?";
		break;
	case 1:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"수송대장님!!";
		break;
	case 2:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"시코르스키 수송대장님!! 대답해주십시오..!!!";
		break;
	case 3:
		m_ePortrait = EPortraitNumber::Delivery;
		m_wstrScript = L"...";
		break;
	case 4:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"아아..!";
		break;
	case 5:
		m_ePortrait = EPortraitNumber::Player;
		m_wstrScript = L"으아아아아아아!!!!!!!!!!!!!!";
		break;
	default:
		m_wstrName = L"";
		m_wstrScript = L"";
		m_eScriptFlow = EScriptFlow::BlackBar_End;
		break;
	}
	m_dwScriptCountMax = m_wstrScript.length();

	Portrait_Check();

}

void CScriptUI::Lock_Cursor()
{
	//RECT rc;

	//GetClientRect(g_hWnd, &rc);

	//rc.left = WINCX >> 1;
	//rc.top = WINCY >> 1;
	//rc.right = rc.left + 1;
	//rc.bottom = rc.top + 1;

	//ClipCursor(&rc);
	SetCursorPos(WINCX >> 1, (WINCY >> 1) - 5);
}

void CScriptUI::Portrait_Check()
{
	switch (m_ePortrait)
	{
	case EPortraitNumber::Admiral:
		m_wstrName = L"사령관 헥터 도일";
		break;
	case EPortraitNumber::Player:
		m_wstrName = L"잭 한마";
		break;
	case EPortraitNumber::Friendly:
		m_wstrName = L"마호메드 아라이 주니어";
		break;
	case EPortraitNumber::Delivery:
		m_wstrName = L"수송대장 시코르스키";
		break;
	case EPortraitNumber::Politician:
		m_wstrName = L"조난당한 정치가";
		break;
	default:
		m_wstrName = L"";
		break;
	}
}

void CScriptUI::BlackBar_Start(_float fDeltaTime)
{
	_float fSpeedPerSec = 3.f;
	_float3 vDir = { 0.f, 1.f, 0.f };

	m_vUI_BlackBar_Up_Pos -= vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Up->Set_Position(m_vUI_BlackBar_Up_Pos);
	m_vUI_BlackBar_Down_Pos += vDir * fSpeedPerSec;
	m_pTransfrom_BlackBar_Down->Set_Position(m_vUI_BlackBar_Down_Pos);

	if (m_vUI_BlackBar_Up_Pos.y <= 510.f)
	{
		if (!m_bSoundOnce)
		{
			m_pManagement->PlaySound(L"PopUp_Quest2.ogg", CSoundMgr::SCRIPT_POPUP);
			m_bSoundOnce = true;
		}
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

_bool CScriptUI::Get_IsScriptEnd()
{
	if(m_eScriptFlow == EScriptFlow::Flow_End)
		return true;

	return false;
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
