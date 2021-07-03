#include "..\Headers\Renderer.h"
#include "GameObject.h"
#include "Management.h"
#include "Camera.h"
#include "Pipeline.h"

USING(Engine)
IMPLEMENT_SINGLETON(CRenderer)

HRESULT CRenderer::Add_GameObject(ERenderType eType, CGameObject * pObject)
{
	_uint iRenderIndex = _uint(eType);

	if (ERenderType::End <= eType)
	{
		PRINT_LOG(L"Error", L"Out of range Renderer");
		return E_FAIL;
	}

	if (nullptr == pObject)
	{
		PRINT_LOG(L"Error", L"pObject is nullptr");
		return E_FAIL;
	}

	auto iter_find = find(m_GameObjects[iRenderIndex].begin(), m_GameObjects[iRenderIndex].end(), pObject);
	if (m_GameObjects[iRenderIndex].end() == iter_find)
	{
		m_GameObjects[iRenderIndex].push_back(pObject);
		Safe_AddRef(pObject);
	}

	return S_OK;
}

_uint CRenderer::Render_GameObject()
{
	_uint iEvent = NO_EVENT;	

	if (iEvent = Render_Priority())
		return iEvent;

	if (iEvent = Render_NonAlpha())
		return iEvent;

	if (iEvent = Render_Alpha())
		return iEvent;

	if (iEvent = Render_Particle())
		return iEvent;

	if (iEvent = Render_AlphaUI())
		return iEvent;

	if (iEvent = Render_UI())
		return iEvent;

	return iEvent;
}

_uint CRenderer::Render_Priority()
{
	_uint iRenderIndex = (_uint)ERenderType::Priority;
	_uint iEvent = NO_EVENT;

	for (auto& pObject : m_GameObjects[iRenderIndex])
	{
		iEvent = pObject->Render_GameObject();
		Safe_Release(pObject);

		if (iEvent)
			return iEvent;
	}

	m_GameObjects[iRenderIndex].clear();

	return iEvent;
}

_uint CRenderer::Render_NonAlpha()
{
	_uint iRenderIndex = (_uint)ERenderType::NonAlpha;
	_uint iEvent = NO_EVENT;

	for (auto& pObject : m_GameObjects[iRenderIndex])
	{
		iEvent = pObject->Render_GameObject();
		Safe_Release(pObject);

		if (iEvent)
			return iEvent;
	}

	m_GameObjects[iRenderIndex].clear();

	return iEvent;
}

_uint CRenderer::Render_Alpha()
{
	LPDIRECT3DDEVICE9 pDevice = CManagement::Get_Instance()->Get_Device();
	if (nullptr == pDevice)
		return RENDER_ERROR;

	_uint iRenderIndex = (_uint)ERenderType::Alpha;
	_uint iEvent = NO_EVENT;	

	///////////////// 알파 테스팅 ///////////////////////////////////////////////
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 1); /* 알파 기준 값 설정 */
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); /* 위에서 설정한 기준값보다 작은 것들 */

	for (auto& pObject : m_GameObjects[iRenderIndex])
	{
		iEvent = pObject->Render_GameObject();
		Safe_Release(pObject);

		if (iEvent)
			return iEvent;
	}

	m_GameObjects[iRenderIndex].clear();

	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	///////////////// 알파 블렌딩 ///////////////////////////////////////////////
	//pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	///*
	//result pixel = dest pixel * blend factor + src pixel * blend factor
	//*/
	//pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	///*
	//D3DRS_DESTBLEND: dest pixel의 (A, R, G, B).
	//D3DBLEND_INVSRCALPHA(blend factor): D3DBLEND_SRCALPHA의 역. (1-As, 1-As, 1-As, 1-As).
	//*/
	//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	///*
	//D3DRS_SRCBLEND: src pixel의 (A, R, G, B).
	//D3DBLEND_SRCALPHA(blend factor): src pixel의 A값을 0 ~ 1로 환산한 비율 값. (As, As, As, As).
	//*/
	//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	//for (auto& pObject : m_GameObjects[iRenderIndex])
	//{
	//	iEvent = pObject->Render_GameObject();
	//	Safe_Release(pObject);

	//	if (iEvent)
	//		return iEvent;
	//}

	//m_GameObjects[iRenderIndex].clear();

	//pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return iEvent;
}

