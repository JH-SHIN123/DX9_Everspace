#include "stdafx.h"
#include "..\Headers\Crosshair.h"
#include "Pipeline.h"
#include "Collision.h"
#include "Player.h"

CCrosshair::CCrosshair(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CCrosshair::CCrosshair(const CCrosshair & other)
	: CGameObject(other)
{
}

HRESULT CCrosshair::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CCrosshair::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

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
		L"Component_Texture_Crosshair",
		L"Com_Texture",
		(CComponent**)&m_pTexture)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture");
		return E_FAIL;
	}

	// For.Com_Texture (개틀링건)
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Crosshair_Gatling",
		L"Com_Texture2",
		(CComponent**)&m_pTexture_Gatling)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture2");
		return E_FAIL;
	}

	// For.Com_Texture (미사일)
	if (FAILED(CGameObject::Add_Component(
		EResourceType::NonStatic,
		L"Component_Texture_Crosshair_Missile",
		L"Com_Texture3",
		(CComponent**)&m_pTexture_Missile)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Texture3");
		return E_FAIL;
	}

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}


	return S_OK;
}

_uint CCrosshair::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	Movement(fDeltaTime);	
	Searching_Target(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CCrosshair::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::AlphaUI, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CCrosshair::Render_GameObject()
{
	CGameObject::Render_GameObject();	
	TRANSFORM_DESC transformDesc = m_pTransform->Get_TransformDesc();

	POINT	pt = {};
	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);
	ShowCursor(FALSE);

	_float4x4 matView;
	D3DXMatrixIdentity(&matView);
	matView._11 = 60.f;
	matView._22 = 60.f;
	matView._41 = (_float)pt.x - (WINCX >> 1);
	matView._42 = (_float)pt.y * -1.f + (WINCY >> 1) - 15.f;

	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_VIEW, &matView);
	

	_int iWeapon = ((CPlayer*)m_pManagement->Get_GameObject(L"Layer_Player"))->Get_Weapon_Type();
	switch (iWeapon)
	{
	case WEAPON_LAZER:
		m_pTexture->Set_Texture(0);
		break;
	case WEAPON_MACHINEGUN:
		m_pTexture_Gatling->Set_Texture(0);
		break;
	case WEAPON_MISSILE:
		m_pTexture_Missile->Set_Texture(0);
		break;
	default:
		break;
	}
	m_pVIBuffer->Render_VIBuffer();	
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return _uint();
}

_uint CCrosshair::Movement(_float fDeltaTime)
{
	return _uint();
}

CCrosshair * CCrosshair::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CCrosshair* pInstance = new CCrosshair(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCrosshair::Clone(void * pArg/* = nullptr*/)
{
	CCrosshair* pClone = new CCrosshair(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Crosshair");
		Safe_Release(pClone);
	}

	return pClone;
}

void CCrosshair::Free()
{
	Safe_Release(m_pBossMonsterCollide);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTexture_Gatling);
	Safe_Release(m_pTexture_Missile);

	CGameObject::Free();
}

_uint CCrosshair::Searching_Target(_float fDeltaTime)
{
	if (m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster") && m_pManagement->Get_GameObjectList(L"Layer_Boss_Monster")->size() != 0)
	{
		RAY ray;
		CPipeline::CreatePickingRay(ray, g_hWnd, WINCX, WINCY, m_pDevice);
		_float3 m_vLockOn;
		m_vLockOn = ray.vDirection;

		_float4x4 matView;
		m_pDevice->GetTransform(D3DTS_VIEW, &matView);
		D3DXMatrixInverse(&matView, 0, &matView);
		D3DXVec3TransformCoord(&ray.vPos, &ray.vPos, &matView);
		D3DXVec3TransformNormal(&ray.vDirection, &ray.vDirection, &matView);
		D3DXVec3Normalize(&ray.vDirection, &ray.vDirection);

		m_vLockOn = ray.vDirection;
		D3DXVec3Normalize(&m_vLockOn, &m_vLockOn);

		// True면? - LockOn HUD 생성
		if (m_pBossMonsterCollide && CCollision::IntersectRayToSphere(ray, m_pBossMonsterCollide->Get_BoundingSphere()))
		{
			if (!m_IsBossLockOn)
			{
				wstring TargetLayerTag = L"Layer_Boss_Monster";

				// LockOn 생성.
				if (FAILED(m_pManagement->Add_GameObject_InLayer(
					EResourceType::NonStatic,
					L"GameObject_LockOn",
					L"Layer_LockOn", &TargetLayerTag)))
				{
					PRINT_LOG(L"Error", L"Failed To Add LockOn In Layer");
					return E_FAIL;
				}
				m_IsBossLockOn = true;
				m_pManagement->PlaySound(L"Lock_On.ogg", CSoundMgr::LOCKON);

			}
			else if (!m_IsDistOn)
			{
				// 거리 나타내주는 HUD생성.

				m_IsDistOn = true;
			}
		}
	}
	return _uint();
}
