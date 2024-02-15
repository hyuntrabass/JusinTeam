#pragma once

#include "Client_Define.h"
#include "GameObject.h"
#include "CommonTrail.h"

BEGIN(Client)

class CDragon_Boss final : public CGameObject
{
public:

	enum DRAGON_ANIM
	{
		ATTACK00,
		ATTACK01,
		ATTACK02,
		OUROBOROS_ATTACK10, // 양발 빠르게 후려치기
		ATTACK04,
		ATTACK05,
		ATTACK06,
		DIE,
		FAFNIR01_SC08_MON_ROAR,
		FLY_LOOP,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		KNOCKDOWN,
		OUROBOROS_ATTACK01, // 01 오른발 후려치기
		OUROBOROS_ATTACK02, // 02 양발로 내려찍기
		OUROBOROS_ATTACK03, // 짧게 울부짖기
		OUROBOROS_ATTACK04, // 왼발로 쿵
		OUROBOROS_ATTACK05, // 04 날개치기
		OUROBOROS_ATTACK06_END,
		OUROBOROS_ATTACK06_LOOP, // 하늘 날아서 불쏘기
		OUROBOROS_ATTACK06_START,
		OUROBOROS_ATTACK07, // 00 울부짖기_불기둥
		OUROBOROS_ATTACK08, // 05 불쏘는거
		OUROBOROS_ATTACK09, // 사라졌다가 땅찍으면서 내려오기(내려올때 바닥데미지)
		OUROBOROS_ATTACK_NO_USE,
		OUROBOROS_RAGE,
		ROAR,
		RUN,
		START,
		STURN,
		TURN_L,
		TURN_R,
		VOIDDRAGON_RAGE,
		WALK,
		ANIM_END
	};

	enum DRAGON_STATE
	{
		STATE_IDLE,
		STATE_ROAR,
		STATE_CHASE,
		STATE_RIGHT_ATTACK, // 01
		STATE_TAKE_DOWN, // 02
		STATE_SHORT_ROAR, // 03
		STATE_LEFT_ATTACK, // 04
		STATE_WING_ATTACK, // 05
		STATE_FLY_FIRE, // 06
		STATE_FIRE_PILLAR, // 07
		STATE_SHOOT_FIRE, // 08
		STATE_SOAR, // 09
		STATE_DOUBLE_SLASH, // 10
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

	enum DRAGON_ATTACK // 랜덤 방지용
	{
		RIGHT_ATTACK,
		TAKE_DOWN,
		SHORT_ROAR,
		LEFT_ATTACK,
		WING_ATTACK,
		FLY_FIRE,
		FIRE_PILLAR,
		SHOOT_FIRE,
		SOAR,
		DOUBLE_SLASH,
		ATTACK_END
	};

private:
	CDragon_Boss(_dev pDevice, _context pContext);
	CDragon_Boss(const CDragon_Boss& rhs);
	virtual ~CDragon_Boss() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

public:
	HRESULT Add_Collider();
	void Update_Collider();

public:
	void Update_Trail(_float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };

private:
	CCommonTrail* m_pLeftTrail1 = { nullptr };
	CCommonTrail* m_pLeftTrail2 = { nullptr };
	CCommonTrail* m_pLeftTrail3 = { nullptr };

	CCommonTrail* m_pRightTrail1 = { nullptr };
	CCommonTrail* m_pRightTrail2 = { nullptr };
	CCommonTrail* m_pRightTrail3 = { nullptr };

private:
	DRAGON_STATE m_ePreState = STATE_END;
	DRAGON_STATE m_eCurState = STATE_END;

private:
	//static const _float m_fChaseRange;
	static const _float m_fAttackRange;

private:
	ANIM_DESC m_Animation{};

private:
	_float m_fIdleTime = {};

private:
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };
	_bool m_bAttack_Selected[ATTACK_END] = { false };

private:
	_bool m_bChangePass = { false };
	_uint m_iPassIndex = {};
	_float m_fHitTime = {};


public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDragon_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END