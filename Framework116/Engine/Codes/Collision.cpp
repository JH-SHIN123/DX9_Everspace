#include "Collision.h"
#include "Layer.h"
#include "GameObject.h"
#include "Collide.h"
#include "CollideSphere.h"
#include "Pipeline.h"

USING(Engine)

CGameObject* CCollision::PickingObject(float& OutDist, const HWND hWnd, const int iWinCX, const int iWinCY, const LPDIRECT3DDEVICE9 pDevice, const list<CGameObject*>* listObject)
{
	const CGameObject* pPickingObject = nullptr;
	float fMinDist = -1.f;
	OutDist = fMinDist;

	if (nullptr == listObject) return nullptr;

	RAY ray;
	CPipeline::CreatePickingRay(ray, hWnd, iWinCX, iWinCY, pDevice);

	D3DXMATRIX view;
	pDevice->GetTransform(D3DTS_VIEW, &view);
	CPipeline::TransformRay(ray, view);

	for (auto& Obj : (*listObject)) {
		if (nullptr == Obj || Obj->Get_IsEmptyCollides()) continue;

		for (auto& Collide : (*Obj->Get_Collides())) {
			if (ECollideType::Sphere == Collide->Get_CollideType()) {
				const BOUNDING_SPHERE& bounds = static_cast<CCollideSphere*>(Collide)->Get_BoundingSphere();
				if (CCollision::IntersectRayToSphere(ray, bounds)) {

					// Ray Center <-> BoundingSphere : z 체크
					float fDist = CPipeline::Get_Distance(ray.vPos, bounds.Get_Position());
					if (fMinDist == -1.f) {
						fMinDist = fDist;
						pPickingObject = Obj;
					}
					else if (fMinDist >= fDist) {
						fMinDist = fDist;
						pPickingObject = Obj;
					}
				}
			}
		}
	}

	OutDist = fMinDist;
	return const_cast<CGameObject*>(pPickingObject);
}

bool CCollision::IntersectRayToSphere(const RAY& pInRay, const BOUNDING_SPHERE& pBounds)
{
	_float3 vBoundsToRay = pInRay.vPos - pBounds.Get_Position();

	float b = 2.f * D3DXVec3Dot(&pInRay.vDirection, &vBoundsToRay);
	float c =  D3DXVec3Dot(&vBoundsToRay, &vBoundsToRay) - (pBounds.fRadius * pBounds.fRadius);

	// b^2 - 4*c
	// 판별식을 찾는다.
	float discriminant = (b * b) - (4.0f * c);

	// 가상의 수에 대한 테스트
	if (discriminant < 0.0f)
		return false;

	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;

	// 해가 >= 0 일경우 구체를 교차하는 것이다.
	if (s0 >= 0.0f || s1 >= 0.0f)
		return true;

	return false;
}
