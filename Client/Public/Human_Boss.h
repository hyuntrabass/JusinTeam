#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CHuman_Boss final : public CGameObject
{
public:

	enum HUMANBOSS_ANIM
	{
	HumanBoss_Attack01, // 창으로 정면 2번 치기
	HumanBoss_Attack01_A,
	HumanBoss_Attack02, // 철퇴로 3연타 
	HumanBoss_Attack02_A,
	HumanBoss_Attack03, // 창 빙빙 돌리고 때리기
	HumanBoss_Attack03_A,
	HumanBoss_Attack04, //철퇴로 멀리 때리기
	HumanBoss_Attack04_A, // 앞쪽에 뭐 소환하는 느낌
	HumanBoss_Attack05, // 공중에 소리지름 
	HumanBoss_Attack05_A, //창던지기
	HumanBoss_Attack06, // 점프후 주변 휘두름
	HumanBoss_Attack06_A,
	HumanBoss_Attack07,
	HumanBoss_Attack07_A,
	HumanBoss_Attack08,
	HumanBoss_Attack08_A,
	HumanBoss_Attack09, //주변 창으로 공격
	HumanBoss_Attack09_A,
	HumanBoss_Attack10, //철퇴 돌리고 위로 휘두름
	HumanBoss_Die,
	HumanBoss_Die_A,
	HumanBoss_Idle,
	HumanBoss_Idle_A,
	HumanBoss_Roar,
	HumanBoss_Roar_A,
	HumanBoss_Run,
	HumanBoss_Run_A,
	HumanBoss_Spawn,
	HumanBoss_Spawn_A,
	HumanBoss_Talk,
	HumanBoss_Talk_A,
	HumanBoss_Walk,
	HumanBoss_Walk_A,
	HumanBoss_AnimEnd
	};

	enum STATE
	{
		SPEAR1, // 창 정면 2연타
		SPEAR2, // 창 돌리면서 주변 공격
		SPEAR3,	// 창 꽂고 전방에 전기? 공격
		SPEAR4,	// 창 던지기
		MACE1,	// 철퇴 정면 3연타
		MACE2,	// 철퇴로 플레이어쪽 중거리 공격
		MACE3, //
		IDLE,
		WALK,
		RUN,
		DIE,
		Spwan,
		BOSS_STATE_END
	};

private:
	CHuman_Boss(_dev pDevice, _context pContext);
	CHuman_Boss(const CHuman_Boss& rhs);
	virtual ~CHuman_Boss() = default;

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
	virtual void Set_Damage(_int iDamage, _uint MonAttType = 0) override;
private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };
																																																																
private:
	STATE m_ePreState = BOSS_STATE_END;
	STATE m_eState = BOSS_STATE_END;

private:
	ANIM_DESC m_Animation{};

private:
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };
	_bool m_bSecondPattern{};
	_bool m_bChangePass{};
	_float m_fHitTime{};
	_uint m_iPassIndex{};
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHuman_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END