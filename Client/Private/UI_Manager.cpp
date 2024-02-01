#include "UI_Manager.h"
#include "GameInstance.h"
#include "Inven.h"
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

	return S_OK;
}

void CUI_Manager::Set_Exp_ByPercent(_float fExp)
{


	m_fExp.x += m_fExp.y * fExp / 100.f;
	if (m_fExp.x >= m_fExp.y)
	{
		Level_Up();
		m_fExp.x = 0.f;
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

CGameObject* CUI_Manager::Get_InvenFrame()
{
	if (m_pInvenFrame == nullptr)
	{
		return nullptr;
	}
	return m_pInvenFrame;
}

CGameObject* CUI_Manager::Get_ItemSlots(CItemBlock::ITEMSLOT eSlot)
{
	if (m_pItemSlots[eSlot] == nullptr)
	{
		return nullptr;
	}
	return m_pItemSlots[eSlot];
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
