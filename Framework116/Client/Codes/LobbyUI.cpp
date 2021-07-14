#include"stdafx.h"
#include "LobbyUI.h"
#include "Transform.h"
#include "Management.h"
#include"Loading.h"
#include"Lobby.h"
#include"GatchaBox.h"
#include"Product.h"
#include"LobbyCam.h"

USING(Engine)

CLobbyUI::CLobbyUI(LPDIRECT3DDEVICE9 pDevice)
	: CUI(pDevice)
{
}

CLobbyUI::CLobbyUI(const CLobbyUI& other)
	: CUI(other)
{
}

HRESULT CLobbyUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLobbyUI::Ready_GameObject(void* pArg)
{
	CUI::Ready_GameObject(pArg);

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Controller",
		L"Com_Controller",
		(CComponent**)&m_pController)))
	{

		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Controller");
		return E_FAIL;
	}

	 m_vNodeScale = { 50.f,50.f,0.f };
	 m_vFirstNode = { -379.f,124.f,0.f };
	 m_vCursorPos = m_vFirstNode;
	 m_vSecondNode = { -206.f,-123.f,0.f };
	 m_vThirdNode = { 167.f,30.f,0.f };

	
	return S_OK;
}

_uint CLobbyUI::Update_GameObject(_float fDeltaTime)
{
	if (m_bDead)
		return DEAD_OBJECT;
	if (m_wstrTexturePrototypeTag == L"Component_Texture_SceneSelect")
	{
		_float3 vPos = m_pTransform->Get_TransformDesc().vPosition;
		if (!m_bCancelSceneChange)
		{

			if (m_pTransform->Get_TransformDesc().vPosition.x > 0.f)
			{
				vPos.x -= fDeltaTime *1000.f;
			}
			else
			{
				vPos.y = 0.f;
				vPos.x = 0.f;
			}

			m_pTransform->Set_Position(vPos);
		}
		else
		{
			if (m_pTransform->Get_TransformDesc().vPosition.x < 1756.f)
			{
				vPos.x += fDeltaTime *1000.f;
			}
			else
			{
				m_bDead = TRUE;
				m_pLobby->Set_SceneSelect(FALSE);
			}
			m_pTransform->Set_Position(vPos);
		}
	}
	else if (m_wstrTexturePrototypeTag != L"Component_Texture_X"
		&& m_wstrTexturePrototypeTag != L"Component_Texture_achievement")
	{
		if (m_pLobby->Get_SceneSelect())
			return NO_EVENT;
		if (m_bGotoNextScene || m_bStartUnPacking)
			return NO_EVENT;
	}
	else if (m_pLobby->Get_SceneSelect())
	{
		if (m_wstrTexturePrototypeTag != L"Component_Texture_SceneSelect"
			&&m_wstrTexturePrototypeTag != L"Component_Texture_X"
			&& m_wstrTexturePrototypeTag != L"Component_Texture_achievement")
		{
			return NO_EVENT;
		}
	}
	CUI::Update_GameObject(fDeltaTime);
	Update_ItemFrame(fDeltaTime);
	if (m_pLobby->Get_StartUnPacking())
	{
		if (m_wstrTexturePrototypeTag == L"Component_Texture_X")
			Update_Bounds();
	}
	else
		Update_Bounds();
	m_pController->Update_Controller();
	Update_SceneSelect(fDeltaTime);
	Check_Picking();
	OnMouseButton();
	Key_Check(fDeltaTime);
	return NO_EVENT;
}

_uint CLobbyUI::LateUpdate_GameObject(_float fDeltaTime)
{
	if (m_wstrTexturePrototypeTag != L"Component_Texture_X")
	{
		if (m_bGotoNextScene || m_bStartUnPacking)
			return NO_EVENT;
	}
	else if (!m_pBox->Get_StartUnpacking())
	{
		if(!m_pLobby->Get_GotoNextScene())
			m_bDead = TRUE;
	}
	
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (m_bCancel)
	{
		if(m_pBox)
		m_pBox->Set_CancelUnPacking(TRUE);
		if(!m_pProduct)
		m_pProduct = (CProduct*)m_pManagement->Get_GameObject(L"Layer_Product");
		if (m_pProduct)
		{
			m_pProduct->Set_Cancel(TRUE);
			m_pProduct->Set_ShowProduct(TRUE);
		}
	}

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Background, this)))
		return UPDATE_ERROR;


	return _uint();
}

