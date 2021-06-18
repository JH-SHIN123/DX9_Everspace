#include "..\Headers\Scene.h"
#include "Management.h"

USING(Engine)

CScene::CScene(LPDIRECT3DDEVICE9 pDevice)
	: m_pDevice(pDevice)
	, m_pManagement(CManagement::Get_Instance())
{
	Safe_AddRef(m_pDevice);
}

HRESULT CScene::Ready_Scene()
{
	return S_OK;
}

_uint CScene::Update_Scene(_float fDeltaTime)
{
	return S_OK;
}

_uint CScene::LateUpdate_Scene(_float fDeltaTime)
{
	return S_OK;
}

void CScene::Free()
{
	Safe_Release(m_pDevice);
}
