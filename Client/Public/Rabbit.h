#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CRabbit final : public CMonster
{
public:
	enum RABBIT_ANIM
	{
		ATTACK01,
		ATTACK02,
		DIE,
		HIT_ADD, // 고장
		HIT_ADD_L, // 고장
		HIT_ADD_R, // 고장
		IDLE,
		KNOCKDOWN,
		ROAR,
		RUN,
		STUN,
		WALK,
		ANIM_END
	};

	enum RABBIT_STATE
	{
		STATE_IDLE,
		STATE_ROAM,
		STATE_ATTACK,
		STATE_DIE,
		STATE_END
	};

	enum RABBIT_COLL
	{
		LH, RH, LF, RF,
		COLL_END
	};

private:
	CRabbit(_dev pDevice, _context pContext);
	CRabbit(const CRabbit& rhs);
	virtual ~CRabbit() = default;

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
	CCollider* m_pColliderCom[COLL_END] = { nullptr };
private:
	RABBIT_STATE m_ePreState = STATE_END;
	RABBIT_STATE m_eCurState = STATE_END;

private:
	_float m_fIdleTime = {};

	_uint m_iRoamingPattern = {};

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CRabbit* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END