_uint CLobbyUI::Render_GameObject()
{
	if (!m_pLobby->Get_EnterScene())
		return 0;
	CLobbyCam* pCam = (CLobbyCam*)m_pManagement->Get_GameObject(L"Layer_Cam");
	if (m_pLobby->Get_IsGatcha())
	{
		if (m_wstrTexturePrototypeTag == L"Component_Texture_Full_Frame")
			return 0;
		if (m_wstrTexturePrototypeTag == L"Component_Texture_UNDER")
			return 0;
		if (m_wstrTexturePrototypeTag == L"Component_Texture_Holo_Board")
			return 0;
		if (m_wstrTexturePrototypeTag == L"Component_Texture_Stat_Frame")
			return 0;
		if (m_wstrTexturePrototypeTag == L"Component_Texture_Stat_Back")
			return 0;

	}
	else if (!pCam->Get_Stop())
	{
		if (m_wstrTexturePrototypeTag == L"Component_Texture_Full_Frame")
			return 0;
		if (m_wstrTexturePrototypeTag == L"Component_Texture_UNDER")
			return 0;
		if (m_wstrTexturePrototypeTag == L"Component_Texture_Holo_Board")
			return 0;
	}

	/*if (m_bClicked && !m_pLobby->Get_SceneSelect())
	{

		m_fClicked += m_pManagement->Get_DeltaTime();
		if (m_fClicked >= 0.1f)
			m_bClicked = false;
		if(m_wstrTexturePrototypeTag != L"Component_Texture_SceneSelect")
			return 0;
	}*/
	if (!Render_ItemSlot())
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
	////////////////////////////////////////
	/////////////////////////
	m_pTexture->Set_Texture(m_dwIdx);
	m_pVIBuffer->Render_VIBuffer();
	/////////////////////////////////////////////////////////////////
	}

	if (m_wstrTexturePrototypeTag == L"Component_Texture_SceneSelect")
	{
		_float4x4 matView;
		D3DXMatrixIdentity(&matView);
		TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();
		matView._11 = transformDesc.vScale.x + 100.f;
		matView._22 = transformDesc.vScale.y + 100.f;
		matView._41 = transformDesc.vPosition.x;
		matView._42 = transformDesc.vPosition.y;
		m_pDevice->SetTransform(D3DTS_VIEW, &matView);
		m_pTexture->Set_Texture(5);
		m_pVIBuffer->Render_VIBuffer();
		
		if (m_pTransform->Get_TransformDesc().vPosition.x <= 0.f)
		{
			Render_Cursor();
			Render_Nodes();
		}

	}
	return _uint();
}

void CLobbyUI::Update_Bounds()
{
	if (m_pTransform == nullptr)
		return;
	
	_float3 vPos = _float3(0.f,0.f,0.f);
	_float3 vDecartPos = m_pTransform->Get_TransformDesc().vPosition;
	_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
	vPos.x = vDecartPos.x + _float(WINCX/2.f);
	vPos.y = _float(WINCY/2.f)- vDecartPos.y;
	vPos.y -= 30.f;
	_float3 vSize = m_pTransform->Get_TransformDesc().vScale;


	m_tUIBounds.left = (LONG)(vPos.x - (vSize.x/2.f));
	m_tUIBounds.top = (LONG)(vPos.y - (vSize.y / 2.f));
	m_tUIBounds.right = (LONG)(vPos.x + (vSize.x / 2.f));
	m_tUIBounds.bottom = (LONG)(vPos.y + (vSize.y / 2.f));


}

void CLobbyUI::Check_Picking()
{
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	
	if (PtInRect(&m_tUIBounds, pt))
			Set_IsPicking(TRUE);
	else
			Set_IsPicking(FALSE);
}

