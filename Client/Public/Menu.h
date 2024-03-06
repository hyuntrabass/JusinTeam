#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Wearable_Slot.h"
#include "GraphicSetting.h"
#include "SoundSetting.h"

BEGIN(Client)
class CMenu final : public COrthographicObject
{
public:
	enum MENU { ENV, MENU_END };
	enum ENV_SLOT {GRAPHIC, SOUND,ENV_END };

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
	ENV_SLOT									m_eCurSlot{};
	ENV_SLOT									m_ePrevSlot{};

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
	class CTextButtonColor*						m_pGameEndButton{nullptr};
	CGameObject*								m_pMenu[MENU_END]{};
	CTextButtonColor*							m_pSlots[ENV_END]{};
	class CGraphicSetting*						m_pGraphicSettings[CGraphicSetting::LIST_END]{};
	class CSoundSetting*						m_pSoundSettings[CSoundSetting::LIST_END]{};


public:

private:
	void Init_State();
	void Tick_GraphicSlot(_float fTimeDelta);
	void Tick_SoundSlot(_float fTimeDelta);

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