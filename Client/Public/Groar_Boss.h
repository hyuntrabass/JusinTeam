#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CGroar_Boss final : public CGameObject
{
public:
	enum GROAR_NPC_ANIM
	{
		DIE,
		NPC_IDLE,
		TALK,
		NPC_ANIM_END
	};

	enum GROAR_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		ATTACK04,
		ATTACK05,
		ATTACK06,
		ATTACK07,
		ATTACK_RAGE,
		DIE01,
		GROAR_DEAD_SC01_MON_GROAR,
		GROAR_DEAD_SC02_MON_GROAR,
		GROAR_DEAD_SC03_MON_GROAR,
		HIT_ADD,
		IDLE,
		MON_GROAR_ASGARD_ATTACK00, // 오른손에서 초록색 투사체 던지기
		MON_GROAR_ASGARD_ATTACK01, // 왼손에서 초록색 투사체 던지기
		MON_GROAR_ASGARD_ATTACK02, // 초록색 투사체 6개 날리기
		MON_GROAR_ASGARD_ATTACK03, // 바닥 쾅쾅 찍기
		MON_GROAR_ASGARD_ATTACK04, // 꼬리치기 -> 거미줄로 땡기기
		MON_GROAR_ASGARD_ATTACK05, // 거미 소환
		MON_GROAR_ASGARD_ATTACK06, // 꼬리찍어서 장판데미지
		MON_GROAR_ASGARD_ATTACK07, // 손 X자하고 바닥에서 초록불(장판 위 계속 데미지)
		MON_GROAR_ASGARD_ATTACK08, // 울부짖기(장판)
		MON_GROAR_ASGARD_ATTACK_RAGE, // 울부짖기(장판)
		// 00, 01, 02, 03, 04, 05, 06, 07, 08
		MON_GROAR_ASGARD_DIE,
		MON_GROAR_ASGARD_IDLE,
		MON_GROAR_ASGARD_ROAR,
		MON_GROAR_ASGARD_SPAWN,
		MON_GROAR_ASGARD_WALK,
		ROAR,
		RUN,
		STUN,
		TURN_L,
		TURN_R,
		W_ATTACK01,
		W_ATTACK02,
		W_ATTACK04,
		W_ATTACK05,
		W_ATTACK06,
		W_ATTACK07,
		W_ATTACK08,
		W_GROAR_RAGE,
		WALK,
		ANIM_END
	};

	enum GROAR_STATE
	{
		STATE_NPC,
		STATE_SCENE01,
		STATE_SCENE02,
		STATE_BOSS,
		STATE_END
	};

	enum GROAR_BOSS_STATE
	{
		BOSS_STATE_IDLE,
		BOSS_STATE_ROAR,
		BOSS_STATE_CHASE,
		BOSS_STATE_THROW_ATTACK, // 00, 01
		BOSS_STATE_FLOOR_ATTACK, // 03, 06, 07, 08
		BOSS_STATE_SIX_MISSILE, // 02
		BOSS_STATE_WEB, // 04
		BOSS_STATE_SPIDER, // 05
		BOSS_STATE_DIE,
		BOSS_STATE_END
	};

	// BOSS_STATE_THROW_ATTACK 바로 뒤에 BOSS_STATE_SIX_MISSILE 안나오게
	// 그룹별로 랜덤하게

private:
	CGroar_Boss(_dev pDevice, _context pContext);
	CGroar_Boss(const CGroar_Boss& rhs);
	virtual ~CGroar_Boss() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

public:
	HRESULT Add_Collider();
	void Update_Collider();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

	CModel* m_pNPCModelCom = { nullptr };
	CModel* m_pScene01ModelCom = { nullptr };
	CModel* m_pScene02ModelCom = { nullptr };
	CModel* m_pBossModelCom = { nullptr };

	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };

private:
	GROAR_STATE m_ePreState = STATE_END;
	GROAR_STATE m_eCurState = STATE_END;

	GROAR_BOSS_STATE m_eBossPreState = BOSS_STATE_END;
	GROAR_BOSS_STATE m_eBossCurState = BOSS_STATE_END;

	GROAR_BOSS_STATE m_eBossPreAttackState = BOSS_STATE_SPIDER;

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

private:
	ANIM_DESC m_Animation{};

private:
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bSwitchThrow = { false };
	_bool m_bCreateMissile = { false };
	_uint m_iThrowAttackCombo = {};

private:
	_bool m_bCreateSpider = { false };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGroar_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END