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
	_bool			m_isPicking{ false };
	_uint			m_CustomPart[PART_TYPE::PT_END];
	PART_TYPE		m_eChangedPart{ PT_END };

	_vec4			m_vHairColor{0.f, 0.f, 0.f, 0.f};

public:
	HRESULT Set_CustomPart(PART_TYPE eType, _uint iIndex);
	void Set_HairColor(_vec4 vColor) { m_vHairColor = vColor; }
	void Set_Picking_UI(_bool isPicking) { m_isPicking = isPicking; }

	const _vec4& Get_HairColor() const { return m_vHairColor; }
	const _uint& Get_CustomPart (PART_TYPE eType);

	const PART_TYPE& Is_CustomPartChanged() const { return m_eChangedPart; }
	const _bool& Is_Picking_UI() const { return m_isPicking; }

public:
	virtual void Free() override;
};

END
