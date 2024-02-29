#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

struct MiniDungeonInfo
{
	_mat mMatrix{};
	_uint iIndex{};
};

class CGuard final : public CGameObject
{
public:
	enum GUARD_ANIM {
		ANIM_DIE,
		ANIM_IDLE,
		ANIM_RUN,
		ANIM_ATTACK_1,
		ANIM_ATTACK_2,
		ANIM_WALK,
		ANIM_END
	};

	enum GUARD_STATE {
		STATE_IDLE,
		STATE_PATROL,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
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

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);
	void View_Detect_Range();
private:
	GUARD_STATE m_ePreState = STATE_END;
	GUARD_STATE m_eCurState = STATE_END;

private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;

	CCollider* m_pBodyColliderCom = nullptr;
	CCollider* m_pAttackColliderCom = nullptr;
	CTexture* m_pDissolveTextureCom = nullptr;

	class CEffect_Dummy* m_pBaseEffect{ nullptr };
	class CEffect_Dummy* m_pFrameEffect{ nullptr };

private:
	_uint m_iPassIndex{};
	_float m_fDeadTime{ 0.f };
	_float m_fDissolveRatio{ 0.f };

	_bool m_bChangePass = false;
	_float m_fHitTime { 0.f };
	_float m_fHittedTime{ 0.f };
	_uint m_iIndex{0};
	_mat GuardMatrix{};

private:
	ANIM_DESC m_Animation{};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_vec4 Compute_PlayerPos();

public:
	HRESULT Add_Collider();
	void Update_Collider();

public:
	static CGuard* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END