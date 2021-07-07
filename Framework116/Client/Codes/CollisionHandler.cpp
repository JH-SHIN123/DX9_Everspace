#include"stdafx.h"
#include "CollisionHandler.h"
#include "Collision.h"

void CCollisionHandler::Collision_SphereToSphere(const wstring& wstrDstLayerTag, const wstring& wstrSrcLayerTag)
{
	const list<CGameObject*>* Dst_Layer = CManagement::Get_Instance()->Get_GameObjectList(wstrDstLayerTag);
	const list<CGameObject*>* Src_Layer = CManagement::Get_Instance()->Get_GameObjectList(wstrSrcLayerTag);

	if (nullptr == Dst_Layer) return;
	if (nullptr == Src_Layer) return;

	if (wstrSrcLayerTag == L"Layer_Planet")
	{
		for (auto & dst : *Dst_Layer)
		{
			if (nullptr == dst) continue;
			const vector<class CCollide*>* dstCollides = dst->Get_Collides();
			if (nullptr == dstCollides) continue;

			for (auto& src : *Src_Layer)
			{
				if (nullptr == src) continue;
				const vector<class CCollide*>* srcCollides = src->Get_Collides();
				if (nullptr == srcCollides) continue;

				//
				CTransform* pDstTransform = (CTransform*)dst->Get_Component(L"Com_Transform");
				CTransform* pSrcTransform = (CTransform*)src->Get_Component(L"Com_Transform");

				if (nullptr == pDstTransform || nullptr == pSrcTransform) continue;

				float fDstScaleRate = 0.f;
				fDstScaleRate += pDstTransform->Get_TransformDesc().vScale.x;
				fDstScaleRate += pDstTransform->Get_TransformDesc().vScale.y;
				fDstScaleRate += pDstTransform->Get_TransformDesc().vScale.z;
				fDstScaleRate /= 3.f;

				float fSrcScaleRate = 0.f;
				fSrcScaleRate += pSrcTransform->Get_TransformDesc().vScale.x;
				fSrcScaleRate += pSrcTransform->Get_TransformDesc().vScale.y;
				fSrcScaleRate += pSrcTransform->Get_TransformDesc().vScale.z;
				fSrcScaleRate /= 3.f;

				if (Check_Collides(dstCollides, srcCollides, fDstScaleRate, fSrcScaleRate))
				{
					// 충돌 OK
					dst->Set_IsAstroidCollide(true);
					break;
				}
			}
		}
	}
	else
	{
		for (auto & dst : *Dst_Layer)
		{
			if (nullptr == dst) continue;
			const vector<class CCollide*>* dstCollides = dst->Get_Collides();
			if (nullptr == dstCollides) continue;

			for (auto& src : *Src_Layer)
			{
				if (nullptr == src) continue;
				const vector<class CCollide*>* srcCollides = src->Get_Collides();
				if (nullptr == srcCollides) continue;

				//
				CTransform* pDstTransform = (CTransform*)dst->Get_Component(L"Com_Transform");
				CTransform* pSrcTransform = (CTransform*)src->Get_Component(L"Com_Transform");

				if (nullptr == pDstTransform || nullptr == pSrcTransform) continue;

				float fDstScaleRate = 0.f;
				fDstScaleRate += pDstTransform->Get_TransformDesc().vScale.x;
				fDstScaleRate += pDstTransform->Get_TransformDesc().vScale.y;
				fDstScaleRate += pDstTransform->Get_TransformDesc().vScale.z;
				fDstScaleRate /= 3.f;

				float fSrcScaleRate = 0.f;
				fSrcScaleRate += pSrcTransform->Get_TransformDesc().vScale.x;
				fSrcScaleRate += pSrcTransform->Get_TransformDesc().vScale.y;
				fSrcScaleRate += pSrcTransform->Get_TransformDesc().vScale.z;
				fSrcScaleRate /= 3.f;

				if (Check_Collides(dstCollides, srcCollides, fDstScaleRate, fSrcScaleRate))
				{
					// 충돌 OK
					dst->Set_IsCollide(true);
					src->Set_IsCollide(true);
					
					break;
				}
			}
		}
	}
}

bool CCollisionHandler::Check_Collides(const vector<class CCollide*>* pDstCollides, const vector<class CCollide*>* pSrcCollides,
	const _float vDstScaleRate, const _float vSrcScaleRate)
{
	for (auto& dst : *pDstCollides) 
	{
		if (nullptr == dst) continue;
		for (auto& src : *pSrcCollides)
		{
			if (nullptr == src) continue;
			if (CCollision::IntersectSphereToSphere(dst->Get_BoundingSphere(), src->Get_BoundingSphere(), vDstScaleRate, vSrcScaleRate))
				return true;
		}
	}

	return false;
}
