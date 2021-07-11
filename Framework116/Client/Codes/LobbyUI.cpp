#include"stdafx.h"
#include "LobbyUI.h"
#include "Transform.h"
#include "Management.h"
#include"Loading.h"
#include"Lobby.h"
#include"GatchaBox.h"
#include"Product.h"

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
	 m_vCursorPos = { -400.f,-100.f,0.f };
	 m_vFirstNode = { -400.f,-100.f,0.f };
	 m_vSecondNode = { 100.f,100.f,0.f };
	 m_vThirdNode = { 300.f,200.f,0.f };

	
	return S_OK;
}

_uint CLobbyUI::Update_GameObject(_float fDeltaTime)
{
	if (m_bDead)
		return DEAD_OBJECT;
	if (m_wstrTexturePrototypeTag == L"Component_Texture_SceneSelect")
	{
		_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
		if (m_pTransform->Get_TransformDesc().vScale.x < 1171.f)
		{
			vScale.x += fDeltaTime * 1171.f;
		}
		if (m_pTransform->Get_TransformDesc().vScale.y < 683.f)
		{
			vScale.y += fDeltaTime* 683.f;
		}
		m_pTransform->Set_Scale(vScale);
	}
	else if (m_wstrTexturePrototypeTag != L"Component_Texture_X")
	{
		if (m_pLobby->Get_SceneSelect())
			return NO_EVENT;
		if (m_bGotoNextScene || m_bStartUnPacking)
			return NO_EVENT;
	}
	else if (m_pLobby->Get_SceneSelect())
	{
		if (m_wstrTexturePrototypeTag != L"Component_Texture_SceneSelect"
			&&m_wstrTexturePrototypeTag != L"Component_Texture_X")
			return NO_EVENT;
	}
	if (m_bShowModelIcon)
		m_bRenderItemMean = TRUE;
	else
		m_bRenderItemMean = FALSE;
	CUI::Update_GameObject(fDeltaTime);
	Update_Bounds();
	m_pController->Update_Controller();
	Update_SceneSelect(fDeltaTime);
	Check_Picking();
	OnMouseButton();
	Key_Check(fDeltaTime);
	ChangeModelIcon();
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
	if(!m_pLobby->Get_IsSetPlayerModel())
	{
		m_bShowModelIcon = false;
	}

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
	//if (m_bClicked && !m_pLobby->Get_SceneSelect())
	//{
	//	m_fClicked += m_pManagement->Get_DeltaTime();
	//	if (m_fClicked >= 0.1f)
	//		m_bClicked = false;
	//	if(m_wstrTexturePrototypeTag != L"Component_Texture_SceneSelect")
	//		return 0;
	//}
	//if (m_wstrTexturePrototypeTag != L"Component_Texture_X")
	//{
	//	if (m_bGotoNextScene || m_bStartUnPacking)
	//		return 0;
	//}
	
	CGameObject::Render_GameObject();
	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = transformDesc.vScale.x;
	matView._22 = transformDesc.vScale.y;
	matView._41 = transformDesc.vPosition.x;
	matView._42 = transformDesc.vPosition.y;
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
	/////////////////////////////////////////////////////////////////
	m_pTexture->Set_Texture(m_dwIdx);
	m_pVIBuffer->Render_VIBuffer();
	/////////////////////////////////////////////////////////////////



	if (m_wstrTexturePrototypeTag == L"Component_Texture_SceneSelect")
	{
		_float4x4 matView;
		D3DXMatrixIdentity(&matView);
		matView._11 = transformDesc.vScale.x + 100.f;
		matView._22 = transformDesc.vScale.y + 100.f;
		matView._41 = transformDesc.vPosition.x;
		matView._42 = transformDesc.vPosition.y;
		m_pDevice->SetTransform(D3DTS_VIEW, &matView);
		m_pTexture->Set_Texture(5);
		m_pVIBuffer->Render_VIBuffer();
		
		_float3 vScale = { 1000.f,600.f,0.f };
		if (m_pTransform->Get_TransformDesc().vScale.x >= vScale.x
			&&m_pTransform->Get_TransformDesc().vScale.y >= vScale
			.y)
		{
			Render_Cursor();
			Render_Nodes();
		}

	}

	if (m_bShowModelIcon)
	{
		if(m_fDeltaTime < 1.f)
		m_fDeltaTime += m_pManagement->Get_DeltaTime();
		int iIndex = m_dwIdx;
		for (int i = 0; i <5; i++)
		{	
			if (iIndex >= 5)
				iIndex = 0;
			matView._11 = transformDesc.vScale.x;
			matView._22 = transformDesc.vScale.y;
			matView._41 = transformDesc.vPosition.x +i*transformDesc.vScale.x;
			matView._42 = transformDesc.vPosition.y;
			m_pDevice->SetTransform(D3DTS_VIEW, &matView);
			m_pTexture->Set_Texture(iIndex);
			m_pVIBuffer->Render_VIBuffer();
			iIndex++;
		}
	

		if (m_bRenderItemMean)
			Render_ItemMean();
		
	}

	if (Get_IsPicking())
		Set_Text();
	else if (m_wstrTexturePrototypeTag == L"Component_Texture_Product")
		Set_Text();


	return _uint();
}