void CLobbyUI::Key_Check(_float fDeltaTime)
{
	if(m_pController->Key_Down(KEY_LBUTTON))
	{
		if (Get_IsPicking())
		{
			m_bClicked = TRUE;
			if (m_wstrTexturePrototypeTag == L"Component_Texture_RepairIcon")
			{
			}
			else if (m_wstrTexturePrototypeTag == L"Component_Texture_ShopIcon")
			{
				if(!m_pLobby->Get_IsGatcha())
					m_pLobby->Set_IsGatcha(TRUE);
				else if (!m_pLobby->Get_IsSetPlayerModel())
					m_pLobby->Set_IsSetPlayerModel(TRUE);
			}
			else if (m_wstrTexturePrototypeTag == L"Component_Texture_PlaneTemplete")
			{
				if (!m_pLobby->Get_IsSetPlayerModel())
					m_pLobby->Set_IsSetPlayerModel(TRUE);
			}
			else if (m_wstrTexturePrototypeTag == L"Component_Texture_achievement")
			{
				for (auto& pDst : *m_pManagement->Get_GameObjectList(L"Layer_UI"))
				{
					if (static_cast<CLobbyUI*>(pDst)->m_wstrTexturePrototypeTag == L"Component_Texture_SceneSelect")
					{
						_bool bSet = static_cast<CLobbyUI*>(pDst)->Get_CancelSceneSelect();
						static_cast<CLobbyUI*>(pDst)->Set_CancelSceneSelect(!bSet);
						return;
					}
				}
				if (m_pLobby->Get_SceneSelect())
					return;
				if (m_pLobby->Get_IsGatcha())
					return;
				UI_DESC UiDesc;
				_float PosX = 1171.f;
				_float PosY = 0.f;
				_float ScaleX = 1171;
				_float ScaleY = 683;

				UiDesc.tTransformDesc.vPosition = { PosX,PosY,0 };
				UiDesc.tTransformDesc.vScale = { ScaleX,ScaleY,0.f };
				UiDesc.wstrTexturePrototypeTag = L"Component_Texture_SceneSelect";
				if (FAILED(CManagement::Get_Instance()->Add_GameObject_InLayer(
					EResourceType::NonStatic, L"GameObject_LobbyUI"
					, L"Layer_UI", &UiDesc)))
				{
					PRINT_LOG(L"Error", L"Add_GameObject_InLayerTool_Failed");
					return;
				}
				for (auto& pUI : *m_pManagement->Get_GameObjectList(L"Layer_UI"))
				{
					if (!static_cast<CLobbyUI*>(pUI)->Get_Scene())
					{
						static_cast<CLobbyUI*>(pUI)->Set_Scene(m_pLobby);
					}
				}
				m_pLobby->Set_SceneSelect(TRUE);
			}
			else if (m_wstrTexturePrototypeTag == L"Component_Texture_X")
			{
				if (m_pLobby->Get_StartUnPacking())
				{
					m_bCancel = TRUE;
				}
				if (m_pLobby->Get_SceneSelect())
					m_pLobby->Set_SceneChange(TRUE);

			}
			
		}
	}
}



void CLobbyUI::OnMouseButton()
{
	if (Get_IsPicking())
	{
		if (m_wstrTexturePrototypeTag == L"Component_Texture_RepairIcon")
		{
			m_dwIdx = 1;
		}
		else if (m_wstrTexturePrototypeTag == L"Component_Texture_ShopIcon")
		{
			m_dwIdx = 1;
		}
		else if (m_wstrTexturePrototypeTag == L"Component_Texture_achievement")
		{
			m_dwIdx = 1;
		}
	}
	else if (m_wstrTexturePrototypeTag == L"Component_Texture_Product")
		m_dwIdx = 4;
	else if(m_wstrTexturePrototypeTag != L"Component_Texture_PlaneTemplete")
		m_dwIdx = 0;
}



void CLobbyUI::Render_Cursor()
{
	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = m_vNodeScale.x;
	matView._22 = m_vNodeScale.y;
	matView._41 = m_vCursorPos.x;
	matView._42 = m_vCursorPos.y;
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
	/////////////////////////////////////////////////////////////////
	m_pTexture->Set_Texture(1);
	m_pVIBuffer->Render_VIBuffer();
}

void CLobbyUI::Render_Nodes()
{
	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	_float3 vPos;
	for (int i = 0; i < 3; i++)
	{
		if (i == 0)
			vPos = m_vFirstNode;
		else if (i == 1)
			vPos = m_vSecondNode;
		else if (i == 2)
			vPos = m_vThirdNode;
		_bool bIsLock = m_pLobby->GetStageLock(i);

		matView._11 = m_vNodeScale.x;
		matView._22 = m_vNodeScale.y;
		matView._41 = vPos.x;
		matView._42 = vPos.y;
		m_pDevice->SetTransform(D3DTS_VIEW, &matView);
		/////////////////////////////////////////////////////////////////
		m_pTexture->Set_Texture(i+2);
		m_pVIBuffer->Render_VIBuffer();
		if (bIsLock)
		{
			m_pTexture->Set_Texture(6);
			m_pVIBuffer->Render_VIBuffer();
		}
	}
}

