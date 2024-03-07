#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CEagle final : public CMonster
{
public:
	enum EAGLE_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		ATTACK04,
		DIE,
		DIE001,
		EAGLE_RAGE,
		EBOSS_ATTACK01,
		EBOSS_ATTACK02,
		EBOSS_ATTACK03,
		EBOSS_ATTACK04,
		EBOSS_ATTACK05,
		EBOSS_RAGE,
		END000,
		END001,
		FLY,
		FLY001,
		FLYIDLE,
		FLYIDLE001,
		HARPY_SHOT01,
		HIT,
		HIT001,
		HIT_ADD,
		HIT_ADD001,
		IDLE,
		IDLE001,
		KNOCKDOWN,
		KNOCKDOWN001,
		ROAR,
		ROAR001,
		ROAR01,
		ROAR01001,
		START,
		START001,
		STUN,
		STUN001,
		WALK,
		WALK001,
		ANIM_END
	};

	enum EAGLE_STATE
	{
		STATE_SPAWN,
		STATE_IDLE,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_DIE,
		STATE_END
	};

private:
	CEagle(_dev pDevice, _context pContext);
	CEagle(const CEagle& rhs);
	virtual ~CEagle() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	//class CEffect_Dummy* m_pDistortion = { nullptr };

private:
	EAGLE_STATE m_ePreState = STATE_END;
	EAGLE_STATE m_eCurState = STATE_END;

private:
	_float m_fTime = {};
	_float m_fDamageTime = {};
	_float m_fIdleTime = {};

private:
	_bool m_bCreateEffect = { false };

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

private:
	static _uint m_iEagleID;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CEagle* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END