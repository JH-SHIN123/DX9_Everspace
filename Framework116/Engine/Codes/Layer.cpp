#include "..\Headers\Layer.h"
#include "GameObject.h"

USING(Engine)

CLayer::CLayer()
{
}

const CGameObject* CLayer::Get_GameObject(_uint iIndex) const
{
	if (m_GameObjects.size() <= iIndex)
		return nullptr;

	auto iter = m_GameObjects.begin();
	if (m_GameObjects.end() == iter)
		return nullptr;

	for (_uint i = 0; i < iIndex; ++i, ++iter);

	return (*iter);
}

const CComponent * CLayer::Get_Component(const wstring & ComponentTag, _uint iIndex/* = 0*/) const
{
	if (m_GameObjects.size() <= iIndex)
		return nullptr;

	auto iter = m_GameObjects.begin();
	if (m_GameObjects.end() == iter)
		return nullptr;

	for (_uint i = 0; i < iIndex; ++i, ++iter);

	return (*iter)->Get_Component(ComponentTag);
}

HRESULT CLayer::Add_GameObject_InLayer(CGameObject * pObject)
{
	if (nullptr == pObject)
	{
		PRINT_LOG(L"Error", L"pObject is nullptr");
		return E_FAIL;
	}

	auto iter_find = find(m_GameObjects.begin(), m_GameObjects.end(), pObject);
	if (m_GameObjects.end() == iter_find)
	{
		m_GameObjects.push_back(pObject);
	}
	else
	{
		Safe_Release(pObject);
	}

	return S_OK;
}

_uint CLayer::Update_Layer(_float fDeltaTime)
{
	_uint iEvent = NO_EVENT;

	for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (iEvent = (*iter)->Update_GameObject(fDeltaTime))
		{
			switch (iEvent)
			{
			case DEAD_OBJECT:
				Safe_Release(*iter);
				iter = m_GameObjects.erase(iter);
				break;
			case CHANGE_SCENE:
				return iEvent;
			}
		}
		else ++iter;
	}

	return iEvent;
}

_uint CLayer::LateUpdate_Layer(_float fDeltaTime)
{
	_uint iEvent = NO_EVENT;

	for (auto& iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (iEvent = (*iter)->LateUpdate_GameObject(fDeltaTime))
		{
			switch (iEvent)
			{
			case DEAD_OBJECT:
				Safe_Release(*iter);
				iter = m_GameObjects.erase(iter);
				break;
			case CHANGE_SCENE:
				return iEvent;
			}
		}
		else ++iter;
	}

	return iEvent;
}

CLayer * CLayer::Create()
{
	return new CLayer;
}

void CLayer::Free()
{
	for (auto& pObject : m_GameObjects)
	{
		Safe_Release(pObject);
	}

	m_GameObjects.clear();
}