void CLobbyUI::Update_SceneSelect(_float fDeltaTime)
{
	if (m_wstrTexturePrototypeTag != L"Component_Texture_SceneSelect")
		return;

	if (m_pController->Key_Down(KEY_LEFT))
	{
		if (m_iSelect)
			m_iSelect--;
		else
			m_iSelect = 2;
	}
	else if (m_pController->Key_Down(KEY_RIGHT))
			m_iSelect++;
	else if (m_pController->Key_Down(KEY_ENTER))
	{
		if (!m_pLobby->GetStageLock(m_iSelect))
		{
			m_pLobby->SetNextScene(m_iSelect);
			m_pLobby->Set_GotoNextScene(TRUE);
		}
	}
	else if (m_pController->Key_Down(KEY_P))
	{
		m_pLobby->Set_SceneChange(FALSE);
		m_bCancelSceneChange = TRUE;
		return;
	}
	if (m_iSelect > 2)
			m_iSelect = 0;
	
	switch (m_iSelect)
	{
	case 0:
		m_vCursorPos = m_vFirstNode;
		m_vCursorPos.y += m_vNodeScale.y;
		break;
	case 1:
		m_vCursorPos = m_vSecondNode;
		m_vCursorPos.y += m_vNodeScale.y;
		break;
	case 2:
		m_vCursorPos = m_vThirdNode;
		m_vCursorPos.y += m_vNodeScale.y;
		break;
	}
}

_bool CLobbyUI::Render_ItemSlot()
{
	CLobbyCam* pCam = (CLobbyCam*)m_pManagement->Get_GameObject(L"Layer_Cam");

	if (m_pLobby->Get_IsGatcha() || m_pLobby->Get_SceneSelect()
		|| !pCam->Get_Stop()|| m_wstrTexturePrototypeTag != L"Component_Texture_PlaneTemplete")
	{
		if (m_pFontAtkUpCount)
		{
			m_pFontAtkUpCount->Set_IsDead(TRUE);
			Safe_Release(m_pFontAtkUpCount);
		}
		if (m_pFontDefUpCount)
		{
			m_pFontDefUpCount->Set_IsDead(TRUE);
			Safe_Release(m_pFontDefUpCount);
		}
		if (m_pFontHpUpCount)
		{
			m_pFontHpUpCount->Set_IsDead(TRUE);
			Safe_Release(m_pFontHpUpCount);
		}
		if (m_pFontEnergyUpCount)
		{
			m_pFontEnergyUpCount->Set_IsDead(TRUE);
			Safe_Release(m_pFontEnergyUpCount);
		}
		if (m_pFontMissileCount)
		{
			m_pFontMissileCount->Set_IsDead(TRUE);
			Safe_Release(m_pFontMissileCount);
		}
		if (m_pFontVMaxCount)
		{
			m_pFontVMaxCount->Set_IsDead(TRUE);
			Safe_Release(m_pFontVMaxCount);
		}
		return FALSE;
	}

	CGameObject::Render_GameObject();

	_float4x4 matView;
	D3DXMatrixIdentity(&matView);

	_float3 vFirstPos = m_pTransform->Get_TransformDesc().vPosition;
	_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
	_float3 vPos = { 0,0,0 };
	TCHAR szBuf[32] = L"";
	
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int iIndex = i * 3 + j;
			if (iIndex > 6)
				continue;
			vPos.x = vFirstPos.x + j * 160;
			vPos.y = vFirstPos.y - i * 160;

			_float3 vDecartFontPos = vPos;
			_float3 vFontPos = {0,0,0};
			vFontPos.x = vDecartFontPos.x + _float(WINCX / 2.f);
			vFontPos.y = _float(WINCY / 2.f) - vDecartFontPos.y;
			vFontPos.x += 40.f;
			vFontPos.y -= 60.f;
			switch (iIndex)
			{
			case 1:
				_itow_s(m_pLobby->GetAtkBuffItemCount(), szBuf, 10);
				Add_Font_InLayer(L"Layer_Font_ItemCount", m_pFontAtkUpCount
				, szBuf, vFontPos, { 2.f,2.f,2.f }
				, D3DXCOLOR{ 255,255,255,255 });
				break;
			case 2:
				_itow_s(m_pLobby->GetDefBuffItemCount(), szBuf, 10);
				Add_Font_InLayer(L"Layer_Font_ItemCount", m_pFontDefUpCount
				, szBuf, vFontPos, { 2.f,2.f,2.f }
				, D3DXCOLOR{ 255,255,255,255 });
				break;
			case 3:
				_itow_s(m_pLobby->GetHpBuffItemCount(), szBuf, 10);
				Add_Font_InLayer(L"Layer_Font_ItemCount", m_pFontHpUpCount
				, szBuf, vFontPos, { 2.f,2.f,2.f }
				, D3DXCOLOR{ 255,255,255,255 });
				break;
			case 4:	
				_itow_s(m_pLobby->GetEnergyBuffItemCount(), szBuf, 10);
				Add_Font_InLayer(L"Layer_Font_ItemCount", m_pFontEnergyUpCount
				, szBuf, vFontPos, { 2.f,2.f,2.f }
				, D3DXCOLOR{ 255,255,255,255 });
				break;
			case 5:
				_itow_s(m_pLobby->GetMissileCount(), szBuf, 10);
				Add_Font_InLayer(L"Layer_Font_ItemCount", m_pFontMissileCount
				, szBuf, vFontPos, { 2.f,2.f,2.f }
				, D3DXCOLOR{ 255,255,255,255 });		
				break;
			case 6:
				_itow_s(m_pLobby->GetVMaxBuffItem(), szBuf, 10);
				Add_Font_InLayer(L"Layer_Font_ItemCount", m_pFontVMaxCount
				, szBuf, vFontPos, { 2.f,2.f,2.f }
				, D3DXCOLOR{ 255,255,255,255 });
				break;
				}
			matView._11 = vScale.x;
			matView._22 = vScale.y;
			matView._41 = vPos.x;
			matView._42 = vPos.y;
			m_pDevice->SetTransform(D3DTS_VIEW, &matView);
			/////////////////////////////////////////////////////////////////
			m_pTexture->Set_Texture(iIndex);
			m_pVIBuffer->Render_VIBuffer();
		}
	}
	return TRUE;
}

