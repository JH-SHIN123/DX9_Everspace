#pragma once
#ifndef __PLAYER_H__

#include "GameObject.h"

USING(Engine)
class CPlayer final : public CGameObject
{
public:
	explicit CPlayer(LPDIRECT3DDEVICE9 pDevice);
	explicit CPlayer(const CPlayer& other);
	virtual ~CPlayer() = default;

public:
	HRESULT ChangeMesh(const wstring& wstrMeshPrototypeTag);
	const wstring& Get_MeshPrototypeTag() const { return m_wstrMeshPrototypeTag; }

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

private:
	_uint Movement(_float fDeltaTime);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	class CMesh*  m_pMesh = nullptr;
	wstring m_wstrMeshPrototypeTag = L"";
	//class CGeometryMesh* m_pGeoMesh = nullptr;
	CTransform* m_pTransform = nullptr;

private:
	POINT m_tCurCursorPos = { 0,0 };
	POINT m_tPrevCursorPos = { 0,0 };

};

#define __PLAYER_H__
#endif