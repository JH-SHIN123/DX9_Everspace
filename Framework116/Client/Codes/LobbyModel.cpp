#include "stdafx.h"
#include "..\Headers\LobbyModel.h"
#include "Collision.h"
#include"Player.h"

CLobbyModel::CLobbyModel(LPDIRECT3DDEVICE9 pDevice)
	: CGameObject(pDevice)
{
	
	
}

CLobbyModel::CLobbyModel(const CLobbyModel & other)
	: CGameObject(other)
{
}

HRESULT CLobbyModel::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return S_OK;
}

HRESULT CLobbyModel::Ready_GameObject(void * pArg/* = nullptr*/)
{
	CGameObject::Ready_GameObject(pArg);
	//Load_Model
	if (FAILED(Load_Model(L"../../Resources/PrototypeData/StaticPlayer.object")))
	{
		PRINT_LOG(L"Error", L"Failed To Load PathData");
		return E_FAIL;
	}
	// For.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,

		m_pPassData->vecPrototypeTag_Mesh[0],
		L"Com_Mesh",
		(CComponent**)&m_pMesh)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Mesh");
		return E_FAIL;
	}

	// For.Com_Transform Test
	TRANSFORM_DESC TransformDesc;
	TransformDesc.fSpeedPerSec = 45.f;
	TransformDesc.vPosition = _float3(0.f, 3.f, 0.f);
	TransformDesc.fSpeedPerSec = 25.f;
	TransformDesc.fRotatePerSec = D3DXToRadian(90.f);
	TransformDesc.vScale = { 0.01f,0.01f,0.01f };

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		m_pPassData->vecPrototypeTag_Mesh[1],
		L"Com_Transform",
		(CComponent**)&m_pTransform,
		&TransformDesc)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	// For.Com_Collide
	BOUNDING_SPHERE BoundingSphere;
	BoundingSphere.fRadius = 5.f;

	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		m_pPassData->vecPrototypeTag_Mesh[2],
		L"Com_CollideSphere",
		(CComponent**)&m_pCollide,
		&BoundingSphere,
		true)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Transform");
		return E_FAIL;
	}

	// For.Com_Controller
	if (FAILED(CGameObject::Add_Component(
		EResourceType::Static,
		m_pPassData->vecPrototypeTag_Mesh[3],
		L"Com_Controller",
		(CComponent**)&m_pController)))
	{
		PRINT_LOG(L"Error", L"Failed To Add_Component Com_Controller");
		return E_FAIL;
	}

	return S_OK;
}

_uint CLobbyModel::Update_GameObject(_float fDeltaTime)
{
	CGameObject::Update_GameObject(fDeltaTime);

	KeyProcess(fDeltaTime);
	Movement(fDeltaTime);
	

	m_pTransform->Update_Transform();
	m_pCollide->Update_Collide(m_pTransform->Get_TransformDesc().matWorld);
	return NO_EVENT;
}

_uint CLobbyModel::LateUpdate_GameObject(_float fDeltaTime)
{
	CGameObject::LateUpdate_GameObject(fDeltaTime);

	if (FAILED(m_pManagement->Add_GameObject_InRenderer(ERenderType::NonAlpha, this)))
		return UPDATE_ERROR;

	return _uint();
}

_uint CLobbyModel::Render_GameObject()
{
	CGameObject::Render_GameObject();

	m_pDevice->SetTransform(D3DTS_WORLD, &m_pTransform->Get_TransformDesc().matWorld);
	m_pMesh->Render_Mesh();

#ifdef _DEBUG // Render Collide
	/*m_pCollide->Render_Collide();*/
#endif

	return _uint();
}

void CLobbyModel::KeyProcess(_float fDeltaTime)
{
	if (nullptr == m_pController) return;
	m_pController->Update_Controller();
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		m_pTransform->RotateZ(D3DXToRadian(45.f)*fDeltaTime);
	}
	else if (GetAsyncKeyState('E') & 0x8000)
	{
		m_pTransform->RotateZ(-D3DXToRadian(45.f)*fDeltaTime);
	}
	else
		m_pTransform->RotateY(D3DXToRadian(30.f)*fDeltaTime);

}

_uint CLobbyModel::Movement(_float fDeltaTime)
{

	return _uint();
}

