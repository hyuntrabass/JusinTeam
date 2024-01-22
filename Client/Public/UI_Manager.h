#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

private:
	PART_TYPE		m_eChangedPart{ PT_END };

	_bool			m_isPicking{ false };
	_bool			m_isShowing{ false };
	_bool			m_isInvenActive{ false };
	_bool			m_isSetInvenState{ false };

	_uint			m_iCoin{};
	_uint			m_iDiamond{};
	_uint			m_CustomPart[PART_TYPE::PT_END];

	_float2			m_fExp{0.f, 1000.f};

	_vec4			m_vInvenPos{0.f, 20.f, 0.f, 1.f};
	_vec4			m_vPlayerPos{0.f, 0.f, 0.f, 0.f};
	_vec4			m_vCameraPos{0.f, 0.f, 0.f, 0.f};
	_vec4			m_vHairColor{0.f, 0.f, 0.f, 0.f};

public:
	HRESULT Set_CustomPart(PART_TYPE eType, _uint iIndex);
	void Set_HairColor(_vec4 vColor) { m_vHairColor = vColor; }
	void Set_Picking_UI(_bool isPicking) { m_isPicking = isPicking; }
	void Set_FullScreenUI(_bool isShowing) { m_isShowing = isShowing; }
	void Set_InvenActive(_bool isInvenActive) { if (m_isInvenActive && !isInvenActive) { m_isSetInvenState = false; } m_isInvenActive = isInvenActive;  }
	void Set_Exp_ByPercent(_float fExp);
	HRESULT Set_Coin(_uint iCoin);
	HRESULT Set_Diamond(_uint iDia);
	_bool Set_CurrentPlayerPos(_vec4 vPos);

	const _vec4& Get_HairColor() const { return m_vHairColor; }
	const _vec4& Get_InvenPos() const { return m_vInvenPos; }
	const _vec4& Get_LastPlayerPos() const { return m_vPlayerPos; }
	const _uint& Get_CustomPart (PART_TYPE eType);
	const _uint& Get_Coin() const { return m_iCoin; }
	const _uint& Get_Diamond() const { return m_iDiamond; }
	const _float2& Get_Exp() const { return m_fExp; }

	const PART_TYPE& Is_CustomPartChanged() const { return m_eChangedPart; }
	const _bool& Is_Picking_UI() const { return m_isPicking; }
	const _bool& Showing_FullScreenUI() const { return m_isShowing; }
	const _bool& Is_InvenActive() const { return m_isInvenActive; }

	void Level_Up();
public:
	virtual void Free() override;
};

END
