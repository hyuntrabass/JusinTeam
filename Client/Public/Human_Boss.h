#pragma once

#include "Client_Define.h"
#include "GameObject.h"
#include "CommonSurfaceTrail.h"
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
		BossAnim_attack05, // 카운터
		BossAnim_attack06_End, //텔포 등장
		BossAnim_attack06_Loop,
		BossAnim_attack06_Start, //텔포
		BossAnim_attack06_Start001,
		BossAnim_attack07, //낫 빙글빙글
		BossAnim_attack08,
		BossAnim_attack08_2,
		BossAnim_attack09_A,
		BossAnim_attack09_B,
		BossAnim_attack10_A,
		BossAnim_attack10_B,
		BossAnim_attack11_A,
		BossAnim_attack11_B,
		BossAnim_attack12_A,
		BossAnim_attack12_B,
		BossAnim_attack13, // 피자
		BossAnim_BossView_Idle,
		BossAnim_CardTestattack01,
		BossAnim_CardTestattack02,
		BossAnim_CardTestattack03,
		BossAnim_Die,
		BossAnim_Idle,
		BossAnim_Rage,
		BossAnim_Roar,
		BossAnim_Run,
		BossAnim_Run2,
		BossAnim_Spawn, // 등장
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
		Throw_Sickle, // 낫 날리기
		Pizza_Start,
		Pizza_Loop,	//피자 패턴으로 공격
		Pizza_BackLoop,
		Pizza_End,
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
	virtual void Set_Damage(_int iDamage, _uint MonAttType = 0) override;

private:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	HRESULT Add_Collider();
	void Update_Collider();
	void Update_Trail();

private:
	void Set_Pattern();
	_float Compute_Distance();
	void After_Attack(_float fTimedelta);
	_bool Compute_Angle(_float fAngle, _float RotationY = 0.f);
	void View_Attack_Range(ATTACK_RANGE Range, _float fRotationY = 0.f, _bool bPizza = false);
	void Increased_Range(_float Index, _float fTImeDelta,_float fRotationY = 0.f);

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pBodyCollider = { nullptr };
	CCollider* m_pCommonAttCollider = { nullptr };
	CTexture* m_pDissolveTextureCom{ nullptr };
	CTransform* m_pPlayerTransform{ nullptr };

	class CHPBoss* m_pHpBoss{ nullptr };
	class CEffect_Dummy* m_pBaseEffect{ nullptr };
	class CEffect_Dummy* m_pFrameEffect{ nullptr };
	class CEffect_Dummy* m_pDimEffect{ nullptr };
	class CEffect_Dummy* m_pAttackEffect{ nullptr };
	class CEffect_Dummy* m_pRingEffect{ nullptr };
	class CEffect_Dummy* m_pCounterEffect{ nullptr };
	CCommonSurfaceTrail* m_pWeapon_Trail{ nullptr };
	CCommonSurfaceTrail* m_pWeapon_Distortion_Trail{ nullptr };

private:
	ANIM_DESC m_Animation{};
	STATE m_eState = BOSS_STATE_END;
	STATE m_ePreState = BOSS_STATE_END;
	vector<int> m_vecPattern{};
	_bool m_HasPlayedSound{};

private:
	const _mat* m_WeaponBone_Mat{};
	_uint m_iPizzaAttCount{};
	_uint m_iAttackPattern{};
	_bool m_bSelectAttackPattern{};
	_bool m_bCounter_Success{};
	_bool m_bViewModel{true};
	_float m_fHitTime{};
	_float m_fHideTimmer{};
	_bool m_bChangePass{};
	_bool m_bHide{};
	_bool m_bLeftPattern{};
	_bool m_bAttacked{};
	_uint m_iPassIndex{};
	_float m_fAttackRange{};
	_bool m_bViewWeapon{};
	_float m_fWeaponDissolveRatio{};
	_float m_fModelDissolveRatio{};
	_uint m_iWeaponPassIndex{};
	_float m_fPatternDelay{};
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