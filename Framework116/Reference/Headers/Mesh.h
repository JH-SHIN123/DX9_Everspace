#pragma once
#ifndef __MESH_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CComponent
{
private:
	explicit CMesh(LPDIRECT3DDEVICE9 pDevice, const TCHAR * pFilePath, const TCHAR* pTexturePath);
	explicit CMesh(const CMesh& other);
	virtual ~CMesh() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component(void* pArg = nullptr) override;
	virtual _uint	Render_Mesh();

public:
	static CMesh* Create(LPDIRECT3DDEVICE9 pDevice, const TCHAR * pFilePath, const TCHAR * pTexturePath);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	LPD3DXMESH m_pMesh = nullptr;
	vector<D3DMATERIAL9> m_vecMaterials;
	vector<LPDIRECT3DTEXTURE9> m_vecTextures;
	//IDirect3DBaseTexture9
	const TCHAR* m_pFilePath = nullptr;
	const TCHAR* m_pTexturePath = nullptr;

	// 콜라이드툴을 사용해서 채워넣기
	// vector<CCollideSphere>
};
END

#define __MESH_H__
#endif