void CLobbyUI::Update_Bounds()
{
	if (m_pTransform == nullptr)
		return;
	
	_float3 vPos = _float3(0.f,0.f,0.f);
	_float3 vDecartPos = m_pTransform->Get_TransformDesc().vPosition;

	vPos.x = vDecartPos.x + _float(WINCX/2.f);
	vPos.y = _float(WINCY/2.f)- vDecartPos.y;
	vPos.y -= 50.f;
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
		if (m_bShowModelIcon)
		{
			m_bChangeModelIcon = TRUE;
		}
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
				if (!m_bShowModelIcon)
				{
					m_bShowModelIcon = TRUE;
				}
				
			}
			else if (m_wstrTexturePrototypeTag == L"Component_Texture_achievement")
			{

				if (m_pLobby->Get_SceneSelect())
					return;
				UI_DESC UiDesc;
				_float PosX = 0.f;
				_float PosY = 0.f;
				_float ScaleX = 0;
				_float ScaleY = 0;


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

void CLobbyUI::Set_Text()
{
#ifdef _DEBUG
	wstring str;
	RECT rc;
	GetClientRect(g_hWnd, &rc);

	if (L"Component_Texture_achievement" == m_wstrTexturePrototypeTag)
	{
		str = L"스테이지 선택!";
		rc.left = (WINCX>>1) + 500;
		rc.top = (WINCY >> 1) + 200;
		m_pManagement->Get_Font()->DrawText(NULL
		, str.c_str(), -1
		, &rc, DT_LEFT | DT_TOP, D3DXCOLOR(255, 0, 0, 255));
	}
	if (L"Component_Texture_Product" == m_wstrTexturePrototypeTag)
	{
		//TCHAR szMoney[MAX_PATH];
		//_itow_s(m_pLobby->Get_Money(),szMoney,10);
		//rc.left = (WINCX >> 1) - 470;
		//rc.top = (WINCY >> 1) - 465;
		//m_pManagement->Get_Font()->DrawText(NULL
		//	, szMoney, -1
		//	, &rc, DT_LEFT | DT_TOP, D3DXCOLOR(255, 255, 255, 255));
	}
	if (L"Component_Texture_PlaneTemplete" == m_wstrTexturePrototypeTag)
	{
		str = L"아이템 슬롯";
		rc.left = (WINCX >> 1) + 500;
		rc.top = (WINCY >> 1) + 200;
		m_pManagement->Get_Font()->DrawText(NULL
			, str.c_str(), -1
			, &rc, DT_LEFT | DT_TOP, D3DXCOLOR(255, 0, 0, 255));
	}
	if (L"Component_Texture_ShopIcon" == m_wstrTexturePrototypeTag)
	{
		str = L"상자 개봉";
		rc.left = (WINCX >> 1) + 500;
		rc.top = (WINCY >> 1) + 200;
		m_pManagement->Get_Font()->DrawText(NULL
			, str.c_str(), -1
			, &rc, DT_LEFT | DT_TOP, D3DXCOLOR(255, 0, 0, 255));
	}
	if (L"Component_Texture_RepairIcon" == m_wstrTexturePrototypeTag)
	{
		str = L"설정";
		rc.left = (WINCX >> 1) + 500;
		rc.top = (WINCY >> 1) + 200;
		m_pManagement->Get_Font()->DrawText(NULL
			, str.c_str(), -1
			, &rc, DT_LEFT | DT_TOP, D3DXCOLOR(255, 0, 0, 255));
	}
#elif _DEBUG
	 
#endif
}

void CLobbyUI::OnMouseButton()
{
	if (Get_IsPicking())
	{
		if (m_wstrTexturePrototypeTag == L"Component_Texture_RepairIcon")
		{
		}
		else if (m_wstrTexturePrototypeTag == L"Component_Texture_ShopIcon")
		{
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

void CLobbyUI::ChangeModelIcon()
{
	if (!m_bChangeModelIcon)
		return;
	RECT rc[5] = {};
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	for (int i = 0; i < 5; i++)
	{
	_float3 vPos = _float3(0.f, 0.f, 0.f);
	_float3 vDecartPos = m_pTransform->Get_TransformDesc().vPosition;

	vPos.x = vDecartPos.x + _float(WINCX / 2.f);
	vPos.y = _float(WINCY / 2.f) - vDecartPos.y;
	vPos.y -= 50.f;
	_float3 vSize = m_pTransform->Get_TransformDesc().vScale;


	rc[i].left = (LONG)(vPos.x - (vSize.x *0.5f)+(vSize.x*(i)));
	rc[i].top = (LONG)(vPos.y - (vSize.y*0.5f));
	rc[i].right = (LONG)(vPos.x + (vSize.x*0.5f)+(vSize.x*(i)));
	rc[i].bottom = (LONG)(vPos.y + (vSize.y*0.5f));


		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			if (PtInRect(&rc[i], pt))
			{
				m_bClicked = TRUE;
				m_dwIdx += i;
				if (m_dwIdx >= 5)
					m_dwIdx -= 5;
			
				m_bShowModelIcon = FALSE;
				m_bChangeModelIcon = FALSE;
				break;
			}
		}
	}
}


void CLobbyUI::Render_ItemMean()
{

	if (!m_bRenderItemMean)
		return;
	TCHAR str[5][32];
	swprintf_s(str[0], L"");
	swprintf_s(str[1], L"Atk Buff : %d", m_pLobby->GetAtkBuffItemCount());
	swprintf_s(str[2], L"Def Buff : %d", m_pLobby->GetDefBuffItemCount());
	swprintf_s(str[3], L"Hp Buff : %d", m_pLobby->GetHpBuffItemCount());
	swprintf_s(str[4], L"Energy Buff : %d", m_pLobby->GetEnergyBuffItemCount());
	RECT rc;
	GetClientRect(g_hWnd, &rc);

	_float3 vDecartPos = m_pTransform->Get_TransformDesc().vPosition;
	_float3 vPos = { 0,0,0 };
	_float3 vScale = m_pTransform->Get_TransformDesc().vScale;
	vScale.y *= 0.5f;
	_uint iFirstIndex = m_dwIdx;
	for (int i = 0; i < 5; i++)
	{
		if (iFirstIndex >= 5)
			iFirstIndex -= 5;
		vPos.x = vDecartPos.x + _float(WINCX / 2.f);
		vPos.y = _float(WINCY / 2.f) - vDecartPos.y;
		rc.left = (LONG)(vPos.x + i*vScale.x);
		rc.top = (LONG)(vPos.y - vScale.y);
		m_pManagement->Get_Font()->DrawText(NULL
			, str[iFirstIndex], -1
			, &rc, DT_LEFT | DT_TOP, D3DXCOLOR(255, 0, 0, 255));
		iFirstIndex++;
	}
	



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

		matView._11 = m_vNodeScale.x;
		matView._22 = m_vNodeScale.y;
		matView._41 = vPos.x;
		matView._42 = vPos.y;
		m_pDevice->SetTransform(D3DTS_VIEW, &matView);
		/////////////////////////////////////////////////////////////////
		m_pTexture->Set_Texture(i+2);
		m_pVIBuffer->Render_VIBuffer();
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
			m_pLobby->Set_GotoNextScene(TRUE);
	
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

	CUI::Free();
}

void CLobbyUI::Set_Scene(CLobby * _pUI)
{
	m_pLobby = _pUI;
}




