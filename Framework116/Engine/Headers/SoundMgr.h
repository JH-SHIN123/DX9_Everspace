#pragma once
#ifndef __SOUNDMGR_H__
#include "Base.h"

BEGIN(Engine)
class CSoundMgr : public CBase
{
	DECLARE_SINGLETON(CSoundMgr)
public:
	CSoundMgr();
	virtual ~CSoundMgr() = default;

public:
	enum CHANNELID
	{ 
		BGM, PLAYER_MOVE, PLAYER_BOOST, PLAYER_WEAPON , PLAYER_MISSILE_EXPLOSION, PLAYER_SKILL, PLAYER_GATLING, LOCKON,
		LOBBY_EFFECT,
		SHIP_EXPLOSION, 
		LOCKON_ALERT,
		DIALOGUE1, DIALOGUE2,
		RINGCOLLISION, SCRIPT_POPUP, UI, MAXCHANNEL,
		BOSS_ATTACK_1, BOSS_ATTACK_2, BOSS_ATTACK_3, BULLET_EMP,
		LOADING
	};

public:
	virtual void Free() override;

public:
	HRESULT Ready_SoundManager();

public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID);
	void PlayBGM(TCHAR* pSoundKey);
	void StopSound(CHANNELID eID);
	void StopAll();


private:
	void LoadSoundFile();

private:
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
};
END
#define __SOUNDMGR_H__
#endif