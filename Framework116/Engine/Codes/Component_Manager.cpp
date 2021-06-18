#include "Component_Manager.h"
#include "Component.h"

USING(Engine)
IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Add_Component_Prototype(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	CComponent * pPrototype)
{
	_uint iTypeIndex = (_uint)eType;

	if ((_uint)EResourceType::End <= iTypeIndex)
	{
		PRINT_LOG(L"Error", L"Out of range m_Components");
		return E_FAIL;
	}

	if (nullptr == pPrototype)
	{
		PRINT_LOG(L"Error", L"pPrototype is nullptr");
		return E_FAIL;
	}

	auto iter_find = m_Components[iTypeIndex].find(PrototypeTag);
	if (m_Components[iTypeIndex].end() == iter_find)
	{
		m_Components[iTypeIndex].insert(make_pair(PrototypeTag, pPrototype));
	}
	else
	{
		Safe_Release(pPrototype);
	}

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(
	EResourceType eType,
	const wstring & PrototypeTag,
	void * pArg)
{
	_uint iTypeIndex = (_uint)eType;

	if ((_uint)EResourceType::End <= iTypeIndex)
	{
		PRINT_LOG(L"Error", L"Out of range m_Components");
		return nullptr;
	}

	auto iter_find = m_Components[iTypeIndex].find(PrototypeTag);
	if (m_Components[iTypeIndex].end() == iter_find)
	{
		wstring ErrorMsg = PrototypeTag + L"is not found";
		PRINT_LOG(L"Error", ErrorMsg.c_str());
		return nullptr;
	}	

	return iter_find->second->Clone(pArg);
}

void CComponent_Manager::Clear_NonStatic_Resources()
{
	for (auto& Pair : m_Components[(_uint)EResourceType::NonStatic])
	{
		Safe_Release(Pair.second);
	}

	m_Components[(_uint)EResourceType::NonStatic].clear();
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < (_uint)EResourceType::End; ++i)
	{
		for (auto& Pair : m_Components[i])
		{
			Safe_Release(Pair.second);
		}

		m_Components[i].clear();
	}
}
