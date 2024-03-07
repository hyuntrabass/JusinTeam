#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Riding.h"
#include "BrickItem.h"
#define BRICKCOL 11
#define BRICKROW 5

BEGIN(Client)
class CBrickGame final : public COrthographicObject
{
private:
	CBrickGame(_dev pDevice, _context pContext);
	CBrickGame(const CBrickGame& rhs);
	virtual ~CBrickGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	_bool										m_isGameOver{ false };
	_bool										m_isPicking{ false };
	_bool										m_isPrototype{ false };
	_bool										m_isActive{ false };
	_bool										m_isReadyItem{ false };

	_uint										m_iCombo{};
	_uint										m_iCurIndex{};
	_int										m_iSoundChannel = -1;

	_float										m_fStopTime{};

	_float										m_fTime{};
	_float										m_fDir{1.f};
	_uint										m_iMinute{3};
	_uint										m_iSec{60};
	_float										m_fComboTime{};
	_float										m_fTimeLimit{};
	_mat										m_EffectMatrix{};

	LIGHT_DESC									m_Light_Desc{};
	CGameObject*								m_pBackGround{nullptr};
	class CNumEffect*							m_pCombo{ nullptr };

	class CBrickBall*							m_pBall{ nullptr };
	class CBrickBar*							m_pBar{ nullptr };

	class CTextButtonColor*						m_pTimeBar{ nullptr };
	CTextButtonColor*							m_pTimeBarLock{ nullptr };
	class CBlackCat*							m_pCatBoss{ nullptr };

	CTextButtonColor*							m_pItemSlot{ nullptr };
	CTextButtonColor*							m_pItemSlotEffect{ nullptr };
	CTextButtonColor*							m_pItem[CBrickItem::TYPE_END]{ nullptr };

	list<CBrickItem::TYPE>						m_ITemList;

public:
	class CComponent* Find_Component(const wstring& strComTag) override;

private:
	void Tick_Ball(_float fTimeDelta);

private:
	void Init_Game();
	void Create_Bricks();

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBrickGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END