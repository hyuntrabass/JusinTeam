#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
	for (size_t i = 0; i < PART_TYPE::PT_END; i++)
	{
		m_CustomPart[i] = 0;
	}
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


void CUI_Manager::Free()
{

}
