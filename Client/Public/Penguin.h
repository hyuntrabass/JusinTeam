#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CPenguin final : public CMonster
{
public:
	enum PENGUIN_ANIM
	{
		ATTACK01,
		ATTACK02,
		DIE,
		HIT_ADD, // 고장
		HIT_L, // 고장
		HIT_R, // 고장
		IDLE,
		INTERACTION01,
		INTERACTION02,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,
		WALK,
		ANIM_END
	};

	enum PENGUIN_STATE
	{
		STATE_IDLE,
		STATE_ROAM,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

private:
	CPenguin(_dev pDevice, _context pContext);
	CPenguin(const CPenguin& rhs);
	virtual ~CPenguin() = default;

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
	CCollider* m_pColliderCom = { nullptr };

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CPenguin* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END