#include "..\Headers\Transform.h"
#include "Pipeline.h"

USING(Engine)

CTransform::CTransform(LPDIRECT3DDEVICE9 pDevice)
	: CComponent(pDevice)
{
}

CTransform::CTransform(const CTransform & other)
	: CComponent(other)
{
}

_float3 CTransform::Get_State(EState eState) const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	_float3 vState = _float3(0.f, 0.f, 0.f);

	switch (eState)
	{
	case EState::Right:
		memcpy(&vState, &m_TransformDesc.matWorld._11, sizeof(_float3));
		break;
	case EState::Up:
		memcpy(&vState, &m_TransformDesc.matWorld._21, sizeof(_float3));
		break;
	case EState::Look:
		memcpy(&vState, &m_TransformDesc.matWorld._31, sizeof(_float3));
		break;
	case EState::Position:
		memcpy(&vState, &m_TransformDesc.matWorld._41, sizeof(_float3));
		break;
	default:
		break;
	}

	return vState;
}

void CTransform::Set_Position(const _float3 & vPos)
{
	m_TransformDesc.vPosition = vPos;
}

void CTransform::Set_Rot(const _float3 & vRot)
{
	m_TransformDesc.vRotate = vRot;
}

void CTransform::Set_WorldMatrix(const _float4x4 & NewWorld)
{
	m_TransformDesc.matWorld = NewWorld;
}


const TRANSFORM_DESC & CTransform::Get_TransformDesc() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_TransformDesc;
}

HRESULT CTransform::Ready_Component_Prototype()
{
	CComponent::Ready_Component_Prototype();

	return S_OK;
}

HRESULT CTransform::Ready_Component(void * pArg/* = nullptr*/)
{
	CComponent::Ready_Component(pArg);

	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (dynamic_cast<TRANSFORM_DESC*>(ptr))
		{
			memcpy(&m_TransformDesc, ptr, sizeof(TRANSFORM_DESC));
		}		
	}

	return S_OK;
}

_uint CTransform::Update_Transform()
{
	CPipeline::Setup_WorldMatrix(
		&m_TransformDesc.matWorld, 
		&m_TransformDesc.vScale, 
		&m_TransformDesc.vRotate, 
		&m_TransformDesc.vPosition);	

	return _uint();
}

_uint CTransform::Go_Straight(_float fDeltaTime)
{
	_float3 vLook;
	memcpy(&vLook, &m_TransformDesc.matWorld._31, sizeof(_float3));
	D3DXVec3Normalize(&vLook, &vLook);

	m_TransformDesc.vPosition += vLook * m_TransformDesc.fSpeedPerSec * fDeltaTime;

	return _uint();
}

_uint CTransform::Go_Side(_float fDeltaTime)
{
	_float3 vRight;
	memcpy(&vRight, &m_TransformDesc.matWorld._11, sizeof(_float3));
	D3DXVec3Normalize(&vRight, &vRight);

	m_TransformDesc.vPosition += vRight * m_TransformDesc.fSpeedPerSec * fDeltaTime;

	return _uint();
}

_uint CTransform::RotateX(_float fDeltaTime)
{
	m_TransformDesc.vRotate.x += m_TransformDesc.fRotatePerSec * fDeltaTime;

	return _uint();
}

_uint CTransform::RotateY(_float fDeltaTime)
{
	m_TransformDesc.vRotate.y += m_TransformDesc.fRotatePerSec * fDeltaTime;

	return _uint();
}

_uint CTransform::RotateZ(_float fDeltaTime)
{
	m_TransformDesc.vRotate.z += m_TransformDesc.fRotatePerSec * fDeltaTime;

	return _uint();
}

CTransform * CTransform::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CTransform* pInstance = new CTransform(pDevice);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Transform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg/* = nullptr*/)
{
	CTransform* pClone = new CTransform(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Transform");
		Safe_Release(pClone);
	}

	return pClone;
}

void CTransform::Free()
{
	CComponent::Free();
}
