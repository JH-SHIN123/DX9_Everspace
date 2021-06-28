#include "pch.h"
#include "..\Headers\Dummy_Object.h"


CDummy_Object::CDummy_Object(LPDIRECT3DDEVICE9 pDevice, const PASSDATA_OBJECT& tPASSDATA_OBJECT)
	: CGameObject(pDevice)
{
	m_tPassData = tPASSDATA_OBJECT;
}

CDummy_Object::CDummy_Object(const CDummy_Object & other)
	: CGameObject(other)
	, m_tPassData(other.m_tPassData)
{

}

HRESULT CDummy_Object::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CDummy_Object::Ready_GameObject(void * pArg /*= nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	return S_OK;
}

_uint CDummy_Object::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CDummy_Object::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CDummy_Object::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();

	return _uint();
}

CDummy_Object * CDummy_Object::Create(LPDIRECT3DDEVICE9 pDevice, const PASSDATA_OBJECT & tPASSDATA_OBJECT)
{
	CDummy_Object* pInstance = new CDummy_Object(pDevice, tPASSDATA_OBJECT);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Dummy_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDummy_Object::Clone(void * pArg /*= nullptr*/)
{
	CDummy_Object* pClone = new CDummy_Object(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Dummy_Object");
		Safe_Release(pClone);
	}

	return pClone;
}

void CDummy_Object::Free()
{
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);

	CGameObject::Free();
}
