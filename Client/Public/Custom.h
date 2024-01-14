#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "TextButton.h"
#include "TextButtonColor.h"

BEGIN(Client)
class CTextButton;
class CTextButtonColor;
class CCustom final : public CGameObject
{
public:
	enum CUSTOM_MENU { C_FACE, C_HAIR, C_END };
private:
	CCustom(_dev pDevice, _context pContext);
	CCustom(const CCustom& rhs);
	virtual ~CCustom() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Add_Models();
	HRESULT Init_Menu();
	HRESULT Bind_ShaderResources();

private:
	_bool				m_isPrototype{ false };
	_bool				m_bShow{ false };

	_float				m_fAlpha{ 0.f };

	CTextButton*		m_pCustomMenu[C_END];

	_bool				m_isMenuClick[C_END];
	_vec2				m_HairPos[9];
	_vec2				m_FacePos[6];

	CTextButton*		m_pFaceGroup{ nullptr };
	CTextButton*		m_pHairGroup{ nullptr };
	CTextButton*		m_pSelectCustomEffect{ nullptr };

	CTextButton*		m_pClassButton{ nullptr };
	CTextButtonColor*	m_pSelectButton{ nullptr }; 
	CTextButton*	m_pSelectMenuEffect{ nullptr };

private:
	void Set_CameraState(_uint iSelect);

public:
	static CCustom* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END