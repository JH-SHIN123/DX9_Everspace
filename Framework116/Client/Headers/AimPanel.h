#pragma once

#ifndef __AIMPANEL_H__

USING(Engine)
class CAimPanel final : public CUI
{
public:
	explicit CAimPanel(LPDIRECT3DDEVICE9 pDevice);
	explicit CAimPanel(const CAimPanel& other);
	virtual ~CAimPanel() = default;

public:
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;

public:
	static CAimPanel* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;

private:
	void Rotate_AimPanel(const _float fDeltaTime);

private:
	_float m_fRotateAngleZ = 0.f;
};

#define __AIMPANEL_H__
#endif __AIMPANEL_H__