#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CTrilobiteA final : public CMonster
{
public:
	enum TRILOBITE_A_ANIM
	{
		ATTACK01,
		ATTACK02,
		DIE01,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,//
		TURN_L,//
		TURN_R,
		WALK,//
		ANIM_END
	};

	enum TRILOBITE_A_STATE
	{
		STATE_IDLE,
		STATE_ROAM,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

private:
	CTrilobiteA(_dev pDevice, _context pContext);
	CTrilobiteA(const CTrilobiteA& rhs);
	virtual ~CTrilobiteA() = default;

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

public:
	void Attack(_float fTimeDelta);

private:
	CCollider* m_pColliderCom = { nullptr };

private:
	TRILOBITE_A_STATE m_ePreState = STATE_END;
	TRILOBITE_A_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

	_uint m_iRoamingPattern = {};
	_uint m_iAttackPattern = {};
	_uint m_iHitPattern = {};

	_bool m_bSelectAttackPattern = { false };

private:
	static const _float g_fChaseRange;
	static const _float g_fAttackRange;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CTrilobiteA* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END