#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:
	enum CHANNELID
	{
	};

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

private:
	_uint m_CustomPart[PART_TYPE::PT_END];
	PART_TYPE m_eChangedPart{ PT_END };

	_vec4 m_vHairColor{0.f, 0.f, 0.f, 0.f};

public:
	void Set_HairColor(_vec4 vColor) { m_vHairColor = vColor; }
	_vec4 Get_HairColor() { return m_vHairColor; }

	PART_TYPE is_CustomPartChanged() { return m_eChangedPart; }
	_uint Get_CustomPart(PART_TYPE eType);
	HRESULT Set_CustomPart(PART_TYPE eType, _uint iIndex);

public:
	virtual void Free() override;
};

END
