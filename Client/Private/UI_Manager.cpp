#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
	for (size_t i = 0; i < PART_TYPE::PT_END; i++)
	{
		m_CustomPart[i] = 0;
	}
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

HRESULT CUI_Manager::Set_Coin(_uint iCoin)
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

HRESULT CUI_Manager::Set_CustomPart(PART_TYPE eType, _uint iIndex)
{
	m_eChangedPart = eType;
	m_CustomPart[eType] = iIndex;
	return S_OK;
}


void CUI_Manager::Level_Up()
{
}

void CUI_Manager::Free()
{

}
