#include "Player.h"
#include "BodyPart.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Weapon.h"
CPlayer::CPlayer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Init_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_pTransformCom->Set_Scale(_vec3(4.f));
	Place_PartModels();
	m_pTransformCom->Set_Speed(1);
	m_pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	Safe_AddRef(m_pCameraTransform);
	m_SwordSkill[0] = Anim_RA_7062_KnockBack_A; // 검기 날리기 (주력기)
	m_SwordSkill[1] = Anim_RA_9060_SealChain; // 앞으로 점프하면서 때리기
	m_SwordSkill[2] = Anim_RA_9040_RapidAttack; // 사라졌다가 찌르기
	m_SwordSkill[3] = Anim_RA_9050_SealStack; // 난타(쿨김)
	m_SwordSkill[4] = Anim_RA_9080_Hiding; // 은신(우클릭)

	m_BowSkill[0] = Anim_ID_8070_TripleStrike; // 트리플 샷 (주력기)
	m_BowSkill[1] = Anim_ID_8080_BackTumbling; // 백덤블링
	m_BowSkill[2] = Anim_ID_8120_RainArrow; // 화살비
	m_BowSkill[3] = Anim_ID_8130_IllusionArrow; // 분신 나와서 화살(쿨김)
	m_BowSkill[4] = Anim_ID_7060_KnockBack; // 에임모드 변경(우클릭)

	Change_Parts(PT_BODY, 0);
	Change_Parts(PT_HAIR, 0);
	Change_Parts(PT_FACE, 0);

	m_pGameInstance->Register_CollisionObject(this, m_pHitCollider, true);
	

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	m_pGameInstance->Set_TimeRatio(1.0f);

	if (m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG)
		return;

	PART_TYPE eType = CUI_Manager::Get_Instance()->Is_CustomPartChanged();

	if (PART_TYPE::PT_END != eType)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		Change_Parts(eType, CUI_Manager::Get_Instance()->Get_CustomPart(eType));
	}

	m_fAttTimer += fTimeDelta;
	m_fSkiilTimer += fTimeDelta;
	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_CUSTOM)
	{	
		Move(fTimeDelta);
		Init_State();
		Tick_State(fTimeDelta);
	}
	Attack_Camera_Effect();
	_float fMouseSensor = 0.1f;
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CUSTOM)
	{
		m_Animation.bSkipInterpolation = true;
		m_Animation.isLoop = true;
		m_Animation.iAnimIndex = Anim_idle_00;
		if (!CUI_Manager::Get_Instance()->Is_Picking_UI() && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
		{
			_long dwMouseMove;

			if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -1.f * 0.1f);
			}
		}
	}
	_vec4 vScale = m_pTransformCom->Get_Scale();

	After_CommonAtt(fTimeDelta);
	After_SkillAtt(fTimeDelta);

	m_pHitCollider->Update(m_pTransformCom->Get_World_Matrix());

	for (int i = 0; i < AT_Bow_Common; i++)
	{
		_mat offset = /*_mat::CreateScale(_vec3(1.f,6.5f,1.f))* */_mat::CreateTranslation(_vec3(0.f, 1.f, 0.f));
		m_pAttCollider[i]->Update(offset *m_pTransformCom->Get_World_Matrix());
	}

	if (m_pNameTag != nullptr)
		m_pNameTag->Tick(fTimeDelta);

	if (m_bStartGame)
	{
		CEvent_Manager::Get_Instance()->Tick(fTimeDelta);
	}

	if (m_eState==Skill4&&!m_bAttacked)
	{
		m_pGameInstance->Set_TimeRatio(0.08f);
		//m_pGameInstance->Set_ShakeCam(true,0.01f);
	}
	m_pModelCom->Set_Animation(m_Animation);

	if (m_UsingMotionBlur)
		m_ShaderIndex = 1;
	else
		m_ShaderIndex = 0;
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (!m_bStartGame && m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
	{

		m_pTransformCom->Set_Scale(_vec3(1.0f));
		Add_Info();

		m_bStartGame = true;
		CEvent_Manager::Get_Instance()->Init();
		CEvent_Manager::Get_Instance()->Set_Quest(TEXT("공격하기"));
		CEvent_Manager::Get_Instance()->Set_Quest(TEXT("이동하기"));
		CEvent_Manager::Get_Instance()->Set_Quest(TEXT("몬스터와 접촉"));

		Change_Weapon(WP_SWORD,SWORD0);
	}


#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pHitCollider);

	//for (int i = 0; i < AT_End; i++)
	{
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Common]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill1]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill2]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill3]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill4]);
	}

