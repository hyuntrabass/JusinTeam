#include "UI_Manager.h"
#include "GameInstance.h"
#include "Inven.h"
#include "SkillBook.h"
#include "Event_Manager.h"
IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{

}
HRESULT CUI_Manager::Init()
{
	m_pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(m_pGameInstance);

	for (size_t i = 0; i < PART_TYPE::PT_END; i++)
	{
		m_CustomPart[i] = 0;
	}

	if (FAILED(Init_Items()))
	{
		return E_FAIL;
	}
	if (FAILED(Init_Skills()))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CUI_Manager::Set_Exp_ByPercent(_float fExp)
{

	if (!m_isFirstKill)
	{
		m_isFirstKill = true;
		m_fExp.x = m_fExp.y;
	}
	else
	{
		m_fExp.x += m_fExp.y * fExp / 100.f;
	}

	if (m_fExp.x >= m_fExp.y)
	{
		m_iLevel++;
		CEvent_Manager::Get_Instance()->Set_LevelUp(m_iLevel);
		if (m_iLevel == 2 || m_iLevel == 3 || m_iLevel == 4|| m_iLevel == 5)
		{
			CEvent_Manager::Get_Instance()->Set_SkillUnlock(m_iLevel - 2);
		}
		Level_Up();
		m_fExp.x = 0.f;
		m_fExp.y += 20.f * m_iLevel;
		//스탯바꾸는곳에서 처리하는게 나을듯 레벨업함수에서
	}
}

HRESULT CUI_Manager::Set_Coin(_int iCoin)
{
	if ((m_iCoin += iCoin) && m_iCoin < 0)
	{
		m_iCoin = 0;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Manager::Set_Diamond(_uint iDia)
{
	if ((m_iDiamond += iDia) && m_iDiamond < 0)
	{
		m_iDiamond = 0;
		return E_FAIL;
	}
	return S_OK;
}

_bool CUI_Manager::Set_CurrentPlayerPos(_vec4 vPos)
{
	if (!m_isSetInvenState) 
	{
		m_vPlayerPos = vPos; 
		m_isSetInvenState = true;
		return true; 
	}
	return false;
}

void CUI_Manager::Set_MpState(_bool isMp, _uint iMp)
{
	if (isMp)
	{
		m_isMpState = true;
		m_iMpState = iMp;
	}
	else
	{
		m_isMpState = false;
	}
}

void CUI_Manager::Set_Heal(_bool isHeal, _uint iHeal)
{
	if (isHeal)
	{
		m_isHeal = true;
		m_iHeal = iHeal;
	}
	else
	{
		m_isHeal = false;
	}
}

_bool CUI_Manager::Get_Heal(_uint* iHeal)
{
	if (m_isHeal)
	{
		if (iHeal != nullptr)
		{
			*iHeal = m_iHeal;
		}
		return true;
	}
	return false;
}

_bool CUI_Manager::Get_MpState(_uint* iMp)
{
	if (m_isMpState)
	{
		if (iMp != nullptr)
		{
			*iMp = m_iMpState;
		}
		return true;
	}
	return false;
}


const _uint& CUI_Manager::Get_CustomPart(PART_TYPE eType)
{
	m_eChangedPart = PT_END;
	return m_CustomPart[eType];
}

const _uint& CUI_Manager::Get_WeaponType(PART_TYPE eType, WEAPON_TYPE* wpType)
{
	m_eChangedPart = PT_END;
	*wpType = m_eWeaponType;
	return m_CustomPart[eType];
}

HRESULT CUI_Manager::Set_CustomPart(PART_TYPE eType, _uint iIndex)
{
	m_eChangedPart = eType;
	m_CustomPart[eType] = iIndex;
	return S_OK;
}

HRESULT CUI_Manager::Set_WeaponType(WEAPON_TYPE eWpType)
{
	m_eWeaponType = eWpType;
	return S_OK;
}

HRESULT CUI_Manager::Set_ItemSlots(CItemBlock::ITEMSLOT eSlot, CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
	{
		return E_FAIL;
	}
	m_pItemSlots[eSlot] = pGameObject;

	return S_OK;
}

HRESULT CUI_Manager::Set_InvenItemSlots(CItemBlock::ITEMSLOT eSlot, CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
	{
		return E_FAIL;
	}
	m_pInvenItemSlots[eSlot] = pGameObject;

	return S_OK;
}

HRESULT CUI_Manager::Set_SkillBookSlots(WEAPON_TYPE eType, CSkillBlock::SKILLSLOT eSlot, CSkillSlot* pGameObject)
{
	if (pGameObject == nullptr)
	{
		return E_FAIL;
	}
	m_pSkillSlots[eType][eSlot] = pGameObject;

	return S_OK;
}

HRESULT CUI_Manager::Set_SkillBlock(CSkillBlock* pGameObject)
{
	if (pGameObject == nullptr)
	{
		return E_FAIL;
	}
	m_pSkillBlock = pGameObject;
	return S_OK;
}

HRESULT CUI_Manager::Set_SkillBook(CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
	{
		return E_FAIL;
	}
	m_pSkillBook = pGameObject;
	return S_OK;
}

HRESULT CUI_Manager::Init_Items()
{

	std::locale::global(std::locale(".ACP"));
	wifstream		fin;

	fin.open(L"../Bin/Data/ItemInfo.txt", ios::binary);

	if (fin.fail())
		return E_FAIL;
	//EXPENDABLE|POTION|COMMON|체력 포션|IT_hppotion|0|1000|500|

	wstring		wstrCombined = L"";
	int i = 0;
	while (!fin.eof())
	{

		ITEM Item = {};
		wstring strInvenType;
		wstring strItemType;
		wstring strItemTier;
		wstring strItemIndex;
		wstring strPurchase;
		wstring strSale;
		wstring strStatus;
		wstring strPartIndex;
		wstring strLimited;
		getline(fin, strInvenType, L'|');

		if (i > 0)
		{
			strInvenType.erase(std::remove(strInvenType.begin(), strInvenType.end(), '\r'), strInvenType.end());
			strInvenType.erase(std::remove(strInvenType.begin(), strInvenType.end(), '\n'), strInvenType.end());
		}
		if (strInvenType == TEXT("EXPENDABLE"))
		{
			Item.iInvenType = (_uint)INVEN_EXPENDABLES;
		}
		else if (strInvenType == TEXT("EQUIP"))
		{
			Item.iInvenType = (_uint)INVEN_EQUIP;
		}
		else
		{
			Item.iInvenType = (_uint)INVEN_WEARABLE;
		}

		/* 루프 종료 */
		if (strInvenType == TEXT(""))
			break;

		getline(fin, strItemType, L'|');
		if (strItemType == TEXT("TOP"))
		{
			Item.iItemType = (_uint)ITEM_TOP;
		}
		else if (strItemType == TEXT("BODY"))
		{
			Item.iItemType = (_uint)ITEM_BODY;
		}
		else if (strItemType == TEXT("HAND"))
		{
			Item.iItemType = (_uint)ITEM_HAND;
		}
		else if (strItemType == TEXT("FOOT"))
		{
			Item.iItemType = (_uint)ITEM_FOOT;
		}
		else if (strItemType == TEXT("POTION"))
		{
			Item.iItemType = (_uint)ITEM_POTION;
		}
		else if (strItemType == TEXT("INGREDIENT"))
		{
			Item.iItemType = (_uint)ITEM_INGREDIENT;
		}
		else if (strItemType == TEXT("SWORD"))
		{
			Item.iItemType = (_uint)ITEM_SWORD;
		}
		
		else if (strItemType == TEXT("BOW"))
		{
			Item.iItemType = (_uint)ITEM_BOW;
		}

		getline(fin, strItemTier, L'|');

		if (strItemTier == TEXT("COMMON"))
		{
			Item.iItemTier = (_uint)TIER_COMMON;
		}
		else if (strItemTier == TEXT("UNCOMMON"))
		{
			Item.iItemTier = (_uint)TIER_UNCOMMON;
		}
		else if (strItemTier == TEXT("RARE"))
		{
			Item.iItemTier = (_uint)TIER_RARE;
		}
		else if (strItemTier == TEXT("UNIQUE"))
		{
			Item.iItemTier = (_uint)TIER_UNIQUE;
		}
		else if (strItemTier == TEXT("LEGENDARY"))
		{
			Item.iItemTier = (_uint)TIER_LEGENDARY;
		}

		getline(fin, Item.strName, L'|');		
		getline(fin, Item.strTexture, L'|');

		getline(fin, strItemIndex, L'|');
		getline(fin, strStatus, L'|');
		getline(fin, strPurchase, L'|');
		getline(fin, strSale, L'|');
		getline(fin, strPartIndex, L'|');
		getline(fin, strLimited, L'|');

		Item.iIndex = stoi(strItemIndex);
		Item.iStatus = stoi(strStatus);
		Item.iPurchase = stoi(strPurchase);
		Item.iSale = stoi(strSale);
		Item.iPartIndex = stoi(strPartIndex);
		Item.iLimited = stoi(strLimited);

		m_mapItem.emplace(Item.strName, Item);
		i++;
	}
	fin.close();

	return S_OK;
}

HRESULT CUI_Manager::Init_Skills()
{	
	SKILLINFO Info = {};
	Info.strName = TEXT("환영화살");
	Info.iSkillType = (_uint)WP_BOW;
	Info.isSkillIn = false;
	Info.iMp = 45;
	Info.iCoolTime = 20;
	Info.iSkillIdx = 3;
	Info.iModelSkillIndex = 7;
	Info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skillicon1");
	m_SkillInfo[WP_BOW][Info.iSkillIdx] = Info;

	Info.strName = TEXT("화살비");
	Info.iMp = 30;
	Info.iCoolTime = 8;
	Info.iSkillIdx = 2;
	Info.iModelSkillIndex = 1;
	Info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skillicon2");
	m_SkillInfo[WP_BOW][Info.iSkillIdx] = Info;

	Info.strName = TEXT("폭발 화살");
	Info.iMp = 12;
	Info.iCoolTime = 6;
	Info.iSkillIdx = 1;
	Info.iModelSkillIndex = 6;
	Info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skillicon3");
	m_SkillInfo[WP_BOW][Info.iSkillIdx] = Info;
	
	Info.strName = TEXT("삼중 연사");
	Info.iMp = 8;
	Info.iCoolTime = 7;
	Info.iSkillIdx = 0;
	Info.iModelSkillIndex = 0;
	Info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skillicon4");
	m_SkillInfo[WP_BOW][Info.iSkillIdx] = Info;
	
	Info.iSkillType = (_uint)WP_SWORD;
	Info.strName = TEXT("전광 석화");
	Info.iMp = 12;
	Info.iCoolTime = 8;
	Info.iSkillIdx = 2;
	Info.iModelSkillIndex = 2;
	Info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skillicon5");
	m_SkillInfo[WP_SWORD][Info.iSkillIdx] = Info;

	Info.strName = TEXT("기습 공격");
	Info.iMp = 8;
	Info.iCoolTime = 4;
	Info.iSkillIdx = 0;
	Info.iModelSkillIndex = 5;
	Info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skillicon7");
	m_SkillInfo[WP_SWORD][Info.iSkillIdx] = Info;
	
	Info.strName = TEXT("인장 각인");
	Info.iMp = 6;
	Info.iCoolTime = 8;
	Info.iSkillIdx = 1;
	Info.iModelSkillIndex = 4;
	Info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skillicon6");
	m_SkillInfo[WP_SWORD][Info.iSkillIdx] = Info;

	Info.strName = TEXT("연속 각인");
	Info.iMp = 8;
	Info.iCoolTime = 10;
	Info.iSkillIdx = 3;
	Info.iModelSkillIndex = 3;
	Info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skillicon8");
	m_SkillInfo[WP_SWORD][Info.iSkillIdx] = Info;


	return E_NOTIMPL;
}

SKILLINFO CUI_Manager::Get_SkillInfo(WEAPON_TYPE eType, _uint iIdx)
{
	SKILLINFO Info{};
	if (eType >= WP_END || eType < 0 || iIdx < 0 || iIdx > 4)
	{
		return Info;
	}

	return 	m_SkillInfo[eType][iIdx];
}

ITEM CUI_Manager::Find_Item(wstring& strItemName)
{	
	
	auto	iter = m_mapItem.find(strItemName);

	if (iter == m_mapItem.end())
	{
		ITEM Item = {};
		Item.iInvenType = -1;
		return Item;
	}
	return iter->second;

}

HRESULT CUI_Manager::Set_Item(wstring& strItemName, _uint iNum)
{
	ITEM Item = Find_Item(strItemName);
	if (Item.iInvenType == -1)
	{
		return E_FAIL;
	}
	
	dynamic_cast<CInven*>(m_pInven)->Set_Item(Item, iNum);

	return S_OK;
}

HRESULT CUI_Manager::Set_Inven(CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
	{
		return E_FAIL;
	}

	m_pInven = pGameObject;

	return S_OK;
}

HRESULT CUI_Manager::Set_InvenFrame(CGameObject* pGameObject)
{

	if (pGameObject == nullptr)
	{
		return E_FAIL;
	}

	m_pInvenFrame = pGameObject;

	return S_OK;
}

HRESULT CUI_Manager::Unlock_Skill(_uint iIndex)
{
	if (m_pSkillBook == nullptr)
	{
		return E_FAIL;
	}
	dynamic_cast<CSkillBook*>(m_pSkillBook)->Unlock_Skill(iIndex);
	return S_OK;
}

_bool CUI_Manager::Use_Skill(WEAPON_TYPE eType, CSkillBlock::SKILLSLOT eSlot, _int* iIndex, _int* iMp)
{
	if (m_pSkillBlock == nullptr)
	{
		return false;
	}
	
	return m_pSkillBlock->Use_Skill(eType, eSlot, iIndex, iMp);
}

CGameObject* CUI_Manager::Get_InvenFrame()
{
	if (m_pInvenFrame == nullptr)
	{
		return nullptr;
	}
	return m_pInvenFrame;
}

CSkillSlot* CUI_Manager::Get_SkillSlot(WEAPON_TYPE eType, CSkillBlock::SKILLSLOT eSlot)
{
	if (m_pSkillSlots[eType][eSlot] == nullptr)
	{
		return nullptr;
	}

	return m_pSkillSlots[eType][eSlot];
}

CGameObject* CUI_Manager::Get_ItemSlots(CItemBlock::ITEMSLOT eSlot)
{
	if (m_pInvenItemSlots[eSlot] == nullptr)
	{
		return nullptr;
	}
	return m_pInvenItemSlots[eSlot];
}

_bool CUI_Manager::Is_ItemSlotFull(CItemBlock::ITEMSLOT eSlot)
{
	return dynamic_cast<CItemSlot*>(m_pInvenItemSlots[eSlot])->Is_Full();
}

HRESULT CUI_Manager::Set_Item_In_EmptySlot(CItemBlock::ITEMSLOT eSlot, CItem* pItem, _int* iItemNum)
{

	if (FAILED(dynamic_cast<CItemSlot*>(m_pInvenItemSlots[eSlot])->Set_Item(pItem, iItemNum)))
	{
		return E_FAIL;
	}
	if (FAILED(dynamic_cast<CItemSlot*>(m_pItemSlots[eSlot])->Set_Item(pItem, iItemNum)))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

ITEM CUI_Manager::Set_Item_In_FullSlot(CItemBlock::ITEMSLOT eSlot, CItem* pItem, _int* iItemNum, _int* iChangeItemNum )
{
	ITEM eDefItem = {};
	eDefItem.iInvenType = -1;
	//m_iItemNum이 0보다 클경우 그 수만큼 빼주면되고, 0이면 다없애면되고 -1이면 없애면안됨
	if (dynamic_cast<CItemSlot*>(m_pInvenItemSlots[eSlot])->Get_ItemName() != pItem->Get_ItemDesc().strName)
	{
		*iChangeItemNum = dynamic_cast<CItemSlot*>(m_pInvenItemSlots[eSlot])->Get_ItemObject()->Get_ItemNum();
		 ITEM eItem = dynamic_cast<CItemSlot*>(m_pInvenItemSlots[eSlot])->Get_ItemObject()->Get_ItemDesc();

		if (FAILED(Set_Item_In_EmptySlot(eSlot, pItem, iItemNum)))
		{
			return eDefItem;
		}
		return eItem;
	}
	else
	{
		dynamic_cast<CItemSlot*>(m_pInvenItemSlots[eSlot])->Set_FullSlot(pItem, iItemNum);
		dynamic_cast<CItemSlot*>(m_pItemSlots[eSlot])->Set_FullSlot(pItem, iItemNum);
	}
	return eDefItem;
}

void CUI_Manager::Delete_Item_In_Slot(CItemBlock::ITEMSLOT eSlot)
{
	dynamic_cast<CItemSlot*>(m_pInvenItemSlots[eSlot])->Delete_Item();
	dynamic_cast<CItemSlot*>(m_pItemSlots[eSlot])->Delete_Item();
}

void CUI_Manager::Use_Item_In_Slot(CItemBlock::ITEMSLOT eSlot)
{
	dynamic_cast<CItemSlot*>(m_pInvenItemSlots[eSlot])->Use_Item();
	dynamic_cast<CItemSlot*>(m_pItemSlots[eSlot])->Use_Item();
}

void CUI_Manager::Set_RadarPos(TYPE eType, CTransform* pTransform)
{
	m_vecRadarPos[eType].push_back(pTransform);
}

void CUI_Manager::Delete_RadarPos(TYPE eType, CTransform* pTransform)
{
	for (size_t i = 0; i < m_vecRadarPos[eType].size(); i++)
	{
		if (m_vecRadarPos[eType][i] == pTransform)
		{
			m_vecRadarPos[eType].erase(m_vecRadarPos[eType].begin() + i);
		}
	}
}

const vector<CTransform*> CUI_Manager::Get_RadarPosition(TYPE eType) const
{
	return m_vecRadarPos[eType];
}


void CUI_Manager::Level_Up()
{
}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
