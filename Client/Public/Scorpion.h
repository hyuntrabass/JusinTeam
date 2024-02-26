#pragma once

#include "VTFMonster.h"
#include "Client_Define.h"

BEGIN(Client)

class CScorpion final : public CVTFMonster
{
public:
	enum ANIM
	{
		Anim_attack01,
		Anim_attack02,
		Anim_attack03, //°ø°Ý
		Anim_attack04,
		Anim_die,
		Anim_die01,
		Anim_hit_add,
		Anim_hit_L,
		Anim_hit_R,
		Anim_idle,
		Anim_knockdown, //»ç¸Á duration 0.4f
		Anim_roar,
		Anim_run,
		Anim_stun,
		Anim_Turn_L,
		Anim_Turn_R,
		Anim_walk,
		Anim_End
	};
	enum STATE
	{
		State_Idle,
		State_Run,
		State_Attack,
		State_Die,
		State_End
	};

private:
	CScorpion(_dev pDevice, _context pContext);
	CScorpion(const CScorpion& rhs);
	virtual ~CScorpion() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

private:
	CCollider* m_pAttackColliderCom = { nullptr };

private:
	STATE m_eState{ State_End };
	STATE m_ePreState{ State_End };

private:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	HRESULT Add_Components();

public:
	static CScorpion* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END