#endif // DEBUG


	if (m_bStartGame)
	{
		CEvent_Manager::Get_Instance()->Late_Tick(fTimeDelta);
	}

	if (m_pNameTag != nullptr)
		m_pNameTag->Late_Tick(fTimeDelta);

	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CPlayer::Render()
{
	if (m_bHide)
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
		if(m_Body_CurrentIndex>=0)
		Render_Parts(PT_BODY, m_Body_CurrentIndex);
		if (m_Hair_CurrentIndex >= 0)
		Render_Parts(PT_HAIR, m_Hair_CurrentIndex);
		if (m_Face_CurrentIndex >= 0)
		Render_Parts(PT_FACE, m_Face_CurrentIndex);

		if (m_Weapon_CurrentIndex != WP_INDEX_END)
			Render_Parts(PT_WEAPON, (_uint)m_Weapon_CurrentIndex);

		if (m_View_Helmat)
			Render_Parts(PT_HELMET, 0);

	return S_OK;
}

HRESULT CPlayer::Add_Info()
{
	CNameTag::NAMETAG_DESC NameTagDesc = {};
	NameTagDesc.eLevelID = LEVEL_STATIC;
	NameTagDesc.fFontSize = 0.32f;
	NameTagDesc.pParentTransform = m_pTransformCom;
	NameTagDesc.strNameTag = TEXT("플레이어");
	NameTagDesc.vColor = _vec4(0.5f, 0.7f, 0.5f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 1.9f);
	
	m_pNameTag = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NameTag"), & NameTagDesc);
	if (not m_pNameTag)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer::Place_PartModels()
{
	CRealtimeVTFModel::PART_DESC Desc{};
	Desc.ePartType = PT_BODY; 

	Desc.FileName = "body0";

	if (FAILED(m_pModelCom->Place_Parts(Desc, true)))
		return E_FAIL;

	Desc.FileName = "body1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "body2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = PT_HELMET;
	Desc.FileName = "helmet";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = PT_FACE;
	Desc.FileName = "face0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = PT_HAIR;
	Desc.FileName = "hair0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair6";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair7";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair8";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = PT_WEAPON;
	
	Desc.FileName = "bow0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "bow1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "bow2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "sword0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "sword1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "sword2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Render_Parts(PART_TYPE Parts, _uint Index)
{
 
 for (size_t k = 0; k < m_pModelCom->Get_Num_PartMeshes(Parts, Index); k++)
 { 
	 if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_DiffuseTexture", TextureType::Diffuse, (_uint)Parts, (_uint)Index, k)))
		continue;
 
	_bool HasNorTex{}; 
	if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_NormalTexture", TextureType::Normals, (_uint)Parts, (_uint)Index, k)))
	{ 
		HasNorTex = false; 
	} 
	else 
	{ 
		HasNorTex = true; 
	}
 
	_bool HasSpecTex{}; 
	if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_SpecTexture", TextureType::Shininess, (_uint)Parts, (_uint)Index, k)))
	{ 
		HasSpecTex = false; 
	} 
	else 
	{ 
		HasSpecTex = true; 
	}
 
	if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool))) 
		return E_FAIL; 
 
	HasSpecTex = false;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_HasSpecTex", &HasSpecTex, sizeof _bool))) 
		return E_FAIL; 
	
 
	if (FAILED(m_pShaderCom->Begin(m_ShaderIndex)))
		return E_FAIL;
 
	if (FAILED(m_pModelCom->Render_Part((_uint)Parts,(_uint)Index, k)))
		return E_FAIL;
 }
 
	return S_OK;
}

void CPlayer::Change_Parts(PART_TYPE PartsType, _int ChangeIndex)
{
	switch (PartsType)
	{
	case Client::PT_HAIR:
		m_Hair_CurrentIndex = ChangeIndex;
		break;
	case Client::PT_FACE:
		m_Face_CurrentIndex = ChangeIndex;
		break;
	case Client::PT_BODY:
		m_Body_CurrentIndex = ChangeIndex;
		break;
	default:
		break;
	}
}

void CPlayer::Change_Weapon(WEAPON_TYPE PartsType, WEAPON_INDEX ChangeIndex)
{
	m_Weapon_CurrentIndex = ChangeIndex;
	m_Current_Weapon = PartsType;
}


