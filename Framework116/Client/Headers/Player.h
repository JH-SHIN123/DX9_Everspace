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

private:
	void Update_Effect();

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	_uint Set_IsScript(_bool IsScript);
	_uint Set_IsCameraMove(_bool IsCameraMove);

private:
	void	KeyProcess(_float fDeltaTime);
	_uint	Movement(_float fDeltaTime);
	void	TimeOperation(const _float fDeltaTime);
	void	Increase_Stamina(const _float fDeltaTime);
	
	_uint	Collide_Planet_Or_Astroid(const _float fDeltaTime);

public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject * Clone(void * pArg = nullptr) override;
	virtual void Free() override;

private:
	_bool m_IsBoost = false;

private: // For.Components
	CMesh*  m_pMesh = nullptr;
	CTransform* m_pTransform = nullptr;
	CController* m_pController = nullptr;

	//CCamera* m_pCam = nullptr;
	//_float m_fCameraDist = 5.f;

private: // 플레이어가 사용하실 변수님들.

	// 발사속도
	_float m_fMachinegunFireDelay = 0.f;

	//총열돌리는 시간
	_float m_fMachinegunDelay = 0.f;
	_float m_fLaserDelay = 0.f;
	_float m_fMissileDelay = 0.f;
	_bool m_IsLeft = false;

	// Player Skill
	_float m_fOverDriveTime = 20.f; // 20초 타이머
	_float m_fOverDrive = 1.f; // OverDrive 활성화 시엔 2.f;
	_bool m_bOverDrive = false;

	// enum 사용 권장
	//무기 1번 : 기관총, 2번 : 레이저
	_int m_iWeapon = 1;
	_bool m_IsLazer = false;
	_bool m_IsMissile = false;

private: // Engine Effect
	_float3 m_vLeftEngineOffset = { 0.f, 0.f, 0.f };
	_float3 m_vRightEngineOffset = { 0.f, 0.f, 0.f };
	class CEngineEffectSystem* m_pLeftEngineEffect = nullptr;
	class CEngineEffectSystem* m_pRightEngineEffect = nullptr;

private: // Wing Effect
	_float3 m_vLeftWingOffset = { 0.f, 0.f, 0.f };
	_float3 m_vRightWingOffset = { 0.f, 0.f, 0.f };
	class CWingBoost_System* m_pLeftWingBoost = nullptr;
	class CWingBoost_System* m_pRightWingBoost = nullptr;

private: // AlertArrow 관련
	_uint Make_Arrow();
	const list<class CGameObject*>* m_listCheckMonsters = nullptr;
	_bool IsArrow = false;

private: // HP, 스태미너 관련
	class CHP_Bar* m_pHp_Bar = nullptr;
	_float m_fHpLength = 256.f;

	class CStamina_Bar* m_pStamina_Bar = nullptr;
	_float m_fStamina = 100.f;
	_float m_fFullStamina = 100.f;
	_float m_fStaminaLength = 256.f;
	_bool m_IsStaminaShrink = false;
	_float m_fStaminaIncreaseDelay = 0.f;

private: //연출 관련
	_bool m_IsScript = false; //대화
	_bool m_IsCameraMove = false; // 카메라 연출

};

#define __PLAYER_H__
#endif