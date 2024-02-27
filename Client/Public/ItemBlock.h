#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "ItemSlot.h"

BEGIN(Client)
class CItemSlot;
class CItemBlock final : public CGameObject
{
public:
	enum ITEMSLOT { ITEM1, ITEM2, ITEM3, ITEM4, ITEMSLOT_END };

private:
	CItemBlock(_dev pDevice, _context pContext);
	CItemBlock(const CItemBlock& rhs);
	virtual ~CItemBlock() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:

	_bool			m_isPrototype{ false };
	_bool			m_isCoolTime{ false };
	_uint			m_iSelect{};

	_float			m_fTime{};
	CItemSlot* m_pSlots[ITEMSLOT_END]{};

	class CTextButton*	m_pSelect{ nullptr };
	class CTextButtonColor*	m_pMask{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Add_Slots();

public:
	static CItemBlock* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END