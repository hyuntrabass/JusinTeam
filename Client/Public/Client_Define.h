#pragma once

#include "../Default/framework.h"
#include <process.h>
#include "Engine_Defines.h"
#include "Transform.h"
#include <Windows.h>
#include <Commdlg.h>
#include "Model.h"
#pragma comment(lib, "Engine.lib")
#pragma comment(lib, "Imm32.lib")

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
	static const POINT g_ptCenter = { static_cast<signed long>(g_iWinSizeX * 0.5f), static_cast<signed long>(g_iWinSizeY * 0.5f) };

	static const LIGHT_DESC g_Light_Custom
	{
		LIGHT_DESC::Directional,
		_vec4(0.f, 0.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(1.f), // Diffuse
		_vec4(0.5f), // Ambient
		_vec4(0.2f) // Specular
	};

	static const LIGHT_DESC g_Light_Tutorial
	{
		LIGHT_DESC::Directional,
		_vec4(-1.f, -2.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(0.2f), // Diffuse
		_vec4(0.01f), // Ambient
		_vec4(1.f) // Specular
	};

	static const LIGHT_DESC g_Light_Village
	{
		LIGHT_DESC::Directional,
		_vec4(-1.f, -2.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(1.f), // Diffuse
		_vec4(0.3f), // Ambient
		_vec4(0.5f) // Specular
	};

	static const LIGHT_DESC g_Light_Dungeon
	{
		LIGHT_DESC::Directional,
		_vec4(-1.f, -2.f, -1.f, 0.f), // Direction
		_vec4(), // Position
		_vec4(), // Attenuation

		_vec4(0.05f), // Diffuse
		_vec4(0.01f), // Ambient
		_vec4(0.1f) // Specular
	};

	enum LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_LOGO,
		LEVEL_SELECT,
		LEVEL_CUSTOM,
		LEVEL_GAMEPLAY,
		LEVEL_VILLAGE,
		LEVEL_END
	};

	enum CAMERA_MODE
	{
		CM_DEBUG,
		CM_MAIN,
		CM_SELECT,
		CM_CUSTOM,
		CM_CUTSCENE,
		CM_END
	};
	enum CAMERA_STATE
	{
		CS_DEFAULT,
		CS_ZOOM,
		CS_INVEN,
		CS_SHOP,
		CS_SKILLBOOK,
		CS_ENDFULLSCREEN,
		CS_WORLDMAP,
		CS_STATEEND
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
	enum WEAPON_TYPE
	{
		WP_BOW,
		WP_SWORD,
		WP_END
	};
	enum WEAPON_INDEX
	{
		BOW0,
		BOW1,
		BOW2,
		BOW3,
		BOW4,
		SWORD0,
		SWORD1,
		SWORD2,
		SWORD3,
		SWORD4,
		WP_UNEQUIP,
		WP_INDEX_END
	};

	enum ATTACK_TYPE  //�������ִ� Ÿ��
	{
		AT_Sword_Common,// ����
		AT_Sword_Skill1,// ����
		AT_Sword_Skill2,// ����
		AT_Sword_Skill3,// ����
		AT_Sword_Skill4,// ����
		AT_Bow_Common,// �з�����
		AT_Bow_Skill1,// �з�����
		AT_Bow_Skill2,// ����
		AT_Bow_Skill3_Start,
		AT_Bow_Skill3,// �̼� ��������
		AT_Bow_Skill4,// ����
		AT_Bow_SkillR,// ����

		AT_End
	};

	struct WEAPONPART_DESC
	{
		_uint iNumVariations{};
		ANIM_DESC* Animation{};
		CTransform* pParentTransform{ nullptr };
	};
	enum PART_TYPE
	{
		PT_HAIR,
		PT_FACE,
		PT_BODY,
		PT_HELMET,
		PT_WEAPON,
		PT_END
	};

	enum VEHICLE_TYPE
	{
		VC_GROUND,
		VC_FLY,
		VC_END
	};

	/*enum SoundChannel
	{
		SCH_BGM,
		SCH_EFFECT_PLAYER,
		SCH_SHOOK,
		SCH_PAK,
		SCH_JUMP,
		SCH_STEP_L,
		SCH_STEP_R,
		SCH_EFFECT_SKILL,
		SCH_EFFECT_SKILL1,
		SCH_EFFECT_SKILL2,
		SCH_EFFECT_SKILL3,
		SCH_EFFECT_SKILL4,
		SCH_EFFECT_SKILL5,
		SCH_EFFECT_SKILL6,
		SCH_EFFECT_SKILL7,
		SCH_EFFECT_SKILL8,
		SCH_EFFECT_SKILL9,
		SCH_EFFECT_SKILL10,
		SCH_EFFECT_SKILL11,
		SCH_EFFECT,
		SCH_EFFECT2,
		SCH_EFFECT3,
		SCH_AMBIENT,
		SCH_BUTTON,
		SCH_NPC,
		SCH_NPC2,
		SCH_EFFECT_MONSTER0,
		SCH_EFFECT_MONSTER1,
		SCH_EFFECT_MONSTER2,
		SCH_EFFECT_MONSTER3,
		SCH_EFFECT_MONSTER4,
		SCH_EFFECT_MONSTER5,
		SCH_EFFECT_MONSTER6,
		SCH_EFFECT_MONSTER7,
		SCH_EFFECT_MONSTER8,
		SCH_EFFECT_MONSTER9,
		SCH_EFFECT_MONSTER10,
		SCH_EFFECT_MONSTER11,
		SCH_EFFECT_MONSTER12,
		SCH_EFFECT_MONSTER13,
		SCH_EFFECT_MONSTER14,
		SCH_EFFECT_MONSTER15,
		SCH_EFFECT_MONSTER16,
		SCH_EFFECT_MONSTER17,
		SCH_EFFECT_MONSTER18,
		SCH_EFFECT_MONSTER19,
		SCH_MAX
	}; */

	enum SortDepth
	{
		D_MOUSE,
		D_FADE,
		D_LOADING,
		D_ALERT,
		D_INVEN,
		D_SHOP,
		D_QUEST,
		D_MENU,
		D_TALK,
		D_JOBMARK,
		D_BAR,
		D_SCREEN,
		D_NAMETAG,
		D_END
	};

	enum INVEN_TYPE
	{
		INVEN_ALL,
		INVEN_WEARABLE,
		INVEN_EQUIP,
		INVEN_EXPENDABLES,
		INVEN_END
	};
	enum ITEM_TYPE
	{
		ITEM_TOP,
		ITEM_BODY,
		ITEM_HAND,
		ITEM_FOOT,
		ITEM_POTION,
		ITEM_SWORD,
		ITEM_BOW,
		ITEM_INGREDIENT,
		ITEMTYPEEND
	};

	enum ITEM_TIER
	{
		TIER_COMMON,
		TIER_UNCOMMON,
		TIER_RARE,
		TIER_UNIQUE,
		TIER_LEGENDARY,
		TIER_END
	};

	enum WEARABLE_TYPE
	{
		W_TOP,
		W_CHEST,
		W_HAND,
		W_FOOT,
		W_EQUIP,
		W_PET,
		W_END
	};

	enum TUTO_SEQ
	{
		T_OPENINVEN,
		T_EQUIP,
		T_EXIT,
		T_OPENSKILL,
		T_EQUIPSKILL,
		T_SKILLEXIT,
		TUTO_END
	};

#pragma region Objects
	enum ObjectType
	{
		Object_Building,
		Object_Environment,
		Object_Etc,
		Object_End
	};

	struct ObjectInfo
	{
		wstring strPrototypeTag{};
		_vec4 vPos{ _vec4(0.f, 0.f, 0.f, 1.f) };
		_vec4 vLook{ _vec4(0.f, 0.f, 1.f, 0.f) };
		_mat m_WorldMatrix{};
		ObjectType eObjectType{ };
	};
	enum Monster_AttType
	{
		MonAtt_Hit,
		MonAtt_KnockDown,
		MonAtt_Stun,
		MonAtt_Poison,
		Parrying_Succescc,
		MonAtt_End
	};
#pragma endregion

	struct UiInfo
	{
		_uint iLevel{};
		_vec2 vPos{};
		_vec2 vSize{};
		wstring strTexture{};
	};

	struct AttachmentInfo
	{
		CTransform* pParent{ nullptr };
		_mat* pSocket{ nullptr };
		_mat PivotMatrix{};
	};

	typedef struct tagItemInfo
	{
		wstring strName;

		_uint iInvenType;
		_uint iItemType;
		_uint iItemTier;

		wstring strTexture;
		_uint iIndex;

		_uint iStatus;
		_uint iPurchase;
		_uint iSale;
		_uint iPartIndex{};
		_uint iLimited;

	}ITEM;

	typedef struct tagSkillInfo
	{
		wstring strName;
		_uint iSkillType;
		_uint iMp;
		_uint iCoolTime;
		_uint iSkillIdx;
		_bool isSkillIn;
		wstring strTexture;
		_uint iModelSkillIndex;
	}SKILLINFO;

#pragma region Shader Passes
	enum AnimMeshPass
	{
		AnimPass_Default,
		AnimPass_OutLine,
		AnimPass_Shadow,
		AnimPass_Dissolve,
		AnimPass_Rim,
	};

	enum StaticMeshPass
	{
		StaticPass_Default,
		StaticPass_NonLight,
		StaticPass_OutLine,
		StaticPass_AlphaTestMeshes,
		StaticPass_Sky,
		StaticPass_COLMesh,
		StaticPass_SingleColorFx,
		StaticPass_SingleColorDissolve,
		StaticPass_Fireball,
		StaticPass_MaskEffect,
		StaticPass_MaskDissolve,
		StaticPass_MaskEffectClamp,
		StaticPass_SingleColoredEffectFrontCull,
		StaticPass_Shadow,
		StaticPass_Water,
		StaticPass_Worldmap_Water,
		StaticPass_Worldmap_Cloud,
		StaticPsss_Rim,
		StaticPass_DiffEffect,
		StaticPass_SingleColorAlpha,
		StaticPass_MaskAlpha,
		StaticPass_DiffAlpha,
		StaticPass_End,
	};

	enum VTFModelPass
	{
		VTFPass_Default,
		VTFPass_Dissolve,
		VTFPass_Motion_Blur,
		VTFPass_LerpDissolve,
		VTFPass_LerpBlur,
		VTFPass_Main_Rim,

	};
	enum VNTPass
	{
		VNTPass_Terrain,
		VNTPass_Terrain_Editor,
		VNTPass_Terrain_Effect,
	};

	enum VTPass
	{
		VTPass_UI,
		VTPass_UI_Alpha,
		VTPass_UI_Color_Alpha,
		VTPass_Button,
		VTPass_Background,
		VTPass_Background_Mask,
		VTPass_Mask_Texture,
		VTPass_Inv_Mask_Texture,
		VTPass_Mask_Color,
		VTPass_Mask_ColorAlpha,
		VTPass_HP,
		VTPass_Hit,
		VTPass_Sprite,
		VTPass_SpriteMaskTexture,
		VTPass_SpriteMaskColor,
		VTPass_Hell,
		VTPass_Mask_Texture_Dissolve,
		VTPass_Inv_Mask_Texture_Dissolve,
		VTPass_Mask_Color_Dissolve,
		VTPass_Sprite_Dissolve,
		VTPass_SpriteMaskTexture_Dissolve,
		VTPass_SpriteMaskColor_Dissolve,
		VTPass_Dust,
		VTPass_Dissolve,
		VTPass_MP,
		VTPass_BLUR,
		VTPass_Bright,
		VTPass_ScrollAlpha,
		VTPass_HPNoMask,
		VTPass_NineSlice,
		VTPass_FadeVertical,
		VTPass_FadeHorizontal,
		VTPass_LerpColorNAlpha,
		VTPass_HPBoss,
		VTPass_MaskColorMove,
		VTPass_ChangeBright,
		VTPass_End
	};

	enum InstancingPass
	{
		InstPass_Particle_TextureMask,
		InstPass_Particle_Sprite_Color,
		InstPass_Particle_MaskColor,
		InstPass_Particle_TextureMask_Dissolve,
		InstPass_Particle_Sprite_Color_Dissolve,
		InstPass_Particle_MaskColor_Dissolve,
		InstPass_Particle_Sprite_Texture,
		InstPass_Particle_Sprite_Texture_Dissolve,
		InstPass_Particle_Sprite_Texture_RandomIndex,
		InstPass_Particle_Sprite_Texture_RandomIndex_Dissolve,
		InstPass_Particle_Sprite_Color_RandomIndex,
		InstPass_Particle_Sprite_Color_RandomIndex_Dissolve,
		InstPass_Particle_TextureMask_Trail,
		InstPass_Particle_Sprite_Color_Trail,
		InstPass_Particle_MaskColor_Trail,
		InstPass_Particle_TextureMask_Dissolve_Trail,
		InstPass_Particle_Sprite_Color_Dissolve_Trail,
		InstPass_Particle_MaskColor_Dissolve_Trail,
		InstPass_Particle_Sprite_Texture_Trail,
		InstPass_Particle_Sprite_Texture_Dissolve_Trail,
		InstPass_Particle_Sprite_Texture_RandomIndex_Trail,
		InstPass_Particle_Sprite_Texture_RandomIndex_Dissolve_Trail,
		InstPass_Particle_Sprite_Color_RandomIndex_Trail,
		InstPass_Particle_Sprite_Color_RandomIndex_Dissolve_Trail,
		InstPass_Particle_Sprite_DiffMask,
		InstPass_Particle_Sprite_DiffMask_Dissolve,
		InstPass_Particle_Sprite_DiffMask_RandomIndex,
		InstPass_Particle_Sprite_DiffMask_RandomIndex_Dissolve,
		InstPass_Particle_Sprite_DiffMask_Trail,
		InstPass_Particle_Sprite_DiffMask_Dissolve_Trail,
		InstPass_Particle_Sprite_DiffMask_RandomIndex_Trail,
		InstPass_Particle_Sprite_DiffMask_RandomIndex_Dissolve_Trail,
		InstPass_End,
	};
#pragma endregion
#pragma region �ƾ�
	struct SectionInfo
	{
		//_vec4 vStartCutScene{};
		//_vec4 vEndCutScene{};
		_mat mCutSceneMatrix{};
		_uint iSectionType{};
		class CCutScene_Curve** ppCurve{ nullptr };
	};
}
#pragma endregion

#pragma region Ʈ����
struct TriggerInfo
{
	_bool bLimited{};
	_int iIndex{};
	_float fSize{};
	_mat WorldMat{};
};

enum TriggerType
{
	VILLAGE_TRIGGER,
	FRONTDOOR_IN_TRIGGER,
	FRONTDOOR_OUT_TRIGGER,
	BACKDOOR_IN_TRIGGER,
	BACKDOOR_OUT_TRIGGER,
	BOSS_TRIGGER,
	TRIGGER_END
};
#pragma endregion



extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;