_uint CRenderer::Render_Particle()
{
	LPDIRECT3DDEVICE9 pDevice = CManagement::Get_Instance()->Get_Device();
	if (nullptr == pDevice)
		return RENDER_ERROR;

	_uint iRenderIndex = (_uint)ERenderType::Particle;
	_uint iEvent = NO_EVENT;

	// 조명 Off
	pDevice->SetRenderState(D3DRS_LIGHTING, false);
	pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true);

	// Set PointSize Min
	pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, CPipeline::FtoDw(0.0f));

	// control the size of the particle relative to distance
	pDevice->SetRenderState(D3DRS_POINTSCALE_A, CPipeline::FtoDw(0.0f));
	pDevice->SetRenderState(D3DRS_POINTSCALE_B, CPipeline::FtoDw(0.0f));
	pDevice->SetRenderState(D3DRS_POINTSCALE_C, CPipeline::FtoDw(1.0f));

	// use alpha from texture
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// 알파테스팅 + 알파블랜딩
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 1); /* 알파 기준 값 설정 */
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); /* 위에서 설정한 기준값보다 작은 것들 */

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	for (auto& pObject : m_GameObjects[iRenderIndex])
	{
		iEvent = pObject->Render_GameObject();
		Safe_Release(pObject);

		if (iEvent)
			return iEvent;
	}

	m_GameObjects[iRenderIndex].clear();

	pDevice->SetRenderState(D3DRS_LIGHTING, true);
	pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, false);
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	return iEvent;
}

_uint CRenderer::Render_UI()
{
	LPDIRECT3DDEVICE9 pDevice = CManagement::Get_Instance()->Get_Device();
	if (nullptr == pDevice)
		return RENDER_ERROR;

	_uint iRenderIndex = (_uint)ERenderType::UI;
	_uint iEvent = NO_EVENT;

	// UI 조명 off
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	///////////////// 알파 테스팅 ///////////////////////////////////////////////
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 1); /* 알파 기준 값 설정 */
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); /* 위에서 설정한 기준값보다 작은 것들 */

	//pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// UI Render 후 기존으로 세팅하기 위한 변수들
	_float4x4 matPrevView;
	pDevice->GetTransform(D3DTS_VIEW, &matPrevView);
	_float4x4 matPrevProj;
	pDevice->GetTransform(D3DTS_PROJECTION, &matPrevProj);

	// World 항등행렬 세팅
	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	pDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// 직교투영행렬 세팅
	_float2 winSize = CManagement::Get_Instance()->Get_WindowSize();
	_float4x4 matOrtho;
	D3DXMatrixOrthoLH(&matOrtho, winSize.x, winSize.y, 0.f, 1.f);
	pDevice->SetTransform(D3DTS_PROJECTION, &matOrtho);

	for (auto& pObject : m_GameObjects[iRenderIndex])
	{
		iEvent = pObject->Render_GameObject();
		Safe_Release(pObject);

		if (iEvent)
			return iEvent;
	}

	m_GameObjects[iRenderIndex].clear();

	// 기존 세팅으로 돌려놓기
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	pDevice->SetTransform(D3DTS_VIEW, &matPrevView);
	pDevice->SetTransform(D3DTS_PROJECTION, &matPrevProj);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return iEvent;
}

_uint CRenderer::Render_AlphaUI()
{
	LPDIRECT3DDEVICE9 pDevice = CManagement::Get_Instance()->Get_Device();
	if (nullptr == pDevice)
		return RENDER_ERROR;

	_uint iRenderIndex = (_uint)ERenderType::AlphaUI;
	_uint iEvent = NO_EVENT;

	// UI 조명 off
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	///////////////// 알파 테스팅 ///////////////////////////////////////////////
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 1); /* 알파 기준 값 설정 */
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER); /* 위에서 설정한 기준값보다 작은 것들 */

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	 // UI Render 후 기존으로 세팅하기 위한 변수들
	_float4x4 matPrevView;
	pDevice->GetTransform(D3DTS_VIEW, &matPrevView);
	_float4x4 matPrevProj;
	pDevice->GetTransform(D3DTS_PROJECTION, &matPrevProj);

	// World 항등행렬 세팅
	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	pDevice->SetTransform(D3DTS_WORLD, &matWorld);

	// 직교투영행렬 세팅
	_float2 winSize = CManagement::Get_Instance()->Get_WindowSize();
	_float4x4 matOrtho;
	D3DXMatrixOrthoLH(&matOrtho, winSize.x, winSize.y, 0.f, 1.f);
	pDevice->SetTransform(D3DTS_PROJECTION, &matOrtho);

	for (auto& pObject : m_GameObjects[iRenderIndex])
	{
		iEvent = pObject->Render_GameObject();
		Safe_Release(pObject);

		if (iEvent)
			return iEvent;
	}

	m_GameObjects[iRenderIndex].clear();

	// 기존 세팅으로 돌려놓기
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pDevice->SetTransform(D3DTS_VIEW, &matPrevView);
	pDevice->SetTransform(D3DTS_PROJECTION, &matPrevProj);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return iEvent;
}

void CRenderer::Free()
{
	for (_uint i = 0; i < (_uint)ERenderType::End; ++i)
	{
		for (auto& pObject : m_GameObjects[i])
		{
			Safe_Release(pObject);
		}

		m_GameObjects[i].clear();
	}		
}
