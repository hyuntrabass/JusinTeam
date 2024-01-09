#pragma once
#include "Base.h"

BEGIN(Engine)

class CButton_Manager final : public CBase
{
private:
	CButton_Manager() = default;
	virtual ~CButton_Manager() = default;

public:
	void Register_Button(_uint iLevelIndex, const wstring& strButtonTag);

	void Set_ButtonState(_uint iLevelIndex, const wstring& strButtonTag, const _bool& bState);
	const _bool Get_ButtonState(_uint iLevelIndex, const wstring& strButtonTag) const;

public:
	HRESULT Init(_uint iNumLevel);

private:
	map<const wstring, _bool>* m_pButtons{};

public:
	static CButton_Manager* Create(_uint iNumLevel);
	virtual void Free() override;
};

END