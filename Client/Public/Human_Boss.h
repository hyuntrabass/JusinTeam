#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CHuman_Boss final : public CGameObject
{
public:

	enum HUMANBOSS_ANIM
	{
		BossAnim_attack01, // 전방 낫으로 공격
		BossAnim_attack02,	// 반대 방향으로 공격
		BossAnim_attack03,	// 2연속 공격
		BossAnim_attack04, // 주변 전부 공격
		BossAnim_attack05, // 카운터?
		BossAnim_attack06_End,
		BossAnim_attack06_Loop,
		BossAnim_attack06_Start,
		BossAnim_attack06_Start001,
		BossAnim_attack07,
		BossAnim_attack08,
		BossAnim_attack08_2,
		BossAnim_attack09_A, //반 장판
		BossAnim_attack09_B,
		BossAnim_attack10_A,
		BossAnim_attack10_B,
		BossAnim_attack11_A,
		BossAnim_attack11_B,
		BossAnim_attack12_A,
		BossAnim_attack12_B,
		BossAnim_attack13,
		BossAnim_BossView_Idle,	//레이저
		BossAnim_CardTestattack01,
		BossAnim_CardTestattack02,
		BossAnim_CardTestattack03,
		BossAnim_Die,
		BossAnim_Idle,
		BossAnim_Rage,
		BossAnim_Roar,
		BossAnim_Run,
		BossAnim_Run2,
		BossAnim_Spawn,
		BossAnim_Spawn_Idle,
		BossAnim_Walk,
		BossAnim_End
	};

	enum STATE
	{
		CommonAtt0,	// 전방
		CommonAtt1,	// 후방
		CommonAtt2,	// 전후방
		CounterAtt,
		Hide_Start,
		Hide,
		HideAtt,
		Razer,
		Hit,
		Idle,
		Walk,
		Roar,
		Run,
		Die,
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

public:
	void View_Attack_Range();
	void After_Attack(_float fTimedelta);
	_bool Compute_Angle(_float fAngle);

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pBodyCollider = { nullptr };
	CCollider* m_pCommonAttCollider = { nullptr };
	class CEffect_Dummy* m_pBaseEffect{ nullptr };
	class CEffect_Dummy* m_pFrameEffect{ nullptr };
	class CEffect_Dummy* m_pDimEffect{ nullptr };
	class CEffect_Dummy* m_pAttackEffect{ nullptr };
	class CEffect_Dummy* m_pShieldEffect{ nullptr };
	CTexture* m_pDissolveTextureCom{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

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
	_bool m_bShieldOn{};
	_float m_fHitTime{};
	_mat m_BaseEffectMat{};
	_mat m_AttEffectMat{};
	_mat m_ShieldEffectMat{};
	_mat m_AttEffectOriMat{};
	_mat m_BaseEffectOriMat{};
	_float m_fBaseEffectScale{};
	_uint m_iPassIndex{};
	_uint m_iWeaponPassIndex{};
	_bool m_bViewWeapon{};
	_bool m_bAttacked{};
	_float m_fDissolveRatio{};
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHuman_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END