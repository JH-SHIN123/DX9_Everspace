#include "..\Headers\Management.h"

USING(Engine)
IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
	: m_pDevice_Manager(CDevice_Manager::Get_Instance())
	, m_pScene_Manager(CScene_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pGameObject_Manager(CGameObject_Manager::Get_Instance())
	, m_pRenderer(CRenderer::Get_Instance())
	, m_pTime_Manager(CTime_Manager::Get_Instance())
{
}

HRESULT CManagement::Ready_Game(HWND hWnd, _uint iWinCX, _uint iWinCY, EDisplayMode eMode)
{
	if (nullptr == m_pDevice_Manager || 
		nullptr == m_pScene_Manager || 
		nullptr == m_pComponent_Manager ||
		nullptr == m_pGameObject_Manager ||
		nullptr == m_pRenderer ||
		nullptr == m_pTime_Manager)
	{
		PRINT_LOG(L"Error", L"One of Managers is nullptr");
		return E_FAIL;
	}

	if (FAILED(m_pDevice_Manager->Ready_Graphic_Device(hWnd, iWinCX, iWinCY, eMode)))
	{
		PRINT_LOG(L"Error", L"Failed To Ready_Graphic_Device");
		return E_FAIL;
	}

	m_iWinCX = iWinCX;
	m_iWinCY = iWinCY;

	return S_OK;
}

_uint CManagement::Update_Game()
{
	if (nullptr == m_pDevice_Manager || 
		nullptr == m_pScene_Manager || 
		nullptr == m_pGameObject_Manager ||
		nullptr == m_pTime_Manager)
	{
		PRINT_LOG(L"Error", L"One of Managers is nullptr");
		return UPDATE_ERROR;
	}

	_uint iEvent = NO_EVENT;
	_float fDeltaTime = m_pTime_Manager->Update_Time_Manager();

	if (iEvent = m_pScene_Manager->Update_Scene(fDeltaTime))
		return iEvent;	

	if (iEvent = m_pGameObject_Manager->Update_GameObject(fDeltaTime))
		return iEvent;	

	if (iEvent = m_pScene_Manager->LateUpdate_Scene(fDeltaTime))
		return iEvent;

	if (iEvent = m_pGameObject_Manager->LateUpdate_GameObject(fDeltaTime))
		return iEvent;

	m_pDevice_Manager->Render_Begin();
	m_pRenderer->Render_GameObject();
	m_pDevice_Manager->Render_End();

	return iEvent;
}

void CManagement::Clear_NonStatic_Resources()
{
	if (nullptr == m_pComponent_Manager ||
		nullptr == m_pGameObject_Manager)
	{
		PRINT_LOG(L"Error", L"Component Manager or GameObject Manager is nullptr");
		return;
	}

	m_pComponent_Manager->Clear_NonStatic_Resources();
	m_pGameObject_Manager->Clear_NonStatic_Resources();
}

LPDIRECT3DDEVICE9 CManagement::Get_Device() const
{
	if (nullptr == m_pDevice_Manager)
	{
		PRINT_LOG(L"Error", L"Device Manager is nullptr");
		return nullptr;
	}

	return m_pDevice_Manager->Get_Device();
}

_float CManagement::Get_DeltaTime() const
{
	if (nullptr == m_pTime_Manager)
	{
		PRINT_LOG(L"Error", L"Time Manager is nullptr");
		return -1.f;
	}

	return m_pTime_Manager->Get_DeltaTime();
}

HRESULT CManagement::Setup_CurrentScene(_uint iNewSceneType, CScene * pNewScene)
{
	if (nullptr == m_pScene_Manager)
	{
		PRINT_LOG(L"Error", L"Scene Manager is nullptr");
		return E_FAIL;
	}

	return m_pScene_Manager->Setup_CurrentScene(iNewSceneType, pNewScene);
}

HRESULT CManagement::Add_Component_Prototype(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
	{
		PRINT_LOG(L"Error", L"Component Manager is nullptr");
		return E_FAIL;
	}

	return m_pComponent_Manager->Add_Component_Prototype(eType, PrototypeTag, pPrototype);
}


CComponent * CManagement::Clone_Component(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	void * pArg)
{
	if (nullptr == m_pComponent_Manager)
	{
		PRINT_LOG(L"Error", L"Component Manager is nullptr");
		return nullptr;
	}

	return m_pComponent_Manager->Clone_Component(eType, PrototypeTag, pArg);
}

const list<class CGameObject*>* CManagement::Get_GameObjectList(const wstring& LayerTag) const
{
	if (nullptr == m_pGameObject_Manager)
	{
		PRINT_LOG(L"Error", L"GameObject_Manager Manager is nullptr");
		return nullptr;
	}

	return m_pGameObject_Manager->Get_GameObjectList(LayerTag);
}

const CGameObject* CManagement::Get_GameObject(const wstring& LayerTag, _uint iIndex) const
{
	if (nullptr == m_pGameObject_Manager)
	{
		PRINT_LOG(L"Error", L"GameObject_Manager Manager is nullptr");
		return nullptr;
	}

	return m_pGameObject_Manager->Get_GameObject(LayerTag, iIndex);
}

const CComponent * CManagement::Get_Component(const wstring & LayerTag, const wstring & ComponentTag, _uint iIndex) const
{
	if (nullptr == m_pGameObject_Manager)
	{
		PRINT_LOG(L"Error", L"GameObject_Manager Manager is nullptr");
		return nullptr;
	}

	return m_pGameObject_Manager->Get_Component(LayerTag, ComponentTag, iIndex);
}

HRESULT CManagement::Add_GameObject_Prototype(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	CGameObject * pPrototype)
{
	if (nullptr == m_pGameObject_Manager)
	{
		PRINT_LOG(L"Error", L"GameObject_Manager Manager is nullptr");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Add_GameObject_Prototype(eType, PrototypeTag, pPrototype);
}

HRESULT CManagement::Add_GameObject_InLayer(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	const wstring & LayerTag, 
	void * pArg)
{
	if (nullptr == m_pGameObject_Manager)
	{
		PRINT_LOG(L"Error", L"GameObject_Manager Manager is nullptr");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Add_GameObject_InLayer(eType, PrototypeTag, LayerTag, pArg);
}

HRESULT CManagement::Add_GameObject_InLayer_Tool(EResourceType eType, const wstring& PrototypeTag, const wstring& LayerTag, const int _iListboxIndex, void* pArg)
{
	if (nullptr == m_pGameObject_Manager)
	{
		PRINT_LOG(L"Error", L"GameObject_Manager Manager is nullptr");
		return E_FAIL;
	}

	return m_pGameObject_Manager->Add_GameObject_InLayer_Tool(eType, PrototypeTag, LayerTag, _iListboxIndex, pArg);
}

CGameObject * CManagement::Clone_GameObject(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	void * pArg)
{
	if (nullptr == m_pGameObject_Manager)
	{
		PRINT_LOG(L"Error", L"GameObject Manager is nullptr");
		return nullptr;
	}

	return m_pGameObject_Manager->Clone_GameObject(eType, PrototypeTag, pArg);
}

HRESULT CManagement::Add_GameObject_InRenderer(ERenderType eType, CGameObject * pObject)
{
	if (nullptr == m_pRenderer)
	{
		PRINT_LOG(L"Error", L"m_pRenderer is nullptr");
		return E_FAIL;
	}

	return m_pRenderer->Add_GameObject(eType, pObject);
}

void CManagement::Free()
{
	if (Safe_Release(m_pTime_Manager))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Time_Manager");
	}

	if (Safe_Release(m_pRenderer))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Renderer");
	}

	if (Safe_Release(m_pGameObject_Manager))
	{
		PRINT_LOG(L"Warning", L"Failed To Release GameObject_Manager");
	}

	if (Safe_Release(m_pComponent_Manager))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Component_Manager");
	}

	if (Safe_Release(m_pScene_Manager))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Scene_Manager");
	}

	if (Safe_Release(m_pDevice_Manager))
	{
		PRINT_LOG(L"Warning", L"Failed To Release Device_Manager");
	}
}
