#include "stdafx.h"
#include "..\Headers\LobbyModel.h"
#include "Collision.h"
#include"Player.h"
#include"Lobby.h"
#include"LobbyUI.h"
#include"GatchaBox.h"
#include"EngineEffectSystem.h"
#include"WingBoost_System.h"

CLobbyModel::CLobbyModel(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CLobbyModel::CLobbyModel(const CLobbyModel & other)
	: CGameObject(other)
{
}

HRESULT CLobbyModel::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLobbyModel::Ready_GameObject(void * pArg/* = nullptr*/)
{
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Mesh_BigShip",
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
		return E_FAIL;
	}

	// For.Com_Transform Test
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 45.f;
	TransformDesc.vPosition = _float3(20.f, 0.f, 20.f);
	TransformDesc.fSpeedPerSec = 25.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vScale = { 1.f,1.f,1.f };

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		&TransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	// For.Com_Controller
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Controller",
		L"Com_Controller",
		(CComponent**)&m_pController)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Controller");
		return E_FAIL;
	}
	
	return S_OK;
}

_uint CLobbyModel::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);
	//if (!m_bPlayStgClearProduction)
	//{
	//	if (!m_bStartStgClearProduction)
	//	{
	//		CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pLeftEngineEffect);
	//		m_vLeftEngineOffset = { -1.4f, 0.9f, -6.7f };
	//		CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pRightEngineEffect);
	//		m_vRightEngineOffset = { 1.4f, 0.9f, -6.7f };

	//		// Add Wing-Boost Effect

	//		CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pLeftWingBoost);
	//		m_vLeftWingOffset = { -8.2f, -1.5f, -2.f };
	//		CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pRightWingBoost);
	//		m_vRightWingOffset = { 8.2f, -1.5f, -2.f };
	//		m_IsBoost = TRUE;
	//	}
	//	Update_Effect();
	//	PlayStgClearProduction(fDeltaTime);
	//}
	if (!m_bGotoNextScene)
	{
		KeyProcess(fDeltaTime);
		Movement(fDeltaTime);
		m_pTransform->Update_Transform();
	}
	else
	{
		if (!m_bSetCreateCancelButton)
		{
			// Add Engine-Boost Effect
			CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pLeftEngineEffect);
			m_vLeftEngineOffset = { -1.4f, 0.9f, -6.7f };
			CEffectHandler::Add_Layer_Effect_EngineBoost((CGameObject**)&m_pRightEngineEffect);
			m_vRightEngineOffset = { 1.4f, 0.9f, -6.7f };

			// Add Wing-Boost Effect

			CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pLeftWingBoost);
			m_vLeftWingOffset = { -8.2f, -1.5f, -2.f };
			CEffectHandler::Add_Layer_Effect_WingBoost((CGameObject**)&m_pRightWingBoost);
			m_vRightWingOffset = { 8.2f, -1.5f, -2.f };

			m_IsBoost = TRUE;
			m_bSetCreateCancelButton = TRUE;
			Add_Layer_CancelButton();
		}
		Update_Effect();
		StartSceneChange(fDeltaTime);
	}
	return m_pTransform->Update_Transform();;
}

_uint CLobbyModel::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;
	if (m_bGo_Straight)
	{
		m_fDelaySceneChange += fDeltaTime;
		if (m_fDelaySceneChange >= 4.f)
			m_pLobby->Set_SceneChange(TRUE);
	}
	return _uint();
}

_uint CLobbyModel::Render_GameObject()
{
	if (m_pLobby->Get_SceneSelect() && !m_pLobby->Get_GotoNextScene())
		return 0;
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();

#ifdef _DEBUG // Render Collide
	/*m_pCollide->Render_Collide();*/
#endif

	return _uint();
}

void CLobbyModel::KeyProcess(_float fDeltaTime)
{
	if (nullptr == m_pController) return;
	m_pController->Update_Controller();
}

_uint CLobbyModel::Movement(_float fDeltaTime)
{

	m_pTransform->RotateY(-D3DXToRadian(45.f)*fDeltaTime);
	return _uint();
}

void CLobbyModel::StartSceneChange(_float fDeltaTime)
{
	//
	static _float fTime = 0;
	fTime += fDeltaTime;
	if (m_bGo_Straight)
	{
		m_pTransform->Go_Straight(fDeltaTime* 10.f);
		return;
	}
	if (fTime < 3.f)
	{
		m_pTransform->Go_Up(0.01f*fDeltaTime);
	}
	else
	{
		_float3 vPlayerPos = m_pTransform->Get_State(EState::Position);
		_float3 vTargetDir = _float3( -1.f,0.f,1.f );
		_float3 vDir = m_pTransform->Get_State(EState::Look);
		vDir.y = 0.f;
		D3DXVec3Normalize(&vDir, &vDir);
		D3DXVec3Normalize(&vTargetDir,&vTargetDir);
		_float fAngle = acosf(D3DXVec3Dot(&vTargetDir, &vDir));
		m_pTransform->RotateY(fDeltaTime*D3DXToRadian(fAngle)*20.f);
		
		if (0.1>= fAngle)
		{
			m_fSoundTiming += fDeltaTime;
			m_pManagement->PlaySound(L"Jump_Gate.ogg", CSoundMgr::LOBBY_EFFECT);

			vTargetDir = _float3(0.f, 1.f, 1.f);
			vDir = m_pTransform->Get_State(EState::Look);
			vDir.x = 0.f;
			D3DXVec3Normalize(&vTargetDir, &vTargetDir);
			
			fAngle = D3DXVec3Dot(&vDir, &vTargetDir);
			m_pTransform->RotateX(fDeltaTime*fAngle);
		
			if (0.1 >= fAngle&&m_fSoundTiming >= 7.f)
			{
				m_bGo_Straight = TRUE;
			}
		}
	}
}

