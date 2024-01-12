#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CVoid05 final : public CMonster
{
public:
	enum VOID05_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		DIE,
		DIE1,
		HIT_ADD,
		HIT_L,
		HIT_R,
		IDLE,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,
		TURN_L,
		TURN_R,
		WALK,
		ANIM_END
	};

	enum VOID05_STATE
	{
		STATE_IDLE,
		STATE_WALK,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

private:
	CVoid05(_dev pDevice, _context pContext);
	CVoid05(const CVoid05& rhs);
	virtual ~CVoid05() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Change_State(_float fTimeDelta);
	void Control_State(_float fTimeDelta);

private:
	CCollider* m_pRHColliderCom = { nullptr };
	CCollider* m_pLHColliderCom = { nullptr };

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CVoid05* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END