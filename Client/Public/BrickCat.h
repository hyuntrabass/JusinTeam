#pragma once

#include "Client_Define.h"
#include "Pet.h"

BEGIN(Client)

class CBrickCat final : public CPet
{
public:
	enum PET_CAT_ANIM
	{
		COLLECT,
		EMOTION,
		IDLE,
		RUN,
		TELEPORT_END,
		TELEPORT_START,
		ANIM_END
	};

	enum PET_CAT_STATE
	{
		STATE_IDLE,
		STATE_CHASE,
		STATE_EMOTION,
		STATE_INVEN,
		STATE_END
	};

private:
	CBrickCat(_dev pDevice, _context pContext);
	CBrickCat(const CBrickCat& rhs);
	virtual ~CBrickCat() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	PET_CAT_STATE m_ePreState = STATE_END;
	PET_CAT_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};
	_float m_fTrailTime = {};

private:
	_bool m_isTrailOn = { false };
	class CCommonSurfaceTrail* m_pTrail{ nullptr };

public:
	void Set_Trail(_bool isOn) { m_isTrailOn = isOn; }
public:
	static CBrickCat* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END