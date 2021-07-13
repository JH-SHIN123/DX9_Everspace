#include "stdafx.h"
#include "..\Headers\AlertArrow.h"
#include "Collision.h"
#include "Pipeline.h"

CAlertArrow::CAlertArrow(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CAlertArrow::CAlertArrow(const CAlertArrow & other)
	: CGameObject(other)
{
}

HRESULT CAlertArrow::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CAlertArrow::Ready_GameObject(void * pArg/* = nullptr*/)
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
		EResourceType::Static,
		L"Component_Texture_AlertArrow",
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
		(CComponent**)&m_pTransform)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	m_pTargetMonster = (CGameObject*)pArg;
	Safe_AddRef(m_pTargetMonster);
	if (nullptr == m_pTargetMonster)
	{
		PRINT_LOG(L"Error", L"m_pTargetMonster is nullptr");
	}

	m_pPlayerTransform = (CTransform*)m_pManagement->Get_Component(L"Layer_Player", L"Com_Transform");
	Safe_AddRef(m_pPlayerTransform);
	if (nullptr == m_pPlayerTransform)
	{
		PRINT_LOG(L"Error", L"m_pPlayerTransform is nullptr");
	}

	return S_OK;
}

_uint CAlertArrow::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);	
	
	Movement(fDeltaTime);	

	if (m_IsDead == true)
		return DEAD_OBJECT;

	return m_pTransform->Update_Transform();
}