void CPlayer::Move(_float fTimeDelta)
{

	_bool hasMoved{};
	_vec4 vForwardDir = m_pGameInstance->Get_CameraLook();
	vForwardDir.y = 0.f; 
	_vec4 vRightDir = XMVector3Cross(m_pTransformCom->Get_State(State::Up), vForwardDir);
	vRightDir.Normalize();

	_vec4 vDirection{};
	_bool go{};
	if (m_eState == SkillR || m_eState == Aim_Idle)
	{
		m_pTransformCom->LookAt_Dir(m_pCameraTransform->Get_State(State::Look));

	}
	if (m_pGameInstance->Key_Down(DIK_1))
	{
		CEvent_Manager::Get_Instance()->Update_Quest(TEXT("공격하기"));
		if(m_eState!= Skill1)
		{
			m_eState = Skill1;
			m_iCurrentSkill_Index = Skill1;
			m_bAttacked = false;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_2))
	{
		if (m_eState != Skill2)
		{
			m_eState = Skill2;
			m_iCurrentSkill_Index = Skill2;
			m_bAttacked = false;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_3))
	{
		if (m_eState != Skill3)
		{
			m_eState = Skill3;
			m_iCurrentSkill_Index = Skill3;
			m_bAttacked = false;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_4))
	{
		if (m_eState != Skill4)
		{
			m_eState = Skill4;
			m_iCurrentSkill_Index = Skill4;
			m_bAttacked = false;
		}
		//Change_Weapon(WP_BOW, BOW0);
	}

	if (m_pGameInstance->Key_Down(DIK_5))
	{
		if(m_Current_Weapon == WP_SWORD)
		Change_Weapon(WP_BOW, BOW0);
		else
		Change_Weapon(WP_SWORD, SWORD0);
	}

	if(m_pGameInstance->Mouse_Down(DIM_RBUTTON))
	{
		if(m_Current_Weapon == WP_BOW)
		{
			if (m_eState != Aim_Idle)
			{
				m_eState = SkillR;
				m_iCurrentSkill_Index = SkillR;
				m_pGameInstance->Set_AimMode(true);
			}
			else
			{
				m_eState = Attack_Idle;
				m_pGameInstance->Set_AimMode(false);
				_vec4 vLook = m_pTransformCom->Get_State(State::Look);
				vLook.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt_Dir(vLook);
			}
		}


	}

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
	{
		if (m_eState == Aim_Idle)
		{
			m_eState = SkillR;
			m_iCurrentSkill_Index = SkillR;

		}
	}

	if(m_eState!=Attack&&m_eState< Skill1)
	{
		//퀘스트 개수에 따라 bool로 통과하도록 한번 거쳐야할듯 아니면 계속 맵에서 찾아야되니까 
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			vDirection += vForwardDir;
			hasMoved = true;
			CEvent_Manager::Get_Instance()->Update_Quest(TEXT("이동하기"));
		}
		else if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			vDirection -= vForwardDir;
			hasMoved = true;
		}

		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
				vDirection += vRightDir;
				hasMoved = true;
		}
		else if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			vDirection -= vRightDir;
			hasMoved = true;
		}
	}

	if (hasMoved)
	{

		if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if (/*m_pTransformCom->Is_OnGround() and*/
				m_eState == Walk or 
				m_eState == Idle
				)
			{
				m_eState = Run_Start;
				m_pTransformCom->Set_Speed(m_fRunSpeed);
			 }
			else if (/*m_pTransformCom->Is_OnGround() and*/
				m_eState == Run or 
				m_eState == Run_End or
				m_pModelCom->IsAnimationFinished(Anim_Normal_run_start))
			{
				m_eState = Run;
				m_pTransformCom->Set_Speed(m_fRunSpeed);
			}
			else if (m_eState == Attack_Idle)
			{
				m_eState = Attack_Run;
				m_pTransformCom->Set_Speed(m_fRunSpeed);
			}
		}
		else
		{
			if (m_eState == Run or
				m_eState == Run_End or
				m_eState == Run_Start)
			{
				m_eState = Run;
				m_pTransformCom->Set_Speed(m_fRunSpeed);
			}
			else if (/*m_pTransformCom->Is_OnGround() and*/
				m_eState == Idle or
				m_eState == Walk or
				m_eState == Attack_Idle)
			{
				m_eState = Walk;
				m_pTransformCom->Set_Speed(m_fWalkSpeed);
			}
			if (m_eState == Run)
			{
				m_eState = Run;
				m_pTransformCom->Set_Speed(m_fRunSpeed);
			}
		}

		m_pTransformCom->Go_To_Dir(vDirection, fTimeDelta);

		_vec4 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();

		_float fInterpolTime = 0.4f;
		m_vOriginalLook = vLook;
		if (m_fInterpolationRatio < fInterpolTime)
		{
			if (not m_isInterpolating)
			{
				m_vOriginalLook = vLook;
				m_isInterpolating = true;
			}

			m_fInterpolationRatio += fTimeDelta;


			_float fRatio = m_fInterpolationRatio / fInterpolTime;

			vDirection = XMVectorLerp(m_vOriginalLook, vDirection, fRatio);
		}
		else
		{
			m_isInterpolating = false;
			m_fInterpolationRatio = 0.f;
		}
			m_pTransformCom->LookAt_Dir(vDirection);
		
	}
	else if (m_eState == Walk or
		m_pModelCom->IsAnimationFinished(Anim_B_idle_end)
		)
	{
		m_eState = Idle;
	}

	if (m_pGameInstance->Key_Down(DIK_F))
	{
		vDirection += vForwardDir;
	
		m_pTransformCom->Set_Speed(0.5f);
		m_pTransformCom->Go_To_Dir(vDirection, fTimeDelta);

		Common_Attack();
	}

	m_ReturnZoomTime += (fTimeDelta * 0.5f);
	
	if (m_iCurrentSkill_Index==0&&m_fAttTimer > 1.0f)
	{
		_float fZoom = Lerp(m_fAttackZoom, 0.f, m_ReturnZoomTime);
		m_pGameInstance->Set_CameraAttackZoom(fZoom);

	}
	else if (m_iCurrentSkill_Index != 0 && m_fSkiilTimer> 1.0f)
	{
		_float fZoom = Lerp(m_fAttackZoom, 0.f, m_ReturnZoomTime);
		m_pGameInstance->Set_CameraAttackZoom(fZoom);

	}
}
void CPlayer::Common_Attack()
{
	if (m_fAttTimer < 0.55f)
		return;

	if (m_fAttTimer > 1.1f || m_iAttackCombo > 3 || (m_eState != Attack_Idle && m_eState != Attack))
		m_iAttackCombo = 0;

	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;
	m_eState = Attack;
	m_iCurrentSkill_Index = 0;
	if(m_Current_Weapon == WP_SWORD)
	{
		switch (m_iAttackCombo)
		{
		case 0:
			m_Animation.iAnimIndex = Anim_Assassin_Attack01_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			break;
		case 1:
			m_Animation.iAnimIndex = Anim_Assassin_Attack02_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			m_fAttackZoom = 1.f;
			break;
		case 2:
			m_Animation.iAnimIndex = Anim_Assassin_Attack03_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			m_fAttackZoom = 1.5f;
			break;
		case 3:
			m_Animation.fAnimSpeedRatio = 3.5f;
			m_Animation.iAnimIndex = Anim_Assassin_Attack04_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			
			break;

		default:
			break;
		}
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		switch (m_iAttackCombo)
		{
		case 0:
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack01_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			break;
		case 1:
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack02_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			break;
		case 2:
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack03_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			break;
		case 3:
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack04_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			break;

		default:
			break;
		}
	}
	
}
void CPlayer::Skill1_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		m_Animation.iAnimIndex = m_SwordSkill[0];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		m_Animation.iAnimIndex = m_BowSkill[0];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
}
void CPlayer::Skill2_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		m_Animation.iAnimIndex = m_SwordSkill[1];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		m_Animation.iAnimIndex = m_BowSkill[1];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
}
void CPlayer::Skill3_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		m_Animation.fAnimSpeedRatio = 3.f;
		m_Animation.iAnimIndex = m_SwordSkill[2];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		m_Animation.iAnimIndex = m_BowSkill[2];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
	
}
void CPlayer::Skill4_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		m_Animation.iAnimIndex = m_SwordSkill[3];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		m_Animation.iAnimIndex = m_BowSkill[3];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
}
void CPlayer::SkillR_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		m_Animation.iAnimIndex = m_SwordSkill[4];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		m_Animation.iAnimIndex = 0;

		m_Animation.iAnimIndex = m_BowSkill[4];
		
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
}
void CPlayer::Cam_AttackZoom(_float fZoom)
{
	
		m_fAttackZoom = fZoom;
		


		m_pGameInstance->Set_CameraAttackZoom(m_fAttackZoom);

		m_ReturnZoomTime = 0.f;

}
void CPlayer::Return_Attack_IdleForm()
{
	if(m_Current_Weapon==WP_SWORD)
	{
		if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack01_A))
			m_Animation.iAnimIndex = Anim_Assassin_Attack01_B;
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack02_A))
			m_Animation.iAnimIndex = Anim_Assassin_Attack02_B;
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack03_A))
			m_Animation.iAnimIndex = Anim_Assassin_Attack03_B;
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack04_A))
			m_Animation.iAnimIndex = Anim_Assassin_Attack04_B;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack01_A))
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_01_B;
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack02_A))
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_02_B;
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack03_A))
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_03_B;
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack04_A))
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_04_B;
	}

}
void CPlayer::After_CommonAtt(_float fTimeDelta)
{
	if(m_Current_Weapon == WP_SWORD)
	{
		if (m_fAttTimer > 0.2f && m_fAttTimer < 0.35f)
		{
			if (m_iAttackCombo == 1)
			{
				m_pTransformCom->Set_Speed(5.f);
				m_pTransformCom->Go_Straight(fTimeDelta);
				Cam_AttackZoom(0.7f);

			}
			else if (m_iAttackCombo == 2)
			{
				m_pTransformCom->Set_Speed(3.f);
				m_pTransformCom->Go_Straight(fTimeDelta);
				Cam_AttackZoom(1.2f);
			}
			else if (m_iAttackCombo == 3)
			{
				m_pTransformCom->Set_Speed(3.f);
				m_pTransformCom->Go_Straight(fTimeDelta);
				Cam_AttackZoom(1.7f);
			}
			else if (m_iAttackCombo == 4)
			{
				m_pTransformCom->Set_Speed(25.f);
				m_pTransformCom->Go_Straight(fTimeDelta);
				Cam_AttackZoom(2.2f);
			}
		}
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_fAttTimer > 0.25f && m_fAttTimer < 0.34f)
		{
			if (m_iAttackCombo == 4)
			{
				m_pTransformCom->Set_Speed(7.f);
				m_pTransformCom->Go_Backward(fTimeDelta);
			}
		}
	}
}
void CPlayer::After_SkillAtt(_float fTimeDelta)
{
	if (m_iCurrentSkill_Index == 0)
		return;

	if (m_Current_Weapon == WP_SWORD)
	{
		if (m_iCurrentSkill_Index == Skill1)
		{
			if (m_fSkiilTimer > 0.1f && m_fSkiilTimer < 0.2f)
			{
				Cam_AttackZoom(1.3f);

				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill1);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.32f && m_fSkiilTimer < 0.5f)
			{
				Cam_AttackZoom(2.6f);
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill1);
					m_bAttacked = true;
				}
			}
			if (m_fSkiilTimer > 0.21f && m_fSkiilTimer < 0.32f && m_bAttacked)
				m_bAttacked = false;
		}
		else if (m_iCurrentSkill_Index == Skill2)
		{
			if (m_fSkiilTimer > 0.1f && m_fSkiilTimer < 0.5f)
			{
				m_pTransformCom->Set_Speed(15.f);
				m_pTransformCom->Go_Straight(fTimeDelta);
			}
			else if (m_fSkiilTimer > 0.65f && m_fSkiilTimer < 1.0f)
			{
				m_pTransformCom->Set_Speed(5.f);
				m_pTransformCom->Go_Straight(fTimeDelta);

			}

			if (m_fSkiilTimer > 0.1f && m_fSkiilTimer < 0.2f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill2);
					m_bAttacked = true;
				}
			}
			if (m_fSkiilTimer > 0.2f && m_fSkiilTimer < 0.27f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.27f && m_fSkiilTimer < 0.35f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill2);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.35f && m_fSkiilTimer < 0.4f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.42f && m_fSkiilTimer < 0.5f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill2);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.5f && m_fSkiilTimer < 0.54f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.7f && m_fSkiilTimer < 0.8f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill2);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.81f && m_fSkiilTimer < 0.87f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.88f && m_fSkiilTimer < 0.98f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill2);
					m_bAttacked = true;
				}
			}
		}
		else if (m_iCurrentSkill_Index == Skill3)
		{
			if (m_fSkiilTimer > 0.1f && m_fSkiilTimer < 0.5f)
				Cam_AttackZoom(0.5f);
			if (m_fSkiilTimer > 0.15f && m_fSkiilTimer < 0.35f)
			{
				m_bHide = true;
			}

			if (m_fSkiilTimer > 0.35f && m_fSkiilTimer < 0.6f)
			{
				m_bHide = false;
				m_pTransformCom->Set_Speed(15.f);
				m_pTransformCom->Go_Straight(fTimeDelta);
			}

			if (m_fSkiilTimer > 0.35f && m_fSkiilTimer < 0.38f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill3);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.38f && m_fSkiilTimer < 0.43f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.5f && m_fSkiilTimer < 0.6f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill3);
					m_bAttacked = true;
				}
			}

		}
		else if (m_iCurrentSkill_Index == Skill4)
		{
			if (m_fSkiilTimer > 0.5f && m_fSkiilTimer < 1.1f)
			{
				//m_pGameInstance->Set_ShakeCam(true,0.15f);
			}
			if (m_fSkiilTimer > 0.1f && m_fSkiilTimer < 0.8f)
			{
				Cam_AttackZoom(3.3 * m_fSkiilTimer);
			}

			if (m_fSkiilTimer > 0.1f && m_fSkiilTimer < 0.2f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.2f && m_fSkiilTimer < 0.22f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.23f && m_fSkiilTimer < 0.25f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.25f && m_fSkiilTimer < 0.29f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.29f && m_fSkiilTimer < 0.35f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.35f && m_fSkiilTimer < 0.4f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.45f && m_fSkiilTimer < 0.5f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.5f && m_fSkiilTimer < 0.53f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.54f && m_fSkiilTimer < 0.6f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.61f && m_fSkiilTimer < 0.66f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.66f && m_fSkiilTimer < 0.7f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.71f && m_fSkiilTimer < 0.74f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.75f && m_fSkiilTimer < 0.779f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.8f && m_fSkiilTimer < 0.82f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 0.85f && m_fSkiilTimer < 0.89f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.9f && m_fSkiilTimer < 0.92f && m_bAttacked)
			{
				m_bAttacked = false;
			}
			if (m_fSkiilTimer > 0.92f && m_fSkiilTimer < 0.94f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
			else if (m_fSkiilTimer > 0.94f && m_fSkiilTimer < 0.97f && m_bAttacked)
			{
				m_bAttacked = false;
			}

			if (m_fSkiilTimer > 1.0f && m_fSkiilTimer < 1.2f)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill4);
					m_bAttacked = true;
				}
			}
		}
		
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_iCurrentSkill_Index == Skill1)
		{
			if (m_fSkiilTimer > 0.f && m_fSkiilTimer < 0.2f)
			{
				m_pGameInstance->Set_AimMode(true,_vec3(0.5f,1.1f,1.7f));
			}
			else if (m_fSkiilTimer > 1.3 && m_fSkiilTimer < 2.0f)
			{
				m_pGameInstance->Set_AimMode(false);
			}
		

		}
		else if (m_iCurrentSkill_Index == Skill2)
		{
			if (m_fSkiilTimer > 0.1f && m_fSkiilTimer < 0.5f)
			{
				m_pTransformCom->Set_Speed(9.f);
				m_pTransformCom->Go_Backward(fTimeDelta);
			}
			else if (m_fSkiilTimer > 0.8f && m_fSkiilTimer < 0.9f)
			{
				m_pGameInstance->Set_ShakeCam(true, 0.1f);
			}
		}
		else if (m_iCurrentSkill_Index == Skill3)
		{
			if (m_fSkiilTimer > 0.1f && m_fSkiilTimer < 0.92f)
				Cam_AttackZoom(-3.f);

		}
		else if (m_iCurrentSkill_Index == Skill4)
		{
			if (m_fSkiilTimer > 0.2f && m_fSkiilTimer < 0.6f)
			{
				m_pGameInstance->Set_ShakeCam(true);
				Cam_AttackZoom(-2.f);
			}
			
		}
		else if (m_iCurrentSkill_Index == SkillR)
		{
			if (m_fSkiilTimer > 0.f && m_fSkiilTimer < 0.06f)
			{
				Cam_AttackZoom(2.f);

				m_pGameInstance->Set_ShakeCam(true,0.01f);
			}
		}
	}

}
void CPlayer::Check_Att_Collider(ATTACK_TYPE Att_Type)
{


	switch (Att_Type)
	{
	case Client::AT_Sword_Common:
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type],10,0); // 맞았을때 데미지를 줘라!
		if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[Att_Type]))
		{
			m_pGameInstance->Set_TimeRatio(0.01f);
			m_pGameInstance->Set_ShakeCam(true, 0.01f);
		}
		break;
	case Client::AT_Sword_Skill1:
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], 20, 0);
		break;
	case Client::AT_Sword_Skill2:
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], 30, 0);
		break;
	case Client::AT_Sword_Skill3:
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], 40, 0);
		break;
	case Client::AT_Sword_Skill4:
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], 50, 0);


		break;
	default:
		break;
	}

	
}
void CPlayer::Attack_Camera_Effect()
{
	for(int i = 0; i < AT_Bow_Common;i++)
	{
		if (m_eState == Skill4 && !m_bAttacked)
		{
			
		}
		else
			m_pGameInstance->Set_TimeRatio(1.f);
	}

}
void CPlayer::Summon_Riding(Riding_Type Type)
{
	if (m_pRiding != nullptr)
		Safe_Release(m_pRiding);

	Riding_Desc Desc{};
	Desc.Type = Type;
	wstring strName{};

	strName = TEXT("Prototype_GameObject_Riding");

	m_pRiding = dynamic_cast<CRiding*>(m_pGameInstance->Clone_Object(strName, &Desc));

	if (m_pRiding == nullptr)
		return;
}
void CPlayer::UnMount_Riding()
{
	m_pRiding->Delete_Riding();
}

