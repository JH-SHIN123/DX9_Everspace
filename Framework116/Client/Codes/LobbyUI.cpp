#include"stdafx.h"
#include "LobbyUI.h"
#include "Transform.h"
#include "Management.h"
#include"Loading.h"
#include"Lobby.h"
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
	
	return S_OK;
}

_uint CLobbyUI::Update_GameObject(_float fDeltaTime)
{
	if (m_bDead)
		return DEAD_OBJECT;
	if (m_bGotoNextScene)
		return NO_EVENT;
	CUI::Update_GameObject(fDeltaTime);
	Update_Bounds();
	Check_Picking();
	OnMouseButton();
	Key_Check(fDeltaTime);
	ChangeModelIcon();
	return NO_EVENT;
}

_uint CLobbyUI::LateUpdate_GameObject(_float fDeltaTime)
{

	if (m_bGotoNextScene)
		return NO_EVENT;
	

	CGameObject::LateUpdate_GameObject(fDeltaTime);
	if(!m_pLobby->Get_IsSetPlayerModel())
	{
		m_bShowModelIcon = false;
	}
	
	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
		return UPDATE_ERROR;
	


	return _uint();
}

_uint CLobbyUI::Render_GameObject()
{
	if (m_bGotoNextScene)
		return 0;
	
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
	if (m_bShowModelIcon)
	{
		if(m_fDeltaTime < 1.f)
		m_fDeltaTime += m_pManagement->Get_DeltaTime();
		int iIndex = m_dwIdx;
		for (int i = 0; i <4; i++)
		{	
			if (iIndex >= 4)
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
	}


	if (Get_IsPicking())
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
	
	_float3 vSize = m_pTransform->Get_TransformDesc().vScale;

	m_tUIBounds.left = vPos.x - (vSize.x);
	m_tUIBounds.top = vPos.y - (vSize.y);
	m_tUIBounds.right = vPos.x + (vSize.x);
	m_tUIBounds.bottom = vPos.y + (vSize.y);

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
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		m_fDelayCheck += fDeltaTime;
		if (m_bShowModelIcon && m_fDelayCheck >= 0.1f)
		{
			m_fDelayCheck = 0.f;
			m_bChangeModelIcon = TRUE;
		}
		if (Get_IsPicking())
		{
			if (m_wstrTexturePrototypeTag == L"Component_Texture_RepairIcon")
			{
			}
			else if (m_wstrTexturePrototypeTag == L"Component_Texture_ShopIcon")
			{
				m_pLobby->Set_IsGatcha(TRUE);
			}
			else if (m_wstrTexturePrototypeTag == L"Component_Texture_PlaneTemplete")
			{
				if (!m_pLobby->Get_IsSetPlayerModel())
					m_pLobby->Set_IsSetPlayerModel(TRUE);
				if (m_fDelayCheck >= 0.1f && !m_bShowModelIcon)
				{
					m_bShowModelIcon = TRUE;
					m_fDelayCheck = 0.f;
				}
				
			}
			else if (m_wstrTexturePrototypeTag == L"Component_Texture_achievement")
			{
				m_pLobby->Set_GotoNextScene(TRUE);
			}
		
		}
	}

	
}

void CLobbyUI::Set_Text()
{
	wstring str;
	RECT rc;
	GetClientRect(g_hWnd, &rc);

	if (L"Component_Texture_achievement" == m_wstrTexturePrototypeTag)
	{
		str = L"스테이지 진입!";
		rc.left = (WINCX>>1) + 500;
		rc.top = (WINCY >> 1) + 200;
		m_pManagement->Get_Font()->DrawText(NULL
		, str.c_str(), -1
		, &rc, DT_LEFT | DT_TOP, D3DXCOLOR(255, 0, 0, 255));
	}
	if (L"Component_Texture_PlaneTemplete" == m_wstrTexturePrototypeTag)
	{
		str = L"플레이어 선택";
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
	else if(m_wstrTexturePrototypeTag != L"Component_Texture_PlaneTemplete")
		m_dwIdx = 0;
}

void CLobbyUI::ChangeModelIcon()
{
	if (!m_bChangeModelIcon)
		return;
	RECT rc[4] = {};
	POINT pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	for (int i = 0; i < 4; i++)
	{
	_float3 vPos = _float3(0.f, 0.f, 0.f);
	_float3 vDecartPos = m_pTransform->Get_TransformDesc().vPosition;

	vPos.x = vDecartPos.x + _float(WINCX / 2.f);
	vPos.y = _float(WINCY / 2.f) - vDecartPos.y;

	_float3 vSize = m_pTransform->Get_TransformDesc().vScale;

	rc[i].left = vPos.x - (vSize.x)+(vSize.x*(i));
	rc[i].top = vPos.y - (vSize.y);
	rc[i].right = vPos.x + (vSize.x)+(vSize.x*(i));
	rc[i].bottom = vPos.y + (vSize.y);

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			if (PtInRect(&rc[i], pt))
			{
				m_dwIdx += i;
				if (m_dwIdx >= 4)
					m_dwIdx -= 4;
			
				m_bShowModelIcon = FALSE;
				m_bChangeModelIcon = FALSE;
				break;
			}
		}
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
	Safe_Release(m_pLobby);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}

void CLobbyUI::Set_Scene(CLobby * _pUI)
{
	m_pLobby = _pUI;
	m_pLobby->AddRef();
}




