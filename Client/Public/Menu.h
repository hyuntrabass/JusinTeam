#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"

BEGIN(Client)
class CWearable_Slot;
class CMenu final : public COrthographicObject
{
public:
	enum MENU { SOUND, GRAPHIC, MENU_END };

private:
	CMenu(_dev pDevice, _context pContext);
	CMenu(const CMenu& rhs);
	virtual ~CMenu() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	_bool										m_Clicked{ false };
	_bool										m_isReadytoDeactivate{ false };
	_bool										m_isReadytoActivate{ false };
	_bool										m_isPrototype{ false };
	_bool										m_isActive{ false };

	CGameObject*								m_pTitleButton{ nullptr };
	CGameObject*								m_pExitButton{ nullptr };
	CGameObject*								m_pBackGround{ nullptr };
	CGameObject*								m_pSetting{ nullptr };
	CGameObject*								m_pUnderBar{ nullptr };
	CGameObject*								m_pSelectButton{ nullptr };
	CGameObject*								m_pMenu[MENU_END];


public:

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMenu* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END