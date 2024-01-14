#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "CharacterSelect.h"
#include "TextButton.h"
#include "SelectDesc.h"
#include "TextButtonColor.h"

BEGIN(Client)
class CTextButton;
class CCharacterSelect;
class CTextButtonColor;
class CSelect final : public CGameObject
{
private:
	CSelect(_dev pDevice, _context pContext);
	CSelect(const CSelect& rhs);
	virtual ~CSelect() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Bind_ShaderResources();

private:
	_bool				m_bShutDown{ false };
	_bool				m_bShow{ false };
	_float				m_fAlpha{ 0.f };
	_float				m_fDuration{ 0.f };
	CTextButton*		m_pClassButton{ nullptr };
	CSelectDesc*		m_pSelectDesc{ nullptr };
	CCharacterSelect*	m_pCharacterSelect{ nullptr };
	CTextButtonColor*	m_pSelectButton{ nullptr };
	CTextButtonColor*	m_pBackButton{ nullptr };

private:
	void Set_SelectDesc(_uint iSelect);

public:
	static CSelect* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END