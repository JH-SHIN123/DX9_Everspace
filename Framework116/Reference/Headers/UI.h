#pragma once

#ifndef __UI_H__

#include "GameObject.h"
<<<<<<< HEAD

#include "Transform.h"

BEGIN(Engine)
<<<<<<< HEAD
=======
=======
#include "Transform.h"

BEGIN(Engine)
>>>>>>> e227d5431d91ddb4470e6a2a455a4f2cf20413ea
typedef struct tagUIDesc : public BASE_DESC
{
	TRANSFORM_DESC tTransformDesc;
	wstring wstrTexturePrototypeTag = L"";
}UI_DESC;

<<<<<<< HEAD
=======
>>>>>>> origin/main
>>>>>>> e227d5431d91ddb4470e6a2a455a4f2cf20413ea
class ENGINE_DLL CUI : public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CUI(const CUI& other);
	virtual ~CUI() = default;

public:
<<<<<<< HEAD
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;
=======
<<<<<<< HEAD
	virtual HRESULT Ready_GameObject_Prototype() = 0;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) = 0;
	virtual _uint Update_GameObject(_float fDeltaTime) = 0;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) = 0;
	virtual _uint Render_GameObject() = 0;

protected:
	_uint IsBillboarding();
>>>>>>> e227d5431d91ddb4470e6a2a455a4f2cf20413ea

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

protected:
<<<<<<< HEAD
=======
	// 항상 플레이어를 바라보도록 -> 빌보드
	// 항상 플레이어를 따라다녀야함
	// 맨 앞에 배치 -> Z (1.f 고정)
	// 스케일 변환 X -> (직교투영행렬 곱)
=======
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

protected:
>>>>>>> origin/main
>>>>>>> e227d5431d91ddb4470e6a2a455a4f2cf20413ea
	class CVIBuffer* m_pVIBuffer = nullptr;
	class CTransform* m_pTransform = nullptr;
	class CTexture* m_pTexture = nullptr;

<<<<<<< HEAD
protected:
	wstring m_wstrTexturePrototypeTag = L"";
	RECT m_tUIBounds;
=======
<<<<<<< HEAD
=======
protected:
	wstring m_wstrTexturePrototypeTag = L"";
	RECT m_tUIBounds;
>>>>>>> origin/main
>>>>>>> e227d5431d91ddb4470e6a2a455a4f2cf20413ea
};
END

#define __UI_H__
#endif