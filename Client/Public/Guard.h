#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

struct GuardInfo
{
	_uint iIndex{};
	_mat mMatrix{};
};

class CGuard final : public CGameObject
{
public:
	enum GUARD_PATTERN {
		PATTERN_1,
		PATTERN_2,
		PATTERN_3,
		PATTERN_END
	};

	enum GUARD_ANIM {
		ANIM_DIE,
		ANIM_IDLE,
		ANIM_RUN,
		ANIM_SWING,
		ANIM_BOW,
		ANIM_STEP,
		ANIM_WALK,
		ANIM_END
	};

	enum GUARD_STATE {
		STATE_IDLE,
		STATE_TURN,
		STATE_PATROL,
		STATE_CHASE,
		STATE_ATTACK_SWING,
		STATE_ATTACK_STEP,
		STATE_HIT,
		STATE_BACK,
		STATE_DIE,
		STATE_END
	};


private:
	CGuard(_dev pDevice, _context pContext);
	CGuard(const CGuard& rhs);
	virtual ~CGuard() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;
	_bool Get_Detected() { return m_isDetected; }
public:
	void Init_State(_float fTimeDelta);
	void Tick_State_Pattern1(_float fTimeDelta);
	void Tick_State_Pattern2(_float fTimeDelta);
	void Tick_State_Pattern3(_float fTimeDelta);

	_vec4 Compute_PlayerPos();
	_vec4 Compute_PlayerLook();
	_float Compute_PlayerDistance();

	void Create_Range();
	void Detect_Range(_float fAngle, _float fDist, _vec4 vNormalToPlayer);


private:
	GUARD_STATE m_ePreState = STATE_END;
	GUARD_STATE m_eCurState = STATE_END;

private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVTFModel* m_pModelCom = nullptr;

	CCollider* m_pBodyColliderCom = nullptr;
	CCollider* m_pAttackColliderCom = nullptr;
	CTexture* m_pDissolveTextureCom = nullptr;

	class CEffect_Dummy* m_pBaseEffect{ nullptr };
	class CEffect_Dummy* m_pFrameEffect{ nullptr };

private:
	_uint m_iPassIndex{};
	_float m_fDeadTime{ 0.f };
	_float m_fDissolveRatio{ 0.f };
	_float m_fIdleTime{ 0.f };
	_float m_fTurnTime{};
	_float m_fAttackTime{};
	_float m_fPatrolTime{};
	_float m_fAttackDelay{ 0.f };

	 _randNum m_iRandomAttack{0};
	_bool m_bDamaged{ false };
	_bool m_bAttacked{ false };
	_bool m_bChangePass = false;
	_bool m_isArrived = false;
	_bool m_isDetected{false};
	_float m_fHitTime{ 0.f };
	_float m_fHittedTime{ 0.f };
	GuardInfo m_Info{};
	EffectInfo Info{};
	GUARD_PATTERN m_ePattern{ PATTERN_END };
	_mat m_OriginMatrix{};
	_mat m_EffectMatrix{};
	_vec4 vIdlePos{};
	_vec4 vPatrolPos{};

private:
	ANIM_DESC m_Animation{};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	HRESULT Add_Collider();
	void Update_Collider();
	HRESULT Render_Instance();

public:
	static CGuard* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END