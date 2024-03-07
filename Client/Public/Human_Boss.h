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
		BossAnim_attack01, // ���� ������ ����
		BossAnim_attack02,	// �ݴ� �������� ����
		BossAnim_attack03,	// 2���� ����
		BossAnim_attack04, // �ֺ� ���� ����
		BossAnim_attack05, // ī����
		BossAnim_attack06_End, //���� ����
		BossAnim_attack06_Loop,
		BossAnim_attack06_Start, //����
		BossAnim_attack06_Start001,
		BossAnim_attack07, //�� ���ۺ���
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
		BossAnim_attack13, // ����
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
		BossAnim_Spawn, // ����
		BossAnim_Spawn_Idle,
		BossAnim_Walk,
		BossAnim_End
	};

	enum STATE
	{
		CommonAtt0,	// ���� ����
		CommonAtt1,	// �Ĺ� ����
		CommonAtt2,	// ��ü ����(�ݶ��̴���ŭ)
		Counter_Start,	//�� ������ ī���� or �ݻ� ���� ����
		Counter_Fail,	// �÷��̾ ī���� ������ ����
		Hide_Start,	// ��ŻŸ�� �����
		Hide,	//����� ����
		Hide_Att,	 // ��������¿��� �������
		Throw_Sickle, // �� ������
		Pizza_Start,
		Pizza_Loop,	//���� �������� ����
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