HRESULT CLobbyModel::Load_Model(wstring wstrObjectPrototypePath)
{
	wifstream fin;
	fin.open(wstrObjectPrototypePath);

	if (fin.fail())
	{
		wstring wstrErrorLog = wstrObjectPrototypePath + L" is wrong path";
		PRINT_LOG(L"Error", wstrErrorLog.c_str());
		return E_FAIL;
	}

	// 오브젝트 프로토타입
	TCHAR szObjectProtoTypeTag[MAX_PATH] = L"";
	// 구별 할 클래스명
	TCHAR szObjectClassName[MAX_PATH] = L"";
	// 머테리얼 정보
	TCHAR szMaterial_Diffuse_r[MAX_PATH] = L"";
	TCHAR szMaterial_Diffuse_g[MAX_PATH] = L"";
	TCHAR szMaterial_Diffuse_b[MAX_PATH] = L"";
	TCHAR szMaterial_Diffuse_a[MAX_PATH] = L"";

	TCHAR szMaterial_Ambient_r[MAX_PATH] = L"";
	TCHAR szMaterial_Ambient_g[MAX_PATH] = L"";
	TCHAR szMaterial_Ambient_b[MAX_PATH] = L"";
	TCHAR szMaterial_Ambient_a[MAX_PATH] = L"";

	TCHAR szMaterial_Specular_r[MAX_PATH] = L"";
	TCHAR szMaterial_Specular_g[MAX_PATH] = L"";
	TCHAR szMaterial_Specular_b[MAX_PATH] = L"";
	TCHAR szMaterial_Specular_a[MAX_PATH] = L"";

	TCHAR szMaterial_Emissive_r[MAX_PATH] = L"";
	TCHAR szMaterial_Emissive_g[MAX_PATH] = L"";
	TCHAR szMaterial_Emissive_b[MAX_PATH] = L"";
	TCHAR szMaterial_Emissive_a[MAX_PATH] = L"";

	TCHAR szMaterial_Power[MAX_PATH] = L"";
	// 컴포넌트의 개수
	TCHAR szComponentTag_Count[MAX_PATH] = L"";
	// 읽을 컴포넌트
	TCHAR szComponentTag[MAX_PATH] = L"";

	while (true)
	{
		PASSDATA_OBJECT* pData = new PASSDATA_OBJECT;
		pData->wstrPrototypeTag_Object = L"";
		ZeroMemory(&pData->tMaterial, sizeof(D3DMATERIAL9));
		pData->vecPrototypeTag_Mesh.reserve(10);

		fin.getline(szObjectProtoTypeTag, MAX_PATH, L'(');		// PrototypeTag
		fin.getline(szObjectClassName, MAX_PATH, L')');			// ClassName

		if (fin.eof())
		{
			delete pData;
			pData = nullptr;
			break;
		}

		fin.getline(szMaterial_Diffuse_r, MAX_PATH, L'?');
		fin.getline(szMaterial_Diffuse_g, MAX_PATH, L'?');
		fin.getline(szMaterial_Diffuse_b, MAX_PATH, L'?');
		fin.getline(szMaterial_Diffuse_a, MAX_PATH, L'?');

		fin.getline(szMaterial_Ambient_r, MAX_PATH, L'?');
		fin.getline(szMaterial_Ambient_g, MAX_PATH, L'?');
		fin.getline(szMaterial_Ambient_b, MAX_PATH, L'?');
		fin.getline(szMaterial_Ambient_a, MAX_PATH, L'?');

		fin.getline(szMaterial_Specular_r, MAX_PATH, L'?');
		fin.getline(szMaterial_Specular_g, MAX_PATH, L'?');
		fin.getline(szMaterial_Specular_b, MAX_PATH, L'?');
		fin.getline(szMaterial_Specular_a, MAX_PATH, L'?');

		fin.getline(szMaterial_Emissive_r, MAX_PATH, L'?');
		fin.getline(szMaterial_Emissive_g, MAX_PATH, L'?');
		fin.getline(szMaterial_Emissive_b, MAX_PATH, L'?');
		fin.getline(szMaterial_Emissive_a, MAX_PATH, L'?');

		fin.getline(szMaterial_Power, MAX_PATH, L'?');

		// 읽은 값 삽입
		pData->wstrPrototypeTag_Object = szObjectProtoTypeTag;

		pData->tMaterial.Diffuse.r = (_float)_ttof(szMaterial_Diffuse_r);
		pData->tMaterial.Diffuse.g = (_float)_ttof(szMaterial_Diffuse_g);
		pData->tMaterial.Diffuse.b = (_float)_ttof(szMaterial_Diffuse_b);
		pData->tMaterial.Diffuse.a = (_float)_ttof(szMaterial_Diffuse_a);

		pData->tMaterial.Ambient.r = (_float)_ttof(szMaterial_Ambient_r);
		pData->tMaterial.Ambient.g = (_float)_ttof(szMaterial_Ambient_g);
		pData->tMaterial.Ambient.b = (_float)_ttof(szMaterial_Ambient_b);
		pData->tMaterial.Ambient.a = (_float)_ttof(szMaterial_Ambient_a);

		pData->tMaterial.Specular.r = (_float)_ttof(szMaterial_Specular_r);
		pData->tMaterial.Specular.g = (_float)_ttof(szMaterial_Specular_g);
		pData->tMaterial.Specular.b = (_float)_ttof(szMaterial_Specular_b);
		pData->tMaterial.Specular.a = (_float)_ttof(szMaterial_Specular_a);

		pData->tMaterial.Emissive.r = (_float)_ttof(szMaterial_Emissive_r);
		pData->tMaterial.Emissive.g = (_float)_ttof(szMaterial_Emissive_g);
		pData->tMaterial.Emissive.b = (_float)_ttof(szMaterial_Emissive_b);
		pData->tMaterial.Emissive.a = (_float)_ttof(szMaterial_Emissive_a);

		pData->tMaterial.Power = (_float)_ttof(szMaterial_Power);


		// 카운트 만큼 읽어서 삽입
		fin.getline(szComponentTag_Count, MAX_PATH, L'|');
		_uint iComponentTag_Count = _ttoi(szComponentTag_Count);

		for (_uint i = 0; i < iComponentTag_Count; ++i)
		{
			fin.getline(szComponentTag, MAX_PATH, L'|');
			pData->vecPrototypeTag_Mesh.emplace_back(szComponentTag);
		}
		m_pPassData = pData;
	}
	
	fin.close();

}

CLobbyModel * CLobbyModel::Create(LPDIRECT3DDEVICE9 pDevice)
{
	CLobbyModel* pInstance = new CLobbyModel(pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		PRINT_LOG(L"Error", L"Failed To Create Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLobbyModel::Clone(void * pArg/* = nullptr*/)
{
	CLobbyModel* pClone = new CLobbyModel(*this); /* 복사 생성자 호출 */
	if (FAILED(pClone->Ready_GameObject(pArg)))
	{
		PRINT_LOG(L"Error", L"Failed To Clone Player");
		Safe_Release(pClone);
	}

	return pClone;
}

void CLobbyModel::Free()
{
	Safe_Release(m_pPlayer);
	Safe_Release(m_pMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollide);
	Safe_Release(m_pController);

	CGameObject::Free();
}
