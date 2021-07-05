#pragma once

#ifndef __EFFECTHANDLER_H__

USING(Engine)
class CEffectHandler final
{
private:
	CEffectHandler() = default;
	~CEffectHandler() = default;

public:
	static HRESULT Add_Layer_Effect_Explosion(const _float3& _vPos, const _float _fSize);
};

#define __EFFECTHANDLER_H__
#endif // !__EFFECTHANDLER_H__