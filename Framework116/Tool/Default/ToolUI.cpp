#include "pch.h"
#include "ToolUI.h"
#include"UiTool.h"

CToolUI::CToolUI(LPDIRECT3DDEVICE9 pDevice)
	:CUI(pDevice)
{
}

CToolUI::CToolUI(const CUI & other)
	:CUI(other)
{
}

HRESULT CToolUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return S_OK;
}

HRESULT CToolUI::Ready_GameObject(void * pArg)
{
	CUI::Ready_GameObject(pArg);
	
	return S_OK;
}

_uint CToolUI::Update_GameObject(_float fDeltaTime)
{
	if (m_bDead)
		return DEAD_OBJECT;

	CUI::Update_GameObject(fDeltaTime);
	
	
	return _uint();
}

_uint CToolUI::LateUpdate_GameObject(_float fDeltaTime)
{

	CUI::LateUpdate_GameObject(fDeltaTime);
	if (m_pUiTool->m_pTargetUi != nullptr)
	{
		if ((m_pUiTool->m_pTargetUi) == this)
		{
			m_pUiTool->UpdateData(TRUE);
			m_pTransform->Set_Size(_float3(m_pUiTool->m_fScaleX, m_pUiTool->m_fScaleY, 0.f));
			m_pTransform->Set_Position(_float3(m_pUiTool->m_fPosX, m_pUiTool->m_fPosY, 0.f));
			m_pTransform->RotateZ(m_pUiTool->m_fRotX);
			m_pUiTool->UpdateData(FALSE);
		}
	}
	return _uint();
}

_uint CToolUI::Render_GameObject()
{
	//CUI::Render_GameObject();
	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	_float4x4 matView, matScale, matTrans, matRot;

	D3DXMatrixIdentity(&matView);

	D3DXMatrixScaling(&matScale, transformDesc.vScale.x, transformDesc.vScale.y, transformDesc.vScale.z);
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);

	/////////////////////////////////////////////////////////////////
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	/////////////////////////////////////////////////////////////////

	return _uint();
}

CToolUI* CToolUI::Create(LPDIRECT3DDEVICE9 pDevice,CUiTool* pUiTool)
{
	CToolUI* pInstance = new CToolUI(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}
	pInstance->m_pUiTool = pUiTool;
	return pInstance;
}

CGameObject* CToolUI::Clone(void* pArg)
{
	CToolUI* pClone = new CToolUI(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CToolUI::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
