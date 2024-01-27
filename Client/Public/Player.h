#pragma once
#include "Client_Define.h"
#include "GameObject.h"
#include "BodyPart.h"
#include "Weapon.h"
#include "Riding.h"
#include "NameTag.h"
#include "CommonTrail.h"
#include "CommonSurfaceTrail.h"

BEGIN(Client)

struct BODYPART_DESC
{
	PART_TYPE eType{};
	_uint iNumVariations{};
	ANIM_DESC* Animation{};
	CTransform* pParentTransform{ nullptr };
};


class CPlayer final : public CGameObject
{
public:
	enum ANIM_LIST
	{
		Anim_Assassin_Attack01_A,
		Anim_Assassin_Attack01_B,
		Anim_Assassin_Attack02_A,
		Anim_Assassin_Attack02_B,
		Anim_Assassin_Attack03_A,
		Anim_Assassin_Attack03_B,
		Anim_Assassin_Attack04_A,
		Anim_Assassin_Attack04_B,
		Anim_Assassin_Attack05_A,
		Anim_Assassin_Attack05_B,
		Anim_Assassin_Battle_Idle,
		Anim_Assassin_Battle_Idle_end,
		Anim_Assassin_Battle_Run,
		Anim_Assassin_Battle_Run_end,
		Anim_Assassin_Battle_Run_L,
		Anim_Assassin_Battle_Run_R,
		Anim_Assassin_knockdown,
		Anim_B_idle_end,
		Anim_B_run_end,
		Anim_Cine_Resurrection,
		Anim_Climb_B,
		Anim_Climb_end_end,
		Anim_Climb_end_loop,
		Anim_Climb_end_run,
		Anim_Climb_end_start,
		Anim_Climb_F,
		Anim_Climb_jump,
		Anim_Climb_L,
		Anim_Climb_R,
		Anim_Climb_start,
		Anim_Climb_wait,
		Anim_Collect_end,
		Anim_Collect_loop,
		Anim_Collect_Start,
		Anim_Create_Idle,
		Anim_Create_Pick,
		Anim_Create_Pick_cancel,
		Anim_Create_Pick_Idle,
		Anim_die,
		Anim_hit_add,
		Anim_ID_7040_EnhancedAttack,
		Anim_ID_7050_Dodge,
		Anim_ID_7060_KnockBack,
		Anim_ID_8040_PoisonArrow,
		Anim_ID_8050_ArchersBlessing,
		Anim_ID_8060_SleepArrow,
		Anim_ID_8070_TripleStrike,
		Anim_ID_8080_BackTumbling,
		Anim_ID_8090_DrainArrow,
		Anim_ID_8100_ExplosiveTrap,
		Anim_ID_8110_DodgeAttack,
		Anim_ID_8120_RainArrow,
		Anim_ID_8125_TargetArrow,
		Anim_ID_8130_IllusionArrow,
		Anim_ID_8135_ChaosSign,
		Anim_ID_Sniper_attack01_A,
		Anim_ID_Sniper_attack02_A,
		Anim_ID_Sniper_attack03_A,
		Anim_ID_Sniper_attack04_A,
		Anim_ID_Sniper_Attack_01_B,
		Anim_ID_Sniper_Attack_02_B,
		Anim_ID_Sniper_Attack_03_B,
		Anim_ID_Sniper_Attack_04_B,
		Anim_idle_00,
		Anim_Idle_01,
		Anim_Idle_10,
		Anim_Idle_11,
		Anim_Idle_Change10,
		Anim_Interactioning,
		Anim_jump_end,
		Anim_jump_end_long,
		Anim_jump_end_run,
		Anim_jump_loop,
		Anim_jump_start,
		Anim_LoadingScene_Pose_Sniper,
		Anim_logging,
		Anim_Mining,
		Anim_Mount_fly_run,
		Anim_Mount_Idle,
		Anim_Mount_Run,
		Anim_Mount_Walk,
		Anim_Normal_run,
		Anim_Normal_Run_Add_L,
		Anim_Normal_Run_Add_R,
		Anim_Normal_run_L,
		Anim_Normal_run_R,
		Anim_Normal_run_start,
		Anim_Normal_run_stop,
		Anim_Normal_Walk,
		Anim_R_N_Move_Walk_F,
		Anim_R_War_absorption,
		Anim_RA_7042_EnhancedAttack,
		Anim_RA_7052_Dodge,
		Anim_RA_7062_KnockBack_A,
		Anim_RA_7062_KnockBack_B,
		Anim_RA_9040_RapidAttack,
		Anim_RA_9040_RapidAttack_End,
		Anim_RA_9040_RapidAttack_Loop,
		Anim_RA_9040_RapidAttack_Start,
		Anim_RA_9050_SealStack,
		Anim_RA_9060_SealChain,
		Anim_RA_9070_BattleStation,
		Anim_RA_9080_Hiding,
		Anim_RA_9090_SealBurst,
		Anim_RA_9095,
		Anim_RA_9100_Ambush,
		Anim_RA_9110_WeakpointAttack,
		Anim_RA_9120_BlindAttack,
		Anim_RA_9125_ThrowingDagger,
		Anim_RA_9130_SealInjection,
		Anim_RA_9135_Dummy,
		Anim_RA_9135_Silhouette,
		Anim_RA_B_Att_Skill_9130_SealInjection_02,
		Anim_RA_B_ChangeJobs_00,
		Anim_RA_Login_00,
		Anim_RA_Pose_00,
		Anim_RA_Pose_01,
		Anim_RA_Pose_02,
		Anim_RA_Pose_03,
		Anim_RA_Pose_04,
		Anim_RA_Pose_05,
		Anim_RA_Pose_06,
		Anim_RA_Pose_07,
		Anim_RA_Pose_08,
		Anim_RA_Pose_09,
		Anim_RA_Pose_10,
		Anim_RA_Pose_11,
		Anim_RA_Select_Pose,
		Anim_RA_Statue,
		Anim_RA_War_Cure,
		Anim_RA_War_Immune,
		Anim_RA_War_Return,
		Anim_RA_War_SmokeShell,
		Anim_RA_War_summon,
		Anim_RA_War_Tornado,
		Anim_RA_WeaponShapeViewer_idle,
		Anim_RandomBox_Rogue_Weapon_Idle,
		Anim_Resurrection,
		Anim_revival_end,
		Anim_revival_fail,
		Anim_revival_fail_loop,
		Anim_revival_loop,
		Anim_revival_start,
		Anim_RS_7040_EnhancedAttack,
		Anim_RS_7050_Dodge,
		Anim_RS_7060_KnockBack,
		Anim_RS_8040_PoisonArrow,
		Anim_RS_8050_ArchersBlessing,
		Anim_RS_8060_SleepArrow,
		Anim_RS_8070_TripleStrike,
		Anim_RS_8070_TripleStrike_Old_2,
		Anim_RS_8080_BackTumbling,
		Anim_RS_8090_DrainArrow,
		Anim_RS_8095,
		Anim_RS_8100_ExplosiveTrap,
		Anim_RS_8110_DodgeAttack,
		Anim_RS_8110_DodgeAttack_02,
		Anim_RS_8110_DodgeAttack_03,
		Anim_RS_8120_RainArrow,
		Anim_RS_8125_TargetArrow,
		Anim_RS_8130_IllusionArrow,
		Anim_RS_8135_ChaosSign,
		Anim_RS_8135_Dummy,
		Anim_RS_B_ChangeJobs_00,
		Anim_RS_Login_00,
		Anim_RS_Pose_00,
		Anim_RS_Pose_01,
		Anim_RS_Pose_02,
		Anim_RS_Pose_03,
		Anim_RS_Pose_04,
		Anim_RS_SelectPose,
		Anim_RS_So_Arena,
		Anim_RS_Statue,
		Anim_RS_War_Cure,
		Anim_RS_War_Grab,
		Anim_RS_War_Immune,
		Anim_RS_War_Return,
		Anim_RS_War_SmokeShell,
		Anim_RS_War_summon,
		Anim_RS_War_Tornado,
		Anim_RS_WeaponShapeViewer_idle,
		Anim_Sniper_attack01_A,
		Anim_Sniper_attack02_A,
		Anim_Sniper_attack03_A,
		Anim_Sniper_attack04_A,
		Anim_Sniper_Attack_01_B,
		Anim_Sniper_Attack_02_B,
		Anim_Sniper_Attack_03_B,
		Anim_Sniper_Attack_04_B,
		Anim_Sniper_B_idle,
		Anim_Sniper_Battle_Idle_end,
		Anim_Sniper_Battle_run,
		Anim_Sniper_Battle_Run_end,
		Anim_Sniper_Battle_run_L,
		Anim_Sniper_Battle_run_R,
		Anim_Sniper_knockdown,
		Anim_stun,
		Anim_Stun_start,
		Anim_swim_booster,
		Anim_swim_collect,
		Anim_swim_dive,
		Anim_swim_F,
		Anim_Swim_Idle,
		Anim_Talk_reaction,
		Anim_Talk_Reaction_D,
		Anim_Talk_Reaction_U,
	};

