#pragma once
#ifndef __GAMEOBJECT_H__

#include "Base.h"
#include "Transform.h"

BEGIN(Engine)

typedef struct tagGameObjectDesc : public BASE_DESC
{
	TRANSFORM_DESC	tTransformDesc;
	wstring	wstrMeshName = L"";
}GAMEOBJECT_DESC;

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pDevice);
	explicit CGameObject(const CGameObject& other);
	virtual ~CGameObject() = default;

public:
	const class CComponent* Get_Component(const wstring& ComponentTag) const;
	const vector<class CCollide*>* Get_Collides() const { return &m_Collides; };
	const _bool Get_IsEmptyCollides() const;
	const _bool Get_IsPicking() const { return m_IsPicking; }
	const _bool Get_IsDead() const { return m_IsDead; }
	const _bool Get_IsCollide() const { return m_IsCollide; }
	const _float Get_HP() const { return m_fHp;}
	

public:
	void Set_IsPicking(const _bool _isPicking) { m_IsPicking = _isPicking; };
	void Set_IsDead(const _bool _isDead) { m_IsDead = _isDead; }
	void Set_IsCollide(const _bool _isCollide) { m_IsCollide = _isCollide; }
	void Set_Damage(const _float _fDamage) { m_fHp -= _fDamage; }
	void Set_FullHp(const _float _fFullHp) { m_fFullHp = _fFullHp; }
	void Set_Hp(const _float _fHp) { m_fHp = _fHp; }

public:
	virtual HRESULT Ready_GameObject_Prototype() = 0;	/* 프로토타입 초기화 */
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) = 0; /* 클론 초기화 */
	virtual _uint Update_GameObject(_float fDeltaTime) = 0;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) = 0;
	virtual _uint Render_GameObject() = 0;

protected:
	HRESULT Add_Component(EResourceType eType, const wstring& PrototypeTag, const wstring& ComponentTag, class CComponent** ppComponent = nullptr, 
		void* pArg = nullptr, const bool _bCollide = false);

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() = 0;

protected:
	_bool m_IsDead = false;
	_bool m_IsCollide = false;
	_float m_fHp = 0.f;
	_float m_fFullHp = 0.f;

protected:
	LPDIRECT3DDEVICE9 m_pDevice = nullptr;
	_bool m_IsClone = false;
	_bool m_IsPicking = false;

	typedef unordered_map<wstring, class CComponent*>	COMPONENTS;
	COMPONENTS m_Components; /* 클론 보관 */

	typedef vector<class CCollide*>	COLLIDES;
	COLLIDES m_Collides;

	class CManagement* m_pManagement = nullptr;

///////////////////////////////////////////////////////////
// Tool 전용 함수 / 변수들
public:
	void Set_ListBoxIndex(const int _iListBoxIndex) { m_iListBoxIndex = _iListBoxIndex; }
	const int Get_ListBoxIndex() const { return m_iListBoxIndex; }

protected: 
	int m_iListBoxIndex = -1;
};
END

#define __GAMEOBJECT_H__
#endif