void CPlayer::Init_State()
{
	if (m_eState != m_ePrevState)
	{

		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 2.f;
		m_Animation.bSkipInterpolation = false;
		switch (m_eState)
		{
		case Client::CPlayer::Idle:
			m_Animation.iAnimIndex = Anim_idle_00;
			m_Animation.isLoop = true;
			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Walk:
			m_Animation.iAnimIndex = Anim_Normal_Walk;
			m_Animation.isLoop = true;
			m_iSuperArmor = {};
			m_hasJumped = false;
			break;
		case Client::CPlayer::Run_Start:
			m_Animation.iAnimIndex = Anim_Normal_run_start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Run:
			m_Animation.iAnimIndex = Anim_Normal_run;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.isLoop = true;
			m_iSuperArmor = {};
			m_hasJumped = false;
			break;
		case Client::CPlayer::Run_End:
			m_Animation.iAnimIndex = Anim_Normal_run_stop;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Jump_Start:
			m_Animation.iAnimIndex = Anim_jump_start;
			m_Animation.isLoop = false;
			m_hasJumped = true;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Jump:
			m_Animation.iAnimIndex = Anim_jump_loop;
			m_Animation.isLoop = true;
			m_hasJumped = true;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Jump_End:
			m_Animation.iAnimIndex = Anim_jump_end;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Attack:
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		break;
		case Client::CPlayer::Attack_Idle:
			m_Animation.isLoop = true;
			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Attack_Run:
			if(m_Current_Weapon == WP_SWORD)
			m_Animation.iAnimIndex = Anim_Assassin_Battle_Run_end;
			else if (m_Current_Weapon == WP_BOW)
				m_Animation.iAnimIndex = Anim_Sniper_Battle_Run_end;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Skill1:
			Skill1_Attack();
			break;
		case Client::CPlayer::Skill1_End:
			m_Animation.iAnimIndex = Anim_RA_7062_KnockBack_B;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Skill2:
			Skill2_Attack();
			break;
		case Client::CPlayer::Skill3:
			Skill3_Attack();
			break;
		case Client::CPlayer::Skill4:
			Skill4_Attack();
			break;
		case Client::CPlayer::SkillR:
			SkillR_Attack();
			break;
		case Client::CPlayer::Aim_Idle:
			break;
		default:
			break;
		}

		m_ePrevState = m_eState;
	}
}

	void CPlayer::Tick_State(_float fTimeDelta)
	{
		switch (m_eState)
		{
		case Client::CPlayer::Idle:
			m_Animation.iAnimIndex = Anim_idle_00;
			m_Animation.isLoop = true;
			break;
		case Client::CPlayer::Walk:
				m_hasJumped = false;
			break;
		case Client::CPlayer::Run:
			m_eState = Run_End;
			break;
		case Client::CPlayer::Run_End:
			if (m_pModelCom->IsAnimationFinished(Anim_Normal_run_stop))
			{
				m_eState = Idle;
				m_hasJumped = false;
			}
			break;
		case Client::CPlayer::Attack:
		{
			//if(!m_bAttacked)
			{
				if (m_Current_Weapon == WP_SWORD)
					Check_Att_Collider(AT_Sword_Common);
				m_bAttacked = true;
			}

			Return_Attack_IdleForm();

			if(m_Current_Weapon == WP_SWORD)
			{
				if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack01_B) or
					m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack02_B) or
					m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack03_B) or
					m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack04_B)
					)
				{
					m_eState = Attack_Idle;
				}
			}
			if (m_Current_Weapon == WP_BOW)
			{
				if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_Attack_01_B) or
					m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_Attack_02_B) or
					m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_Attack_03_B) or
					m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_Attack_04_B))
				{
					m_eState = Attack_Idle;
				}
			}
		}
			break;
		case Client::CPlayer::Attack_Idle:
		{
			if (m_Current_Weapon == WP_SWORD)
				m_Animation.iAnimIndex = Anim_Assassin_Battle_Idle;
			else if (m_Current_Weapon == WP_BOW)
			{
				m_Animation.iAnimIndex = Anim_B_idle_end;
				m_Animation.isLoop = false;
			}}
			break;
		case Client::CPlayer::Attack_Run:
			if (m_Current_Weapon == WP_SWORD)
			{
				if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Battle_Run_end))
				{
					m_eState = Run;
					m_hasJumped = false;
				}
			}

			if (m_Current_Weapon == WP_BOW)
			{
				if (m_pModelCom->IsAnimationFinished(Anim_Sniper_Battle_Run_end))
				{
					m_eState = Run;
					m_hasJumped = false;
				}
			}
			break;
		case Client::CPlayer::Skill1:
		{		
			if(m_Current_Weapon == WP_SWORD)
			{
				
				if (m_pModelCom->IsAnimationFinished(m_SwordSkill[0]))
				{
					m_eState = Skill1_End;

				}
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[0]))
				{
					m_eState = Attack_Idle;
				
				}
			}
		}
			break;
		case Client::CPlayer::Skill1_End:
			if (m_pModelCom->IsAnimationFinished(Anim_RA_7062_KnockBack_B))
			{
				m_eState = Attack_Idle;
		
			}
			break;
		case Client::CPlayer::Skill2:
		{
			if (m_Current_Weapon == WP_SWORD)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill2);
					m_bAttacked = true;
				}
				if (m_pModelCom->IsAnimationFinished(m_SwordSkill[1]))
				{
					m_eState = Attack_Idle;
				}
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[1]))
				{
					m_eState = Attack_Idle;

				}
			}
		}
			break;
		case Client::CPlayer::Skill3:
		{
			if (m_Current_Weapon == WP_SWORD)
			{
				if (!m_bAttacked)
				{
					Check_Att_Collider(AT_Sword_Skill3);
					m_bAttacked = true;
				}
				if (m_pModelCom->IsAnimationFinished(m_SwordSkill[2]))
				{
					m_eState = Attack_Idle;
				}
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[2]))
				{
					m_eState = Attack_Idle;

				}
			}
		}
			break;
		case Client::CPlayer::Skill4:
		{
			if (m_Current_Weapon == WP_SWORD)
			{
				
				if (m_pModelCom->IsAnimationFinished(m_SwordSkill[3]))
				{
					m_eState = Attack_Idle;
				}
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[3]))
				{
					m_eState = Attack_Idle;

				}
			}
		}
			break;
		case Client::CPlayer::SkillR:
			if (m_Current_Weapon == WP_SWORD)
			{
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				if (m_pModelCom->IsAnimationFinished(m_BowSkill[4]))
				{
					m_Animation.isLoop = false;
					m_eState = Aim_Idle;
				}
			}

			break;
		case Client::CPlayer::Jump_Start:
			break;
		case Client::CPlayer::Jump:
			break;
		case Client::CPlayer::Jump_End:
			break;
		case Client::CPlayer::State_End:
			break;
		default:
			break;
		}
	
	}

