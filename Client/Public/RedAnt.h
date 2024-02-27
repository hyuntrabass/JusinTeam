#pragma once

#include "VTFMonster.h"
#include "Client_Define.h"

BEGIN(Client)

class CRedAnt final : public CVTFMonster
{
public:
	enum ANIM
	{
		Anim_attack01,
		Anim_attack02, //°ø°Ý
		Anim_attack03,
		Anim_die, //»ç¸Á
		Anim_hit_add,
		Anim_idle,
		Anim_knockdown,
		Anim_L_hit,
		Anim_R_hit,
		Anim_roar,
		Anim_run,
		Anim_stun,
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
	CRedAnt(_dev pDevice, _context pContext);
	CRedAnt(const CRedAnt& rhs);
	virtual ~CRedAnt() = default;

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
	static CRedAnt* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END