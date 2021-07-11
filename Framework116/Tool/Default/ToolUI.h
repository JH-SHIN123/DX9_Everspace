#pragma once
#include"../../Reference/Headers/UI.h"
USING(Engine)
class CToolUI final : public CUI
{

protected:
	explicit CToolUI(LPDIRECT3DDEVICE9 pDevice);
	explicit CToolUI(const CUI& other);
	virtual ~CToolUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void* pArg = nullptr) override;
	virtual _uint Update_GameObject(_float fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(_float fDeltaTime) override;
	virtual _uint Render_GameObject() override;
	void Set_UiTool(class CUiTool* _pUiTool) { m_pUiTool = _pUiTool; }
	const wstring& Get_TexturePrototypeTag() const{ return m_wstrTexturePrototypeTag; }
public:
	static CToolUI* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

	void Set_Dead(){m_bDead = true;}
	class CUiTool* m_pUiTool;
private:
	_bool m_bDead =false;

};

