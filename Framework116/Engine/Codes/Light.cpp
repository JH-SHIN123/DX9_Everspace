#include "Light.h"

USING(Engine)

int CLight::s_iLightIndex = -1;

CLight::CLight(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CLight::CLight(const CLight& other)
	: CGameObject(other)
{
}

void CLight::InitDirectionalLight(_float3* direction, D3DXCOLOR* color)
{
	::ZeroMemory(&m_tLight, sizeof(m_tLight));

	m_tLight.Type = D3DLIGHT_DIRECTIONAL;
	m_tLight.Diffuse = *color;
	m_tLight.Ambient = *color * 0.2f;
	m_tLight.Specular = *color * 0.6f;
	m_tLight.Direction = *direction;
}

void CLight::Set_LightDir(const _float3& _vDir)
{
	m_tLight.Direction = _vDir;
}

void CLight::InitPointLight(_float3* position, D3DXCOLOR* color)
{
	::ZeroMemory(&m_tLight, sizeof(m_tLight));

	// 임의로 Range 지정함
	m_tLight.Type = D3DLIGHT_POINT;
	m_tLight.Ambient = *color * 0.6f;
	m_tLight.Diffuse = *color;
	m_tLight.Specular = *color * 0.6f;
	m_tLight.Position = *position;
	m_tLight.Range = 150.f;
	m_tLight.Falloff = 1.f;
	m_tLight.Attenuation0 = 1.f;
	m_tLight.Attenuation1 = 0.0f;
	m_tLight.Attenuation2 = 0.0f;
}

void CLight::InitSpotLight(_float3* position, _float3* direction, D3DXCOLOR* color)
{
	::ZeroMemory(&m_tLight, sizeof(m_tLight));

	m_tLight.Type = D3DLIGHT_SPOT;
	m_tLight.Ambient = *color * 0.0f;
	m_tLight.Diffuse = *color;
	m_tLight.Specular = *color * 0.6f;
	m_tLight.Position = *position;
	m_tLight.Direction = *direction;
	m_tLight.Range = 25.f;
	m_tLight.Falloff = 1.0f;
	m_tLight.Attenuation0 = 1.0f;
	m_tLight.Attenuation1 = 0.0f;
	m_tLight.Attenuation2 = 0.0f;
	m_tLight.Theta = 0.4f;
	m_tLight.Phi = 0.9f;
}

HRESULT CLight::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLight::Ready_GameObject(void* pArg)
{
	CGameObject::Ready_GameObject(pArg);
	// 전역 light 인덱스 증가
	++s_iLightIndex;

	LIGHT_DESC* lightDescPtr = nullptr;

	TRANSFORM_DESC transformDesc;
	_float3 vLightDir = { 1.0f, -0.0f, 0.25f }; // Default : 의미없음.
	D3DXCOLOR tLightColor = D3DCOLOR_XRGB(255, 255, 255);

	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (lightDescPtr = dynamic_cast<LIGHT_DESC*>(ptr))
		{
			transformDesc = lightDescPtr->tTransformDesc;
			m_eLightType = lightDescPtr->eLightType;
			vLightDir = lightDescPtr->vLightDir;
			tLightColor = lightDescPtr->tLightColor;
		}
	}

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		L"Component_Transform",
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		(void*)&transformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	switch (m_eLightType)
	{
	case ELightType::Directional:
		InitDirectionalLight(&vLightDir,&tLightColor);
		break;
	case ELightType::SpotLight:
		InitSpotLight(&vLightDir, &transformDesc.vPosition, &tLightColor);
		break;
	case ELightType::PointLight:
		InitPointLight(&transformDesc.vPosition, &tLightColor);
		break;
	}

	// 일단 조명 한개만 설치할거임
	m_pDevice->SetLight(s_iLightIndex, &m_tLight);
	m_pDevice->LightEnable(s_iLightIndex, true);

	m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	return S_OK;
}

_uint CLight::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	return m_pTransform->Update_Transform();
}

_uint CLight::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);
	m_tLight.Position = m_pTransform->Get_State(EState::Position);

	return _uint();
}

_uint CLight::Render_GameObject()
{
	m_pDevice->SetLight(s_iLightIndex, &m_tLight);

	return _uint();
}

CLight* CLight::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLight* pInstance = new CLight(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLight::Clone(void* pArg)
{
	CLight* pClone = new CLight(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLight::Free()
{
	--s_iLightIndex;
	Safe_Release(m_pTransform);
	CGameObject::Free();
}
