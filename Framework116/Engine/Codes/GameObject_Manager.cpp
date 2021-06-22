#include "..\Headers\GameObject_Manager.h"
#include "GameObject.h"
#include "Layer.h"

USING(Engine)
IMPLEMENT_SINGLETON(CGameObject_Manager)

CGameObject_Manager::CGameObject_Manager()
{
}

const CGameObject* CGameObject_Manager::Get_GameObject(const wstring& LayerTag, _uint iIndex) const
{
	auto iter = m_Layers.find(LayerTag);
	if (m_Layers.end() == iter)
		return nullptr;

	return iter->second->Get_GameObject(iIndex);
}

const CComponent * CGameObject_Manager::Get_Component(const wstring & LayerTag, const wstring & ComponentTag, _uint iIndex) const
{
	auto iter = m_Layers.find(LayerTag);
	if (m_Layers.end() == iter)
		return nullptr;

	return iter->second->Get_Component(ComponentTag, iIndex);
}

HRESULT CGameObject_Manager::Add_GameObject_Prototype(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	CGameObject * pPrototype)
{
	_uint iTypeIndex = (_uint)eType;

	if ((_uint)EResourceType::End <= iTypeIndex)
	{
		PRINT_LOG(L"Error", L"Out of range m_GameObjects");
		return E_FAIL;
	}

	if (nullptr == pPrototype)
	{
		PRINT_LOG(L"Error", L"pPrototype is nullptr");
		return E_FAIL;
	}

	auto iter_find = m_GameObjects[iTypeIndex].find(PrototypeTag);
	if (m_GameObjects[iTypeIndex].end() == iter_find)
	{
		m_GameObjects[iTypeIndex].insert(make_pair(PrototypeTag, pPrototype));
	}
	else
	{
		Safe_Release(pPrototype);
	}

	return S_OK;
}

HRESULT CGameObject_Manager::Add_GameObject_InLayer(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	const wstring & LayerTag, void * pArg)
{
	CGameObject* pClone = Clone_GameObject(eType, PrototypeTag, pArg);
	if (nullptr == pClone)
	{
		PRINT_LOG(L"Error", L"Failed To Clone_GameObject");
		return E_FAIL;
	}

	auto iter_find = m_Layers.find(LayerTag);
	if (m_Layers.end() == iter_find)
	{
		CLayer* pLayer = CLayer::Create();
		m_Layers.insert(make_pair(LayerTag, pLayer));
	}	

	return m_Layers[LayerTag]->Add_GameObject_InLayer(pClone);
}

CGameObject * CGameObject_Manager::Clone_GameObject(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	void * pArg)
{
	_uint iTypeIndex = (_uint)eType;

	if ((_uint)EResourceType::End <= iTypeIndex)
	{
		PRINT_LOG(L"Error", L"Out of range m_GameObjects");
		return nullptr;
	}

	auto iter_find = m_GameObjects[iTypeIndex].find(PrototypeTag);
	if (m_GameObjects[iTypeIndex].end() == iter_find)
	{
		wstring ErrorMsg = PrototypeTag + L" is not found";
		PRINT_LOG(L"Error", ErrorMsg.c_str());
		return nullptr;
	}

	return iter_find->second->Clone(pArg);
}

void CGameObject_Manager::Clear_NonStatic_Resources()
{
	for (auto& Pair : m_Layers)
	{
		Safe_Release(Pair.second);
	}

	m_Layers.clear();

	for (auto& Pair : m_GameObjects[(_uint)EResourceType::NonStatic])
	{
		Safe_Release(Pair.second);
	}

	m_GameObjects[(_uint)EResourceType::NonStatic].clear();
}

_uint CGameObject_Manager::Update_GameObject(_float fDeltaTime)
{
	_uint iEvent = NO_EVENT;

	for (auto& Pair : m_Layers)
	{
		if (iEvent = Pair.second->Update_Layer(fDeltaTime))
			return iEvent;
	}

	return iEvent;
}

_uint CGameObject_Manager::LateUpdate_GameObject(_float fDeltaTime)
{
	_uint iEvent = NO_EVENT;

	for (auto& Pair : m_Layers)
	{
		if (iEvent = Pair.second->LateUpdate_Layer(fDeltaTime))
			return iEvent;
	}

	return iEvent;
}

void CGameObject_Manager::Free()
{
	for (auto& Pair : m_Layers)
	{
		Safe_Release(Pair.second);
	}

	m_Layers.clear();

	for (_uint i = 0; i < (_uint)EResourceType::End; ++i)
	{
		for (auto& Pair : m_GameObjects[i])
		{
			Safe_Release(Pair.second);
		}

		m_GameObjects[i].clear();
	}
}
