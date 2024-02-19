#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CHPMonster;

class CBalloon final : public CMonster
{
public:
	enum BALLOON_ANIM
	{
		Attack,
		die,
		Idle,
		matAction,
		matAction001,
		Run,
		NodetreeAction,
		Skill_End,
		Skill_Loop,
		Skill_Start,
		ANIM_END,
	};

	enum BALLOON_STATE
	{
		STATE_IDLE,
		STATE_HIT,
		STATE_ATTACK,
		STATE_DIE,
		STATE_END
	};

private:
	CBalloon(_dev pDevice, _context pContext);
	CBalloon(const CBalloon& rhs);
	virtual ~CBalloon() = default;

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
	BALLOON_STATE m_ePreState = STATE_END;
	BALLOON_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

private:
	_bool m_bParticle{};
	_bool m_bDamaged = { false };


public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CBalloon* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END