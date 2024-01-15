#include "UI_Manager.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
	for (size_t i = 0; i < PART_TYPE::PT_END; i++)
	{
		m_CustomPart[i] = 0;
	}
}

_uint CUI_Manager::Get_CustomPart(PART_TYPE eType)
{
	m_eChangedPart = PT_END;
	return m_CustomPart[eType];
}

HRESULT CUI_Manager::Set_CustomPart(PART_TYPE eType, _uint iIndex)
{
	//인덱스 범위를 알면 좋을거같은데
	m_eChangedPart = eType;
	m_CustomPart[eType] = iIndex;
	return S_OK;
}


void CUI_Manager::Free()
{

}