void CLobbyUI::Add_Font_InLayer(wstring strLayerTag, CGameObject*& pFont, wstring str, _float3 vPos, _float3 vScale, D3DXCOLOR tColor)
{
	if (pFont)
		return;

	MYFONT_DESC tFontDesc;

	tFontDesc.wstrText =str;
	tFontDesc.tTextColor = tColor;

	tFontDesc.tTransformDesc.vPosition = vPos;
	tFontDesc.tTransformDesc.vScale = vScale;

	if (FAILED(m_pManagement->Add_GameObject_InLayer(
		EResourceType::Static,
		L"GameObject_MyFont",
		strLayerTag,
		(void*)&tFontDesc,
		(CGameObject**)&pFont)))
	{
		PRINT_LOG(L"Error", L"Failed To Add StatusBoard In Layer");
		return;
	}
}


void CLobbyUI::Update_ItemFrame(_float fDeltaTime)
{
	if (m_wstrTexturePrototypeTag != L"Component_Texture_Item_Frame")
		return;
	if (!m_bStart)
	{
		m_vStartPos = m_pTransform->Get_TransformDesc().vPosition;
		m_bStart = true;
	}
	//모으기
	if (m_pLobby->Get_IsGatcha() || m_pLobby->Get_SceneSelect())
	{
		_float3 vTargetPos = { -810,190,0 };
		_float3 vPos = m_pTransform->Get_TransformDesc().vPosition;
		if (vTargetPos.x < vPos.x)
			vPos.x -= fDeltaTime*100.f;
		if (vTargetPos.y > vPos.y)
			vPos.y += fDeltaTime*100.f;
		m_pTransform->Set_Position(vPos);
	}//헤치기
	else if (m_pLobby->Get_IsSetPlayerModel())
	{
		_float3 vPos = m_pTransform->Get_TransformDesc().vPosition;
		if (m_vStartPos.x > vPos.x)
			vPos.x += fDeltaTime*100.f;
		if (m_vStartPos.y < vPos.y)
			vPos.y -= fDeltaTime*100.f;
		m_pTransform->Set_Position(vPos);
	}
}

CLobbyUI* CLobbyUI::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLobbyUI* pInstance = new CLobbyUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLobbyUI::Clone(void* pArg)
{
	CLobbyUI* pClone = new CLobbyUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLobbyUI::Free()
{
	
	Safe_Release(m_pController);

	Safe_Release(m_pFontAtkUpCount);
	Safe_Release(m_pFontDefUpCount);
	Safe_Release(m_pFontHpUpCount);
	Safe_Release(m_pFontEnergyUpCount);
	Safe_Release(m_pFontMissileCount);
	Safe_Release(m_pFontVMaxCount);

	CUI::Free();
}

void CLobbyUI::Set_Scene(CLobby * _pUI)
{
	m_pLobby = _pUI;


}




