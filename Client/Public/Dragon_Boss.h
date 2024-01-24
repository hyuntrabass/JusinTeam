#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CDragon_Boss final : public CGameObject
{
public:

	enum DRAGON_ANIM
	{
		Dragon_Attack00, // 전방 메테오
		Dragon_Attack01, //앞발 후려치기
		Dragon_Attack02,
		Dragon_Attack03, //양발 후려치기
		Dragon_Attack04, //날개 치기
		Dragon_Attack05, //전방에 함성?
		Dragon_Attack06, //짧게 날기
		Dragon_Die,
		Dragon_Fafnir01_SC08_mon_roar,
		Dragon_Fly_loop,
		Dragon_hit_add,
		Dragon_Hit_L,
		Dragon_Hit_R,
		Dragon_Idle,
		Dragon_Knockdown,
		Dragon_ouroboros_attack01,
		Dragon_ouroboros_attack02,
		Dragon_ouroboros_attack03,
		Dragon_ouroboros_attack04, // 한쪽발로 땅찍기
		Dragon_ouroboros_attack05,
		Dragon_ouroboros_attack06_end,
		Dragon_ouroboros_attack06_loop,
		Dragon_ouroboros_attack06_start,  //하늘 날아서 불쏘기
		Dragon_ouroboros_attack07,
		Dragon_ouroboros_attack08,
		Dragon_ouroboros_attack09, // 사라졌다가 땅찍으면서 내려오기
		Dragon_ouroboros_attack10,
		Dragon_ouroboros_Rage,
		Dragon_roar,
		Dragon_Run,
		Dragon_start,
		Dragon_sturn,
		Dragon_turn_L,
		Dragon_turn_R,
		Dragon_VoidDragon_Rage,
		Dragon_walk
	};

	enum DRAGON_STATE
	{
		IDLE,
		WALK,
		RUN,
		ATTACK,
		HIT,
		STUN,
		DIE,
		BOSS_STATE_END
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
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

public:
	HRESULT Add_Collider();
	void Update_Collider();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };
																																																																
private:
	DRAGON_STATE m_ePreState = BOSS_STATE_END;
	DRAGON_STATE m_eState = BOSS_STATE_END;

private:
	ANIM_DESC m_Animation{};

private:
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDragon_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END