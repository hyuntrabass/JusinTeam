#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

struct GuardTowerInfo
{
	_uint iIndex{};
	_mat mMatrix{};
};

class CGuardTower final : public CGameObject
{
public:
	enum PATTERN_TYPE {
		PATTERN_1,
		PATTERN_2,
		PATTERN_3,
		PATTERN_END
	};

	enum GUARDTOWER_ANIM {
		ANIM_DIE,
		ANIM_IDLE,
		ANIM_END
	};

	enum GUARDTOWER_STATE {
		STATE_IDLE,
		STATE_DETECT,
		STATE_ATTACK_READY,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

	enum EFFECT_DIR {
		UP,
		DOWN,
		LEFT,
		RIGHT,
		DIR_END
	};
private:
	CGuardTower(_dev pDevice, _context pContext);
	CGuardTower(const CGuardTower& rhs);
	virtual ~CGuardTower() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State_Pattern_1(_float fTimeDelta);
	void Tick_State_Pattern_2(_float fTimeDelta);
	void Tick_State_Pattern_3(_float fTimeDelta);
	void View_Detect_Range_Pattern_1(_float fTimeDelta);
	void View_Detect_Range_Pattern_2(EFFECT_DIR eDir);
	void View_Detect_Range_Pattern_3();

	_vec4 Compute_PlayerPos();
	_vec4 Compute_PlayerLook();
	_float Compute_PlayerDistance();
	_vec4 Compute_Player_To_Dir(_vec4 vPos);
	void Compute_Lazer_Dir();
	void Create_Range_Pattern_1();
	void Create_Range_Pattern_2();
	void Create_Range_Pattern_3();
	void Create_Lazer();
	void Create_Attack_Lazer();


private:
	GUARDTOWER_STATE m_ePreState = STATE_END;
	GUARDTOWER_STATE m_eCurState = STATE_END;

private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVTFModel* m_pModelCom = nullptr;

	CCollider* m_pBodyColliderCom = nullptr;
	CCollider* m_pAttackColliderCom = nullptr;
	CTexture* m_pDissolveTextureCom = nullptr;

	class CEffect_Dummy* m_pBaseEffect{ nullptr };
	class CEffect_Dummy* m_pFrameEffect{ nullptr };
	class CEffect_Dummy* m_pThreatEffect{ nullptr };
	class CEffect_Dummy* m_pAttackEffect{ nullptr };

private:
	GuardTowerInfo m_Info{};
	PATTERN_TYPE m_Pattern_Type{ PATTERN_END };
	EffectInfo Info{};
	EFFECT_DIR m_Dir{DOWN};
	_uint m_iPassIndex{};
	_float m_fDeadTime{ 0.f };
	_float m_fDissolveRatio{ 0.f };
	_float m_fIdleTime{ 0.f };
	_float m_fDetectTime{ 0.f };
	_float m_fAttackTime{ 0.f };
	_float m_fAttackDelay{ 0.f };
	_float m_fAnimTime{ 0.f };

	_bool m_bChangePass{false};
	_bool m_bDamaged{ false };
	_bool m_bAttacked{ false };
	_bool m_isPrototype{ false };
	_bool m_bInit{ false };
	_bool m_isDetected{ false };
	_float m_fHitTime{ 0.f };
	_float m_fHittedTime{ 0.f };
	_uint m_iIndex{ 0 };
	_mat m_GuardTowerMatrix{};
	_mat m_EffectMatrix{};
	_mat m_LazerMatrix{};
	_vec4 m_vLook{};
	_vec4 m_vCurPlayerPos{};

private:
	ANIM_DESC m_Animation{};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Render_Instance();

public:
	HRESULT Add_Collider();
	HRESULT Add_Attack_Collider();
	void Update_Collider();

public:
	static CGuardTower* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END