void CLobbyModel::Add_Layer_CancelButton()
{
	UI_DESC UiDesc;
	_float PosX = 880.f;
	_float PosY = 470.f;
	_float ScaleX = 80.f;
	_float ScaleY = 70.f;
	UiDesc.tTransformDesc.vPosition = { PosX,PosY,0 };
	UiDesc.tTransformDesc.vScale = { ScaleX,ScaleY,0.f };
	UiDesc.wstrTexturePrototypeTag = L"Component_Texture_X";
	if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
		EResourceType::NonStatic, L"GameObject_LobbyUI"
		, L"Layer_UI", &UiDesc)))
	{
		PRINT_LOG(L"Error", L"Add_GameObject_InLayerTool_Failed");
		return;
	}
	_uint iCount = m_pManagement->Get_GameObjectList(L"Layer_UI")->size()-1;
	CLobbyUI* pUi = (CLobbyUI*)m_pManagement->Get_GameObject(L"Layer_UI",iCount);
	pUi->Set_Model(this);
	pUi->Set_Scene(m_pLobby);
	CGatchaBox* pBox = (CGatchaBox*)(m_pManagement->Get_GameObject(L"Layer_GatchaBox"));
	pUi->Set_GatchaBox(pBox);
	
}


void CLobbyModel::Update_Effect()
{
	// Engine-Boost Effect
	if (m_pLeftEngineEffect) {
		_float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
		vEnginePos += m_pTransform->Get_State(EState::Right) * m_vLeftEngineOffset.x;
		vEnginePos += m_pTransform->Get_State(EState::Up) * m_vLeftEngineOffset.y;
		vEnginePos += m_pTransform->Get_State(EState::Look) * m_vLeftEngineOffset.z;
		m_pLeftEngineEffect->Set_EngineOffset(vEnginePos);
		m_pLeftEngineEffect->Set_IsBoost(m_IsBoost);
	}
	if (m_pRightEngineEffect) {
		_float3 vEnginePos = m_pTransform->Get_TransformDesc().vPosition;
		vEnginePos += m_pTransform->Get_State(EState::Right) * m_vRightEngineOffset.x;
		vEnginePos += m_pTransform->Get_State(EState::Up) * m_vRightEngineOffset.y;
		vEnginePos += m_pTransform->Get_State(EState::Look) * m_vRightEngineOffset.z;
		m_pRightEngineEffect->Set_EngineOffset(vEnginePos);
		m_pRightEngineEffect->Set_IsBoost(m_IsBoost);
	}

	// Wing-Boost Effect
	
	if (m_pLeftWingBoost)
	{
		_float3 vWingPos = m_pTransform->Get_TransformDesc().vPosition;
		vWingPos += m_pTransform->Get_State(EState::Right) * m_vLeftWingOffset.x;
		vWingPos += m_pTransform->Get_State(EState::Up) * m_vLeftWingOffset.y;
		vWingPos += m_pTransform->Get_State(EState::Look) * m_vLeftWingOffset.z;
		m_pLeftWingBoost->Set_WingOffset(vWingPos);
		m_pLeftWingBoost->Set_IsBoost(m_IsBoost);
	}
	if (m_pRightWingBoost) 
	{
		_float3 vWingPos = m_pTransform->Get_TransformDesc().vPosition;
		vWingPos += m_pTransform->Get_State(EState::Right) * m_vRightWingOffset.x;
		vWingPos += m_pTransform->Get_State(EState::Up) * m_vRightWingOffset.y;
		vWingPos += m_pTransform->Get_State(EState::Look) * m_vRightWingOffset.z;
		m_pRightWingBoost->Set_WingOffset(vWingPos);
		m_pRightWingBoost->Set_IsBoost(m_IsBoost);
	}
	
}

void CLobbyModel::PlayStgClearProduction(_float fDeltaTime)
{
	if (m_bPlayStgClearProduction)
		return;
	_float3 vGoalPos = {20.f,0.f,20.f};
	if (!m_pLobby->GetIsStgClear())
	{
		m_bPlayStgClearProduction = TRUE;
		m_pTransform->Set_Position(vGoalPos);
		return;
	}
	if (!m_bStartStgClearProduction)
	{
		m_pTransform->Set_Position(_float3(-100.f, 0.f, 0.f));
		m_bStartStgClearProduction = TRUE;
	}
	m_fPlayProductionTime += fDeltaTime;
	if (m_fPlayProductionTime >= 5.f)
		m_pTransform->Go_Dir(_float3(0, 0, 1), fDeltaTime*10.f);


	if (m_pTransform->Get_TransformDesc().vPosition == vGoalPos)
	{
		m_bPlayStgClearProduction = TRUE;
	}
}


CLobbyModel * CLobbyModel::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLobbyModel* pInstance = new CLobbyModel(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLobbyModel::Clone(void * pArg/* = nullptr*/)
{
	CLobbyModel* pClone = new CLobbyModel(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLobbyModel::Free()
{

	if (m_pLeftEngineEffect) {
		m_pLeftEngineEffect->Set_IsDead(true);
		m_pLeftEngineEffect = nullptr;
	}
	if (m_pRightEngineEffect) {
		m_pRightEngineEffect->Set_IsDead(true);
		m_pRightEngineEffect = nullptr;
	}

	if (m_pLeftWingBoost) {
		m_pLeftWingBoost->Set_IsDead(true);
		m_pLeftWingBoost = nullptr;
	}
	if (m_pRightWingBoost) {
		m_pRightWingBoost->Set_IsDead(true);
		m_pLeftWingBoost = nullptr;
	}

	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pController);

	CGameObject::Free();
}