HRESULT CPlayer::Add_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_RTVTF"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::OBB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.4f, 0.9f, 0.4f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y *0.9f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Player_Hit_OBB"), (CComponent**)&m_pHitCollider, &CollDesc)))
		return E_FAIL;

	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(1.2f, 0.8f, 1.0f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.4f, 0.4f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), 
		TEXT("Com_Collider_Common_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Common]), &CollDesc)))
		return E_FAIL;




	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill2_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill2]), &CollDesc)))
	{
		return E_FAIL;
	}

	CollDesc.vExtents = _vec3(2.3f, 1.2f, 2.3f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.5f, 0.2f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack2621"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill4]), &CollDesc)))
	{
		return E_FAIL;
	}


	CollDesc.eType = ColliderType::Frustum;
	_mat matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_mat matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(30.f), 0.5f, 0.01f, 1.7f);

	

	CollDesc.matFrustum = matView * matProj;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill3_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill1]), &CollDesc)))
	{
		return E_FAIL;
	}

	matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(30.f), 0.5f, 0.01f,2.5f);

	CollDesc.matFrustum = matView * matProj;



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill1_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill3]), &CollDesc)))
	{
		return E_FAIL;
	}

	


	

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	// WorldMatrix 바인드
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	// ViewMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
		return E_FAIL;

	// ProjMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
		return E_FAIL;

	// 카메라 Far 바인드
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
		return E_FAIL;

	// 모션블러용 이전프레임 WorldMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldWorldMatrix", m_OldWorldMatrix)))
		return E_FAIL;

	// 모션블러용 이전프레임 ViewMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix_vec4x4())))
		return E_FAIL;

	// 뼈 바인드
	if (FAILED(m_pModelCom->Bind_Bone(m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

CPlayer* CPlayer::Create(_dev pDevice, _context pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	CEvent_Manager::Destroy_Instance();


	for (int i = 0; i < AT_End; i++)
	{
		Safe_Release(m_pAttCollider[i]);
	}

	if (m_pRiding != nullptr)
		Safe_Release(m_pRiding);

	Safe_Release(m_pNameTag);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCameraTransform);
	Safe_Release(m_pHitCollider);

}
