#include "Mesh.h"
#include "..\Headers\ModelMesh.h"

USING(Engine)

CModelMesh::CModelMesh(LPDIRECT3DDEVICE9 pDevice, const TCHAR* pFilePath, const TCHAR* pTexturePath) :
	CMesh(pDevice),
	m_pFilePath(pFilePath),
	m_pTexturePath(pTexturePath)
{
}

CModelMesh::CModelMesh(const CModelMesh& other) :
	CMesh(other),
	m_pFilePath(other.m_pFilePath),
	m_pTexturePath(other.m_pTexturePath)
{
	m_vecMaterials.reserve(other.m_vecMaterials.size());
	for (auto& p : other.m_vecMaterials)
	{
		m_vecMaterials.emplace_back(p);
	}

	m_vecTextures.reserve(other.m_vecTextures.size());
	for (auto& p : other.m_vecTextures)
	{
		Safe_AddRef(p);
		m_vecTextures.emplace_back(p);
	}
}

HRESULT CModelMesh::Ready_Component_Prototype()
{
	CMesh::Ready_Component_Prototype();

	// Load Mesh Data
	ID3DXBuffer* adjBuffer = 0;
	ID3DXBuffer* mtrlBuffer = 0;
	DWORD        numMtrls = 0;

	HRESULT hr = 0;
	//"bigship1.x"
	hr = D3DXLoadMeshFromX(
		m_pFilePath,
		D3DXMESH_MANAGED,
		m_pDevice,
		&adjBuffer,
		&mtrlBuffer,
		0,
		&numMtrls,
		&m_pMesh);

	if (FAILED(hr))
	{
		PRINT_LOG(L"Error", L"D3DXLoadMeshFromX() - FAILED");
		return E_FAIL;
	}

	// 재질 정보 로드
	if (mtrlBuffer != 0 && numMtrls != 0)
	{
		D3DXMATERIAL* mtrls = (D3DXMATERIAL*)mtrlBuffer->GetBufferPointer();
		for (size_t i = 0; i < (size_t)numMtrls; ++i)
		{
			// MatD3D 속성은 로드될때 ambient값을 가지지 않으므로
			// 지금 이를 지정한다.
			mtrls[i].MatD3D.Ambient = mtrls[i].MatD3D.Diffuse;

			// i 번째 재질을 저장한다.
			m_vecMaterials.emplace_back(mtrls[i].MatD3D);

			// i 번째 재질에 연결된 텍스쳐가 있는지를 확인한다.
			if (mtrls[i].pTextureFilename != 0)
			{
				// 만약 있다면, i번째 서브셋을 위한 텍스쳐를 읽어들인다.
				wstring wstrTextureFileName = L"";
				string strTextureFileName = mtrls[i].pTextureFilename;
				wstrTextureFileName.assign(strTextureFileName.begin(), strTextureFileName.end());

				// 절대경로
				wstring fullPath = m_pTexturePath + wstrTextureFileName;

				IDirect3DTexture9* tex = 0;
				D3DXCreateTextureFromFile(
					m_pDevice,
					fullPath.c_str(),
					&tex);

				// save the loaded texture
				m_vecTextures.emplace_back(tex);
			}
			else
			{
				// no texture for the ith subset
				m_vecTextures.emplace_back(nullptr);
			}
		}
	}
	Safe_Release(mtrlBuffer); // done w/ buffer

	// 메시 순서 재정리
	hr = m_pMesh->OptimizeInplace(
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)adjBuffer->GetBufferPointer(),
		0, 0, 0);
	Safe_Release(adjBuffer); // done w/ buffer

	if (FAILED(hr))
	{
		PRINT_LOG(L"Error", L"OptimizeInplace() - FAILED");
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CModelMesh::Ready_Component(void* pArg)
{
	CComponent::Ready_Component(pArg);

	return S_OK;
}

_uint CModelMesh::Render_Mesh()
{
	for (size_t i = 0; i < m_vecMaterials.size(); i++)
	{
		m_pDevice->SetMaterial(&m_vecMaterials[i]);
		//D3DMATERIAL9 mat;
		//mat.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		//mat.Ambient = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		//mat.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		//mat.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		//mat.Power = 5.f;
		//m_pDevice->SetMaterial(&mat);
		
		if(nullptr != m_vecTextures[i])
			m_pDevice->SetTexture(0, m_vecTextures[i]);
		m_pMesh->DrawSubset(i);
	}

	return _uint();
}

CModelMesh* CModelMesh::Create(LPDIRECT3DDEVICE9 pDevice, const TCHAR* pFilePath, const TCHAR* pTexturePath)
{
	CModelMesh* pInstance = new CModelMesh(pDevice, pFilePath, pTexturePath);
	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Texture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModelMesh::Clone(void* pArg)
{
	CModelMesh* pClone = new CModelMesh(*this);
	if (FAILED(pClone->Ready_Component(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone CModelMesh");
		Safe_Release(pClone);
	}

	return pClone;
}

void CModelMesh::Free()
{
	CMesh::Free();

	m_vecMaterials.clear();

	for (auto& p : m_vecTextures)
	{
		Safe_Release(p);
	}
	m_vecTextures.clear();
}