_uint CAlertArrow::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);
	
	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::UI, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CAlertArrow::Render_GameObject()
{
	CGameObject::Render_GameObject();

	_float3 vTargetPos = m_pTargetMonster->Get_Collides()->front()->Get_BoundingSphere().Get_Position();
	_float3 vPlayerPos = m_pPlayerTransform->Get_State(EState::Position);
	_float3 vPlayerLook = m_pPlayerTransform->Get_State(EState::Look);
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);


	////////////타겟의 3d좌표를 2d좌표로/////////////////////
	D3DVIEWPORT9 vp;
	m_pDevice->GetViewport(&vp);
	_float4x4 TestView, TestProj;
	m_pDevice->GetTransform(D3DTS_VIEW, &TestView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj);
	_float4x4 matCombine = TestView * TestProj;
	D3DXVec3TransformCoord(&vTargetPos, &vTargetPos, &matCombine);
	vTargetPos.x += 1.f;
	vTargetPos.y += 1.f;

	vTargetPos.x = (vp.Width * (vTargetPos.x)) / 2.f + vp.X;
	vTargetPos.y = (vp.Height * (2.f - vTargetPos.y) / 2.f + vp.Y);

	_float3 ptTarget;
	ptTarget.x = -1.f * (WINCX / 2) + vTargetPos.x;
	ptTarget.y = 1.f * (WINCY / 2) + vTargetPos.y;
	ptTarget.z = 0.f;
	//////////////////플레이어의 3d좌표를 2d좌표로////////////////////////////
	D3DVIEWPORT9 vp2;
	m_pDevice->GetViewport(&vp2);
	_float4x4 TestView2, TestProj2;
	m_pDevice->GetTransform(D3DTS_VIEW, &TestView2);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj2);
	_float4x4 matCombine2 = TestView2 * TestProj2;
	D3DXVec3TransformCoord(&vPlayerPos, &vPlayerPos, &matCombine2);
	vPlayerPos.x += 1.f;
	vPlayerPos.y += 1.f;

	vPlayerPos.x = (vp2.Width * (vPlayerPos.x)) / 2.f + vp2.X;
	vPlayerPos.y = (vp2.Height * (2.f - vPlayerPos.y) / 2.f + vp2.Y);

	_float3 ptPlayer;
	ptPlayer.x = -1.f * (WINCX / 2) + vPlayerPos.x;
	ptPlayer.y = 1.f * (WINCY / 2) + vPlayerPos.y;
	ptPlayer.z = 0.f;
	//////////////////////////////////////////////////////////////////

	//////////////////플레이어Look의 3d좌표를 2d좌표로////////////////////////////
	//D3DVIEWPORT9 vp3;
	//m_pDevice->GetViewport(&vp3);
	//_float4x4 TestView3, TestProj3;
	//m_pDevice->GetTransform(D3DTS_VIEW, &TestView3);
	//m_pDevice->GetTransform(D3DTS_PROJECTION, &TestProj3);
	//_float4x4 matCombine3 = TestView3 * TestProj3;
	//D3DXVec3TransformCoord(&vPlayerLook, &vPlayerLook, &matCombine2);
	//vPlayerLook.x += 1.f;
	//vPlayerLook.y += 1.f;

	//vPlayerLook.x = (vp3.Width * (vPlayerLook.x)) / 2.f + vp3.X;
	//vPlayerLook.y = (vp3.Height * (2.f - vPlayerLook.y) / 2.f + vp3.Y);

	//_float3 ptPlayerLook;
	//ptPlayerLook.x = vPlayerLook.x;
	//ptPlayerLook.y = vPlayerLook.y;
	//ptPlayerLook.z = 0.f;
	////////////////////////////////////////////////////////////////////////////

	_float3 vDir = ptTarget - ptPlayer;
	_float3 vLook = {1.f, 0.f, 0.f};

	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&vLook, &vLook);

	_float v1v2 = D3DXVec3Dot(&vDir, &vLook);
	_float fCeta = acosf(v1v2);

	if (vDir.x > 0 && vDir.y < 0)
	{
		fCeta += D3DXToRadian(90.f);
	}
	if (vDir.y > 0)
		fCeta = -1.f * fCeta/* + D3DXToRadian(180.f)*/;
	//////////////////////////////////////////////////////////

	_float4x4 matView;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matView);

	
	matView._11 = 50.f;
	matView._22 = 50.f;

	if (vDir.x > 0)
	{
		matView._41 = /*-(WINCX / 2.f) + */vDir.x * (WINCX / 4);
		matView._42 = /*-(WINCY / 2.f) + */vDir.y * (WINCY / 4);
	}
	else
	{
		matView._41 = /*-(WINCX / 2.f) + */-vDir.x * (WINCX / 4);
		matView._42 = /*-(WINCY / 2.f) + */-vDir.y * (WINCY / 4);
	}
	int i = 0;

	//if (vDir.x > 0)
	//{
	//	matView._41 = (1.f * (WINCX / 2) + cosf(D3DXToDegree(fCeta)) * 1.f) / 2.f;
	//	matView._42 = (1.f * (WINCY / 2) - sinf(D3DXToDegree(fCeta)) * 1.f) / 2.f;
	//}
	//else
	//{
	//	matView._41 = (1.f * (WINCX / 2) + cosf(D3DXToDegree(fCeta)) * 1.f) / -2.f ;
	//	matView._42 = (1.f * (WINCY / 2) - sinf(D3DXToDegree(fCeta)) * 1.f) / -2.f ;
	//}
	//_float fX += cosf(m_fAngle * PI / 180.f) * m_fSpeed;
	//_float fY -= sinf(m_fAngle * PI / 180.f) * m_fSpeed;
	// 데카르트 좌표계.. - WINCX /2, -WINCY / 2
	// 오른쪽, 왼쪽 구별해야댐.


	_float4x4 matRotate;
	D3DXMatrixRotationZ(&matRotate, (fCeta));
	//
	_float4x4 Test;
	Test = matView * matRotate;
	
	//_float testdegree = D3DXToDegree(fCeta);
	//wstring a = to_wstring(testdegree);
	//PRINT_LOG(L"a", a.c_str());

	m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetTransform(D3DTS_VIEW, &Test);
	m_pTexture->Set_Texture(0);
	m_pVIBuffer->Render_VIBuffer();
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	//////////////////////////////////////////////////////

	return _uint();
}

_uint CAlertArrow::Movement(_float fDeltaTime)
{
	// 타겟 몬스터의 포스 받아옴.
	// 이제 화살표가 타겟 몬스터의 포스쪽을 가리켜야 하는디..
	// 월드상에서 화살표의 Look벡터가 몬스터 포스를 가르킨다 하믄..
	// 직교투영 했을때.. 
	// 
	_float3 vTargetPos = m_pTargetMonster->Get_Collides()->front()->Get_BoundingSphere().Get_Position();
	if (nullptr == vTargetPos)
	{
		PRINT_LOG(L"Error", L"vTargetPos is nullptr");
		return _uint();
	}
	



	return _uint();
}

CAlertArrow * CAlertArrow::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CAlertArrow* pInstance = new CAlertArrow(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create AlertArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAlertArrow::Clone(void * pArg/* = nullptr*/)
{
	CAlertArrow* pClone = new CAlertArrow(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone AlertArrow");
		Safe_Release(pClone);
	}

	return pClone;
}

void CAlertArrow::Free()
{
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pTargetMonster);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pTexture);

	CGameObject::Free();
}
