#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "ItemBlock.h"
#include "InvenFrame.h"
#include "SkillBlock.h"
BEGIN(Engine)
class CGameInstance;
END
BEGIN(Client)
class CItemBlock;
class CSkillBlock;
class CInvenFrame;
class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)
public:
	enum TYPE { MONSTER, NPC, TYPE_END };
	enum MOUSESTATE { M_DEFAULT, M_TEXT, M_GRAB, MOUSESTATE_END };
private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

private:
	PART_TYPE		m_eChangedPart{ PT_END };
	MOUSESTATE		m_eMouseState { M_DEFAULT };
	WEAPON_TYPE		m_eWeaponType{ WP_BOW };

	_bool			m_isSetSkillSlot{ false };
	_bool			m_isPicking{ false };
	_bool			m_isShowing{ false };
	_bool			m_isInvenActive{ false };
	_bool			m_isSetInvenState{ false };
	_bool			m_bTimeStop{ false };


	_uint			m_iCoin{};
	_uint			m_iDiamond{};
	_uint			m_CustomPart[PART_TYPE::PT_END];

	_float2			m_fExp{0.f, 1000.f};

	_vec2			m_vHp{1000.f, 1000.f};
	_vec2			m_vMp{1000.f, 1000.f};

	_vec4			m_vInvenPos{0.f, 1000.f, 0.f, 1.f};
	_vec4			m_vPlayerPos{0.f, 0.f, 0.f, 0.f};
	_vec4			m_vCameraPos{0.f, 0.f, 0.f, 0.f};
	_vec4			m_vHairColor{0.f, 0.f, 0.f, 0.f};
	
	map<const wstring, ITEM> m_mapItem;

	CGameInstance*	m_pGameInstance{ nullptr };
	CGameObject*	m_pSkillSlots[WP_END][CSkillBlock::SKILL_END];
	CGameObject*	m_pItemSlots[CItemBlock::ITEMSLOT_END];
	CGameObject*	m_pInvenItemSlots[CItemBlock::ITEMSLOT_END];
	CGameObject*	m_pInven{ nullptr };
	CGameObject*	m_pInvenFrame{ nullptr };

	vector<CTransform*> m_vecRadarPos[TYPE_END];
	SKILLINFO		m_SkillInfo[WP_END][4];

public:
	HRESULT Init();
	/*
	
	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);
	HRESULT Render();
	*/

private:
	HRESULT Init_Items();
	HRESULT Init_Skills();


public:
	SKILLINFO Get_SkillInfo(WEAPON_TYPE eType, _uint iIdx);
	void Set_TimeStop(_bool bStop) { m_bTimeStop = bStop; }
	_bool Get_TimeStop() { return m_bTimeStop; }

	ITEM Find_Item(wstring& strItemName);

	HRESULT Set_Item(wstring& strItemName, _uint iNum = 1);
	HRESULT Set_Inven(CGameObject* pGameObject);
	HRESULT Set_InvenFrame(CGameObject* pGameObject);
	HRESULT Set_ItemSlots(CItemBlock::ITEMSLOT eSlot, CGameObject* pGameObject);
	HRESULT Set_InvenItemSlots(CItemBlock::ITEMSLOT eSlot, CGameObject* pGameObject);
	HRESULT Set_SkillBookSlots(WEAPON_TYPE eType, CSkillBlock::SKILLSLOT eSlot, CGameObject* pGameObject);

	CGameObject* Get_InvenFrame();
	SKILLINFO Get_SkillSlot(WEAPON_TYPE eType, CSkillBlock::SKILLSLOT eSlot);
	CGameObject* Get_ItemSlots(CItemBlock::ITEMSLOT eSlot);
	_bool Is_ItemSlotFull(CItemBlock::ITEMSLOT eSlot);
	HRESULT Set_Item_In_EmptySlot(CItemBlock::ITEMSLOT eSlot, CItem* pItem, _int* iItemNum);
	ITEM Set_Item_In_FullSlot(CItemBlock::ITEMSLOT eSlot, CItem* pItem, _int* iItemNum, _int* iChangeItemNum = nullptr);
	void Delete_Item_In_Slot(CItemBlock::ITEMSLOT eSlot);

	void Set_RadarPos(TYPE eType, CTransform* pTransform);
	void Delete_RadarPos(TYPE eType, CTransform* pTransform);
	const vector<CTransform*> Get_RadarPosition(TYPE eType) const;

	HRESULT Set_CustomPart(PART_TYPE eType, _uint iIndex);
	HRESULT Set_WeaponType(WEAPON_TYPE eWpType);
	void Set_HairColor(_vec4 vColor) { m_vHairColor = vColor; }
	void Set_Picking_UI(_bool isPicking) { m_isPicking = isPicking; }
	void Set_FullScreenUI(_bool isShowing) { m_isShowing = isShowing; }
	void Set_InvenActive(_bool isInvenActive) { if (m_isInvenActive && !isInvenActive) { m_isSetInvenState = false; } m_isInvenActive = isInvenActive;  }
	void Set_Exp_ByPercent(_float fExp);
	HRESULT Set_Coin(_int iCoin);
	HRESULT Set_Diamond(_uint iDia);
	_bool Set_CurrentPlayerPos(_vec4 vPos);
	void Set_MouseState(MOUSESTATE eState) { m_eMouseState = eState; }
	void Set_Hp(_int iHp, _int iMaxHp) { m_vHp = _vec2((_float)iHp, (_float)iMaxHp); }
	void Set_Mp(_int iMp, _int iMaxMp) { m_vMp = _vec2((_float)iMp, (_float)iMaxMp); }


	void Set_SkillSlotChange(_bool isSkillSlotChange) { m_isSetSkillSlot = isSkillSlotChange; }
	_bool Is_SkillSlotChange() { return m_isSetSkillSlot; }

	const _vec2& Get_Hp() const { return m_vHp; }
	const _vec2& Get_Mp() const { return m_vMp; }

	const _vec4& Get_HairColor() const { return m_vHairColor; }
	const _vec4& Get_InvenPos() const { return m_vInvenPos; }
	const _vec4& Get_LastPlayerPos() const { return m_vPlayerPos; }
	const _uint& Get_CustomPart (PART_TYPE eType);
	const _uint& Get_WeaponType(PART_TYPE eType, WEAPON_TYPE* wpType);
	const _uint& Get_Coin() const { return m_iCoin; }
	const _uint& Get_Diamond() const { return m_iDiamond; }
	const _float2& Get_Exp() const { return m_fExp; }
	const MOUSESTATE& Get_MouseState() const { return m_eMouseState; }

	const PART_TYPE& Is_CustomPartChanged() const { return m_eChangedPart; }
	const _bool& Is_Picking_UI() const { return m_isPicking; }
	const _bool& Showing_FullScreenUI() const { return m_isShowing; }
	const _bool& Is_InvenActive() const { return m_isInvenActive; }

	void Level_Up();
public:
	virtual void Free() override;
};

END
