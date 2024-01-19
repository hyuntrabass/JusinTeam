#pragma once
#include "Client_Define.h"
#include "GameObject.h"
#include "BodyPart.h"
#include "Weapon.h"
#include "NameTag.h"
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
	Anim_Collect_loop, //채집
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
	Anim_jump_end,												// 점프
	Anim_jump_end_long,
	Anim_jump_end_run,
	Anim_jump_loop,
	Anim_jump_start,
	Anim_LoadingScene_Pose_Sniper,
	Anim_logging,												// 나무베는거
	Anim_Mining,												// 광물캐는거
	Anim_Mount_fly_run,
	Anim_Mount_Idle,
	Anim_Mount_Run,
	Anim_Mount_Walk,											
	Anim_Normal_run,											// 기본 움직임
	Anim_Normal_run_L,
	Anim_Normal_run_R,
	Anim_Normal_run_start,
	Anim_Normal_run_stop,
	Anim_Normal_Walk,
	Anim_R_So_Customize_Pose01_End,
	Anim_R_So_Customize_Pose01_Loop,
	Anim_R_So_Customize_Pose01_Start,
	Anim_R_So_Customize_Pose02_End,
	Anim_R_So_Customize_Pose02_Loop,
	Anim_R_So_Customize_Pose02_Start,
	Anim_R_So_Customize_Pose03_End,
	Anim_R_So_Customize_Pose03_Loop,
	Anim_R_So_Customize_Pose03_Start,
	Anim_R_So_Customize_Pose04_End,
	Anim_R_So_Customize_Pose04_Loop,
	Anim_R_So_Customize_Pose04_Start,
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
	Anim_end
};



	struct WEAPONPART_DESC
	{
		_uint iNumVariations{};
		ANIM_DESC* Animation{};
		CTransform* pParentTransform{ nullptr };
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
		Jump_Start,
		Jump,
		Jump_End,
		State_End
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
	HRESULT Add_Parts();
	HRESULT Add_Weapon();
	HRESULT Add_Info();

public:
	void Change_Parts(PART_TYPE PartsType,_int ChangeIndex);
	void Change_Weapon(WEAPON_TYPE PartsType,_int ChangeIndex);

	void Reset_PartsAnim();
	void Set_Key(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Common_SwordAttack();
	void Return_Attack_IdleForm();
	void Sword_Attack_Dash(_float fTimeDelta);
public:
	void Init_State();
	void Tick_State(_float fTimeDelta);
private:
	vector<CBodyPart*> m_vecParts{};
	CGameObject* m_pWeapon{};
	CGameObject* m_pNameTag{};
	CTransform* m_pCameraTransform{};
private:
	ANIM_DESC m_Animation{};
	PLAYER_STATE m_eState{ Idle };
	PLAYER_STATE m_ePrevState{ Idle };
	_float4 m_vPos{};
	_float m_fGravity{};
	WEAPON_TYPE m_Current_Weapon{WP_END};
	_bool m_bStartGame{};
	_float m_fWalkSpeed{2.f};
	_float m_fRunSpeed{4.f};

	_bool m_isInterpolating{};
	_float m_fInterpolationRatio{};
	_vec4 m_vOriginalLook{};
	_float m_iSuperArmor{};
	_float m_fAttTimer{};
	_bool m_bAttacked{};
	_bool m_hasJumped{};
	_int m_iAttackCombo{};

	_vec4 m_currentDir{};
	_float m_lerpFactor{0.1f};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END