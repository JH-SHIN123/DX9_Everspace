#include "..\Headers\Camera.h"
#include "Pipeline.h"

USING(Engine)

CCamera::CCamera(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
}

CCamera::CCamera(const CCamera & other)
	: CGameObject(other)
{
}

const CAMERA_DESC& CCamera::Get_CameraDesc() const
{
	return m_CameraDesc;
}

HRESULT CCamera::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CCamera::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);

	if (auto ptr = (BASE_DESC*)pArg)
	{
		if (dynamic_cast<CAMERA_DESC*>(ptr))
		{
			memcpy(&m_CameraDesc, ptr, sizeof(CAMERA_DESC));
		}
	}

	return S_OK;
}

_uint CCamera::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	return _uint();
}

_uint CCamera::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	/* 뷰행렬 만들기 */	
	CPipeline::Setup_ViewMatrix(
		&m_CameraDesc.matView, /* 뷰행렬 반환 */
		&m_CameraDesc.vEye, /* 월드에서의 카메라 위치(eye) */
		&m_CameraDesc.vAt, /* 월드에서의 카메라가 바라보는 위치(at) */
		&m_CameraDesc.vUp /* 뷰행렬 만들기 위해 필요한 up벡터 */);
	m_pDevice->SetTransform(D3DTS_VIEW, &m_CameraDesc.matView);


	/* 원근 투영행렬 만들기 */
	CPipeline::Setup_ProjectionMatrix(
		&m_CameraDesc.matProj, /* 투영행렬 반환 */
		m_CameraDesc.fFovY, /* FovY: Y축 시야각. */
		m_CameraDesc.fAspect, /* Aspect 종횡비 */
		m_CameraDesc.fNear, /* Near평면의 z값. 현재 카메라와의 깊이 */
		m_CameraDesc.fFar /* Far평면의 z값. 현재 카메라와의 깊이 */);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &m_CameraDesc.matProj);

	return _uint();
}

_uint CCamera::Render_GameObject()
{
	CGameObject::Render_GameObject();

	return _uint();
}

void CCamera::Free()
{
	CGameObject::Free();
}