	enum PLAYER_STATE
	{
		Idle,
		Walk,
		Run_Start,
		Run,
		Run_End,
		Attack,
		Attack_Idle,
		Attack_Run,
		Jump_Start,
		Jump,
		Jump_Run,
		Jump_End,
		Skill1,
		Skill1_End,
		Skill2,
		Skill3,
		Skill4,
		SkillR,
		Aim_Idle,
		Jump_Long_End,
		Mount,
		Climb,
		Climb_U,
		Climb_D,
		Climb_L,
		Climb_R,
		Hit,
		KnockDown,
		Stun,
		Stun_Start,
		Die,
		Revival_Start,
		Revival_End,
		State_End
	};
	struct PLAYER_STATUS
	{
		_int Current_Hp{ 1000 };
		_int Max_Hp{ 1000 };
		_int Max_Mp{ 1000 };
		_int Current_Mp{ 1000 };
		_int Attack{ 100 };
		_int Critical{};
		_int Critical_Dmg{ 150 }; // �⺻ ġ��Ÿ������ 150( �⺻ �������� �߰�50�ۼ�Ʈ ����)
		_int Armor{}; // ������ 10�϶� �޴� ������ 10�ۼ�Ʈ �ٿ���(90�۸� ����)
		_float Speed{}; // �⺻ �ȴ� �̼� 2+���ǵ�/2,�ٴ� �̼� 4+���ǵ�
	};

private:
	CPlayer(_dev pDevice, _context pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Place_PartModels();
	HRESULT Add_Info();
	HRESULT Render_Parts(PART_TYPE Parts, _uint Index);
	HRESULT Add_Riding();

public:
	virtual void Set_Damage(_int iDamage, _uint MonAttType = 0) override;
	void Change_Parts(PART_TYPE PartsType, _int ChangeIndex);
	void Change_Weapon(WEAPON_TYPE PartsType, WEAPON_INDEX ChangeIndex);
	void Move(_float fTimeDelta);

	void Front_Ray_Check();
	_bool Turn_Ray_Check(_bool bRight);
	
	void Health_Regen(_float fTImeDelta);
	void Is_Climb(_float fTimeDelta);
	
	void Common_Attack();
	void Skill1_Attack();
	void Skill2_Attack();
	void Skill3_Attack();
	void Skill4_Attack();
	void SkillR_Attack();

	void Cam_AttackZoom(_float fZoom);
	void Return_Attack_IdleForm();
	void After_CommonAtt(_float fTimeDelta);
	void After_SkillAtt(_float fTimeDelta);
	void Sword_Att_Camera_Effect();
	void Bow_Att_Camera_Effect();
	
	void Check_Att_Collider(ATTACK_TYPE Att_Type);

public:
	void Summon_Riding(Riding_Type Type);
	void Tick_Riding();
	void UnMount_Riding();

public:
	void Init_State();
	void Tick_State(_float fTimeDelta);

private:
	CRiding* m_pRiding{ nullptr };
	CGameObject* m_pNameTag{ nullptr };
	CRealtimeVTFModel* m_pModelCom = { nullptr };
	CTransform* m_pCameraTransform{ nullptr };
	CCollider* m_pHitCollider{ nullptr };
	CCollider* m_pAttCollider[AT_End]{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CRenderer* m_pRendererCom{ nullptr };
	CCommonTrail* m_pLeft_Trail{ nullptr };
	CCommonTrail* m_pRight_Trail{ nullptr };
	CCommonSurfaceTrail* m_pTest_Trail{ nullptr };
	CTexture* m_pDissolveTextureCom{};


private:
	ANIM_DESC m_Animation{};
	PLAYER_STATE m_eState{ Idle };
	PLAYER_STATE m_ePrevState{ Idle };
	WEAPON_TYPE m_Current_Weapon{ WP_END };
	ANIM_LIST m_SwordSkill[5]{};
	ANIM_LIST m_BowSkill[5]{};
	WEAPON_INDEX m_Weapon_CurrentIndex{ WP_INDEX_END };
	Riding_State m_Riding_State{};
	PLAYER_STATUS m_Status{};
	_mat m_Riding_Mat{};
	const _mat* m_Left_Mat{};
	const _mat* m_Right_Mat{};
	_float m_fDissolveRatio{};
	_bool	  m_isInvenActive{ false };
	_float m_fSkillSpeed{};
	_bool m_bAttackStop{};
	_float4 m_vPos{};
	_float m_fGravity{};
	_bool m_bStartGame{};
	_float m_fWalkSpeed{ 1.f };
	_float m_fRunSpeed{ 3.5f };
	_bool m_isInterpolating{};
	_float m_fInterpolationRatio{};
	_vec4 m_vOriginalLook{};
	_float m_iSuperArmor{};
	_float m_fAttTimer{};
	_bool m_bAttacked{};
	_bool m_hasJumped{};
	_int m_iAttackCombo{};
	_int m_iCurrentSkill_Index{};
	_vec4 m_currentDir{};
	_float m_lerpFactor{ 0.1f };
	_vec4 m_vLook{};
	_float m_fLerpTime{};
	_vec4 m_fLerpLook{};
	_vec4 m_fFirstLook{};
	_float m_fAttackZoom{};
	_float m_ReturnZoomTime{};
	_float m_fSkiilTimer{};
	_mat m_OldWorldMatrix{};
	_bool m_bHide{};
	_float m_fHpRegenTime{};
	_float m_fMpRegenTime{};
	_int m_Body_CurrentIndex{ -1 };
	_int m_Helmet_CurrentIndex{ -1 };
	_int m_Hair_CurrentIndex{ -1 };
	_int m_Face_CurrentIndex{ -1 };
	_float m_SkillSpeed{};
	_uint m_ShaderIndex{};
	_bool m_UsingMotionBlur{};
	_bool m_View_Helmat{};
	_vec4 m_SaveCamPos{};
	_vec4 m_SaveCamLook{};
	_bool m_bIsMount{};
	_bool m_bWeapon_Unequip{};
	_bool m_bHelmet_Hide{};
	_bool m_bIsClimb{};
	_bool m_bReady_Climb{};
	_bool m_bReady_Move{};
	_float m_StartRegen{};

	wstring m_strPlayerName{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END