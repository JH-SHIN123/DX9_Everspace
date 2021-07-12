#include "stdafx.h"
#include "..\Headers\Skybox.h"
#include "Camera.h"
#include "MaterialHandler.h"

CSkybox::CSkybox(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	ZeroMemory(&m_tMaterial, sizeof(D3DMATERIAL9));

	CMaterialHandler::Set_RGBA(1.f, 1.f, 1.f, 1.f, &m_tMaterial);

	m_tMaterial.Power = 1.f;
}

CSkybox::CSkybox(const CSkybox & other)
	: CGameObject(other)
	, m_tMaterial(other.m_tMaterial)
	, vColorRGBA(other.vColorRGBA)
{
}

HRESULT CSkybox::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CSkybox::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_VIBuffer_CubeTexture",
		L"Com_VIBuffer",
		(CComponent**)&m_pVIBuffer)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_VIBuffer");
		return E_FAIL;
	}
	wstring strComponentTexturePrototype = L"Component_Texture_Skybox";
	if (auto ptr = (wstring*)pArg)
	{
		if (dynamic_cast<wstring*>(ptr))
		{
			strComponentTexturePrototype = *dynamic_cast<wstring*>(ptr);
		}
	}
	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		strComponentTexturePrototype,
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Component_Texture_Skybox");
		return E_FAIL;
	}

	// 스테이지3 텍스처
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Skybox_Stage3",
		L"Com_Texture",
		(CComponent**)&m_pStage3Texture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Component_Texture_Skybox_Stage3");
		return E_FAIL;
	}


	// For.Com_Transform
	TRANSFORM_DESC TransformDesc;
	TransformDesc.vScale = _float3(50.f, 50.f, 50.f);

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

	m_pCamera = (CCamera*)m_pManagement->Get_GameObject(L"Layer_Cam");
	Safe_AddRef(m_pCamera);
	if (nullptr == m_pCamera)
	{
		PRINT_LOG(L"Error", L"m_pCamera is nullptr");
		return E_FAIL;
	}

	return S_OK;
}

_uint CSkybox::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CSkybox::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Priority, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CSkybox::Render_GameObject()
{
	CGameObject::Render_GameObject();

	/* 3D 렌더링 디폴트 
	m_pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);			// z read 관련
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);	// z write 관련
	*/

	/* 2D 렌더링 디폴트
	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	*/

	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	_uint iStage = m_pManagement->Get_Current_Scene_Type();
	switch(iStage)
	{
	case (_uint)ESceneType::Stage:
		m_pTexture->Set_Texture(0);
		break;
	case (_uint)ESceneType::Stage3:
		m_pStage3Texture->Set_Texture();
		break;
	}
	
	m_pDevice->SetMaterial(&m_tMaterial);
	m_pVIBuffer->Render_VIBuffer();

	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return _uint();
}

_uint CSkybox::Movement(_float fDeltaTime)
{
	m_pTransform->Set_Position(m_pCamera->Get_CameraDesc().vEye);

	return _uint();
}

CSkybox * CSkybox::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CSkybox* pInstance = new CSkybox(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}



CGameObject * CSkybox::Clone(void * pArg/* = nullptr*/)
{
	CSkybox* pClone = new CSkybox(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CSkybox::Free()
{
	Safe_Release(m_pCamera);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pStage3Texture);

	CGameObject::Free();
}
