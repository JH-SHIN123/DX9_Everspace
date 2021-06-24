#include "..\Headers\GameObject.h"
#include "Management.h"
#include "Component.h"

USING(Engine)

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pDevice)
	: m_pDevice(pDevice)
	, m_pManagement(CManagement::Get_Instance())
{
	Safe_AddRef(m_pDevice);
}

CGameObject::CGameObject(const CGameObject & other)
	: m_pDevice(other.m_pDevice)
	, m_pManagement(other.m_pManagement)
	, m_IsClone(true)
{
	Safe_AddRef(m_pDevice);
}

const CComponent * CGameObject::Get_Component(const wstring & ComponentTag) const
{
	auto iter = m_Components.find(ComponentTag);
	if (m_Components.end() == iter)
		return nullptr;

	return iter->second;
}

const _bool CGameObject::Get_IsEmptyCollides() const
{
	return m_Collides.empty();
}

HRESULT CGameObject::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Ready_GameObject(void * pArg)
{
	return S_OK;
}

_uint CGameObject::Update_GameObject(_float fDeltaTime)
{
	return NO_EVENT;
}

_uint CGameObject::LateUpdate_GameObject(_float fDeltaTime)
{
	return NO_EVENT;
}

_uint CGameObject::Render_GameObject()
{
	return NO_EVENT;
}

HRESULT CGameObject::Add_Component(
	EResourceType eType, 
	const wstring & PrototypeTag, 
	const wstring & ComponentTag, 
	CComponent** ppComponent, void * pArg, 
	const bool _bCollide)
{
	auto iter_find = m_Components.find(ComponentTag);
	if (m_Components.end() == iter_find)
	{
		CComponent* pClone = m_pManagement->Clone_Component(eType, PrototypeTag, pArg);
		if (nullptr == pClone)
		{
			wstring ErrorMsg = ComponentTag + L"is nullptr";
			PRINT_LOG(L"Error", ErrorMsg.c_str());
			return E_FAIL;
		}

		m_Components.insert(make_pair(ComponentTag, pClone));

		if (ppComponent)
		{
			*ppComponent = pClone;
			Safe_AddRef(pClone);
		}

		if (_bCollide)
		{
			m_Collides.emplace_back(static_cast<CCollide*>(pClone));
			Safe_AddRef(pClone);
		}
	}	

	return S_OK;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
	{
		Safe_Release(Pair.second);
	}
	m_Components.clear();

	for (auto& Collide : m_Collides)
	{
		Safe_Release(Collide);
	}
	m_Collides.clear();

	Safe_Release(m_pDevice);
}
