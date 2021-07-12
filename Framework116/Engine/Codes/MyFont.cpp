#include "MyFont.h"
#include "Management.h"

USING(Engine)

CMyFont::CMyFont(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CMyFont::CMyFont(const CMyFont& other)
	: CGameObject(other)
{
}

void CMyFont::Set_Text(const wstring& wstrText)
{
	m_wstrText = wstrText;
}

void CMyFont::Set_TextColor(D3DXCOLOR tTextColor)
{
	m_tTextColor = tTextColor;
}

void CMyFont::Set_Position(const _float3& vPos)
{
	if (nullptr == m_pTransform) return;
	m_pTransform->Set_Position(vPos);
}

void CMyFont::Set_Scale(const _float3& vScale)
{
	if (nullptr == m_pTransform) return;
	m_pTransform->Set_Scale(vScale);
}

HRESULT CMyFont::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CMyFont::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);

	MYFONT_DESC* pDescPtr = nullptr;
	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (pDescPtr = dynamic_cast<MYFONT_DESC*>(ptr))
		{
			m_wstrText = pDescPtr->wstrText;
			m_tTextColor = pDescPtr->tTextColor;
		}
	}

	TRANSFORM_DESC tTransformDesc = pDescPtr->tTransformDesc;
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		(void*)&tTransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	return S_OK;
}

_uint CMyFont::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	if (m_IsDead) return DEAD_OBJECT;

	return m_pTransform->Update_Transform_Quaternion();
}

_uint CMyFont::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (m_IsDead) return DEAD_OBJECT;

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::Font, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CMyFont::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pManagement->Get_Sprite()->SetTransform(&m_pTransform->Get_TransformDesc().matWorld);
	m_pManagement->Get_Font()->DrawText(m_pManagement->Get_Sprite(), m_wstrText.c_str(), m_wstrText.length(), nullptr, 0, m_tTextColor);

	return _uint();
}

CMyFont* CMyFont::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CMyFont* pInstance = new CMyFont(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMyFont::Clone(void* pArg)
{
	CMyFont* pClone = new CMyFont(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CMyFont::Free()
{
	Safe_Release(m_pTransform);
	CGameObject::Free();
}
