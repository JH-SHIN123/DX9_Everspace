#include "..\Headers\Renderer.h"
#include "GameObject.h"
#include "Management.h"

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

_uint CRenderer::Render_UI()
{
	_uint iRenderIndex = (_uint)ERenderType::UI;
	_uint iEvent = NO_EVENT;

	// 직교투영행렬 세팅


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
