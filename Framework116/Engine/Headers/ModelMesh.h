#pragma once

#ifndef __MODEL_MESH_H__

#include "Mesh.h"

BEGIN(Engine)
class ENGINE_DLL CModelMesh final : public CMesh
{
private:
	explicit CModelMesh(LPDIRECT3DDEVICE9 pDevice, const TCHAR* pFilePath, const TCHAR* pTexturePath);
	explicit CModelMesh(const CModelMesh& other);
	virtual ~CModelMesh() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;
	virtual _uint	Render_Mesh();

public:
	static CModelMesh* Create(LPDIRECT3DDEVICE9 pDevice, const TCHAR * pFilePath, const TCHAR * pTexturePath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	vector<D3DMATERIAL9> m_vecMaterials;
	vector<LPDIRECT3DTEXTURE9> m_vecTextures;
	//IDirect3DBaseTexture9
	const TCHAR* m_pFilePath = nullptr;
	const TCHAR* m_pTexturePath = nullptr;

	// 콜라이드툴을 사용해서 채워넣기
	// vector<CollideInfo>
};
END

#define __MODEL_MESH_H__
#endif
