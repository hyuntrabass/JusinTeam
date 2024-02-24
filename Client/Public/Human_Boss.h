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
		CommonAtt0,	// 전방 공격
		CommonAtt1,	// 후방 공격
		CommonAtt2,	// 전체 공격(콜라이더만큼)
		Counter_Start,	//기 모으고 카운터 or 반사 패턴 진행
		Counter_Fail,	// 플레이어가 카운터 성공시 기절
		Hide_Start,	// 포탈타고 사라짐
		Hide,	//사라진 상태
		Hide_Att,	 // 사라진상태에서 기습공격
		Razer,
		Pizza, // 반시계방향으로 피자조각 범위  공격
		Hit,
		Idle,
		Walk,
		Roar,
		Run,
		Die,
		Spwan,
		BOSS_STATE_END
	};
	
	enum ATTACK_RANGE
	{
		Range_45,
		Range_90,
		Range_135,
		Range_180,
		Range_360,
		Range_End,	
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
	void View_Attack_Range(ATTACK_RANGE Range,_float fRotationY = 0.f);
	void After_Attack(_float fTimedelta);
	_bool Compute_Angle(_float fAngle, _float RotationY = 0.f);
	void Increased_Range(_float Index, _float fTImeDelta,_float fRotationY = 0.f);
private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pBodyCollider = { nullptr };
	CCollider* m_pCommonAttCollider = { nullptr };
	CTexture* m_pDissolveTextureCom{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

	class CEffect_Dummy* m_pBaseEffect{ nullptr };
	class CEffect_Dummy* m_pFrameEffect{ nullptr };
	class CEffect_Dummy* m_pDimEffect{ nullptr };
	class CEffect_Dummy* m_pAttackEffect{ nullptr };
	class CEffect_Dummy* m_pRingEffect{ nullptr };
	class CEffect_Dummy* m_pCounterEffect{ nullptr };

private:
	ANIM_DESC m_Animation{};
	STATE m_eState = BOSS_STATE_END;
	STATE m_ePreState = BOSS_STATE_END;
private:
	
	_bool m_bSecondPattern{};
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };
	_bool m_bCounter_Success{};
	_float m_fHitTime{};
	_float m_fHideTimmer{};
	_bool m_bChangePass{};
	_bool m_bHide{};
	_bool m_bLeftPattern{};
	_bool m_bAttacked{};
	_uint m_iPassIndex{};
	_float m_fAttackRange{};
	_bool m_bViewWeapon{};
	_float m_fDissolveRatio{};
	_uint m_iWeaponPassIndex{};
	_vec4 m_vRimColor{};
	_bool m_bReflectOn{};
	_mat m_AttEffectMat{};
	_mat m_RingEffectMat{};
	_mat m_BaseEffectMat{};
	_mat m_AttEffectOriMat{};
	_mat m_BaseEffectOriMat{};
	_float m_fBaseEffectScale{};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHuman_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END