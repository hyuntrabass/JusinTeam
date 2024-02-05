#include "Player.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Arrow.h"
#include "FadeBox.h"
#include "HitEffect.h"

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
	m_SwordSkill[0] = Anim_RA_9100_Ambush; // x자로 공격하기
	m_SwordSkill[1] = Anim_RA_9060_SealChain; // 앞으로 점프하면서 때리기
	m_SwordSkill[2] = Anim_RA_9040_RapidAttack; // 사라졌다가 찌르기
	m_SwordSkill[3] = Anim_RA_9050_SealStack; // 난타(쿨김)
	m_SwordSkill[4] = Anim_RA_9080_Hiding; // 은신(우클릭)

	m_BowSkill[0] = Anim_ID_8070_TripleStrike; // 트리플 샷 (주력기)
	m_BowSkill[1] = Anim_ID_8080_BackTumbling; // 백덤블링
	m_BowSkill[2] = Anim_ID_8120_RainArrow; // 화살비
	m_BowSkill[3] = Anim_ID_8130_IllusionArrow; // 분신 나와서 화살(쿨김)
	m_BowSkill[4] = Anim_RS_8110_DodgeAttack; // 에임모드 변경(우클릭)

	Change_Parts(PT_BODY, 1);
	Change_Parts(PT_HAIR, 0);
	Change_Parts(PT_FACE, 0);

	m_pGameInstance->Register_CollisionObject(this, m_pHitCollider, true, m_pAttCollider[AT_Bow_Common], m_pParryingCollider);

	SURFACETRAIL_DESC Desc{};
	Desc.vColor = Colors::WhiteSmoke;
	Desc.iNumVertices = 8;
	//Desc.strMaskTextureTag = L"FX_I_WaveGraMasksRGB_Clamp001_Tex";
	m_pLeft_Trail[0] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	m_pRight_Trail[0] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	Desc.vColor = Colors::LightGreen;
	m_pLeft_Trail[1] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	m_pRight_Trail[1] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	Desc.vColor = Colors::Aquamarine;
	m_pLeft_Trail[2] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	m_pRight_Trail[2] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	Desc.vColor = Colors::MediumPurple;
	m_pLeft_Trail[3] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	m_pRight_Trail[3] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	Desc.vColor = Colors::MediumVioletRed;
	m_pLeft_Trail[4] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	m_pRight_Trail[4] = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	//trail_desc.vColor = _vec4(0.929f, 0.929f, 0.886f, 1.f);
	//trail_desc.vColor = _vec4(0.98f, 0.965f, 0.11f, 1.f);
	//trail_desc.vColor = _vec4(0.467f, 0.949f, 0.416f, 1.f);
	//trail_desc.vColor = _vec4(0.569f, 0.176f, 0.769f, 1.f);
	//trail_desc.vColor = _vec4(0.631f, 0.043f, 0.125f, 1.f);

	m_Left_Mat = m_pModelCom->Get_BoneMatrix("B_Weapon_L");
	m_Right_Mat = m_pModelCom->Get_BoneMatrix("B_Weapon_R");


	SURFACETRAIL_DESC SurfaceDesc{};
	SurfaceDesc.iNumVertices = 20;
	SurfaceDesc.vColor = _vec4(0.f, 0.6f, 1.f, 1.f);
	SurfaceDesc.strMaskTextureTag = L"FX_G_Note_MusicSheet001_Tex";
	m_pTest_Trail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &SurfaceDesc);

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_PLAYER);
	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{

	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return;
	}

	if (m_pGameInstance->Get_CameraState() == CS_WORLDMAP)
	{
		return;
	}

	if (m_bHide && m_fDissolveRatio < 1.f)
	{
		m_fDissolveRatio += fTimeDelta * 5.f;
	}
	else if (!m_bHide && m_fDissolveRatio > 0.f)
	{
		m_fDissolveRatio -= fTimeDelta * 4.f;
	}

	if (m_fDissolveRatio < 0.f)
	{
		m_fDissolveRatio = 0.f;
	}

	if (m_fBoostSpeed > 0.f && m_fBoostSpeedTimmer > 0.f)
	{
		m_fBoostSpeedTimmer -= fTimeDelta;
	}
	else
	{
		m_fBoostSpeed = 0.f;
	}
	Update_Trail(fTimeDelta);


	m_StartRegen += fTimeDelta;
	m_fAttTimer += fTimeDelta;
	m_fSkiilTimer += fTimeDelta;


	m_OldWorldMatrix = m_pTransformCom->Get_World_Matrix();

	if (m_pTest_Trail)
	{
		_vec3 vCenterforTrail = _vec3(m_pTransformCom->Get_CenterPos());
		m_pTest_Trail->Tick(vCenterforTrail + _vec3(0.f, 1.f, 0.f), vCenterforTrail);
	}


	if (m_bStartGame)
	{
		CEvent_Manager::Get_Instance()->Tick(fTimeDelta);

	}

	if (m_pGameInstance->Get_CameraModeIndex() == CM_DEBUG)
	{
		return;
	}

	PART_TYPE eType = CUI_Manager::Get_Instance()->Is_CustomPartChanged();

	if (PART_TYPE::PT_END != eType)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		if (eType == PT_WEAPON)
		{
			WEAPON_TYPE eWpType{};
			_uint iWpIdx = CUI_Manager::Get_Instance()->Get_WeaponType(eType, &eWpType);
			Change_Weapon(eWpType, (WEAPON_INDEX)iWpIdx);

		}
		else
		{
			Change_Parts(eType, CUI_Manager::Get_Instance()->Get_CustomPart(eType));
		}

	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
	{
		if (!m_bReadySwim)
		{
			m_bReadySwim = true;
		}
		else
		{
			m_bReadySwim = false;
		}
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD6))
	{
		m_eState = Mining;
		m_iMiningCount = 0;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
	{
		m_eState = Collect_Start;
	}
	if (m_pGameInstance->Key_Down(DIK_8))
	{
		if (!m_bIsMount)
		{
			m_bIsMount = true;
			m_eState = Mount;
			m_Animation.iAnimIndex = Anim_Mount_Idle;
			Summon_Riding(Horse);
		}
		else
		{
			m_pRiding->Delete_Riding();
		}
	}
	Front_Ray_Check();

	if (m_bIsMount)
	{
		m_pRiding->Tick(fTimeDelta);
		Tick_Riding();
	}

	if (m_bIsClimb)
	{
		Is_Climb(fTimeDelta);
	}

	if (m_pGameInstance->Get_CameraState() == CS_ZOOM && m_Animation.iAnimIndex != Anim_Talk_reaction)
	{
		m_Animation.iAnimIndex = Anim_Talk_reaction;
		m_Animation.isLoop = true;
	}
	else if (m_Animation.iAnimIndex == Anim_Talk_reaction && m_pGameInstance->Get_CameraState() != CS_ZOOM)
	{
		m_eState = Idle;
	}

	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		if (CUI_Manager::Get_Instance()->Is_InvenActive())
		{
			m_isInvenActive = true;
			if (CUI_Manager::Get_Instance()->Set_CurrentPlayerPos(m_pTransformCom->Get_State(State::Pos)))
			{
				m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
				m_Animation.iAnimIndex = Anim_idle_00;
				m_Animation.isLoop = true;
				m_hasJumped = false;
				m_iSuperArmor = {};
			}

			m_pTransformCom->Set_State(State::Pos, CUI_Manager::Get_Instance()->Get_InvenPos());

			if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
			{
				_long dwMouseMove;

				if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
				{
					m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -1.f * 0.1f);
				}
			}
		}
		return;
	}
	if (m_isInvenActive && !CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		m_pTransformCom->Set_State(State::Pos, CUI_Manager::Get_Instance()->Get_LastPlayerPos());
		m_isInvenActive = false;
		return;
	}

	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_CUSTOM && !m_bIsMount)
	{
		Move(fTimeDelta);
		Init_State();

		Tick_State(fTimeDelta);
		if (m_Current_Weapon == WP_SWORD)
		{
			After_SwordAtt(fTimeDelta);
		}
		else if (m_Current_Weapon == WP_BOW)
		{
			After_BowAtt(fTimeDelta);
		}

	}
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
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -1.f * fMouseSensor);
			}
		}
	}

	After_CommonAtt(fTimeDelta);

	m_pHitCollider->Update(m_pTransformCom->Get_World_Matrix());

	if (m_bArrowRain_Start)
	{
		Arrow_Rain();
	}

	for (int i = 0; i < AT_Bow_Skill1; i++)
	{
		_mat offset = /*_mat::CreateScale(_vec3(1.f,6.5f,1.f))* */_mat::CreateTranslation(_vec3(0.f, 1.f, 0.f));
		m_pAttCollider[i]->Update(offset * m_pTransformCom->Get_World_Matrix());
	}

	if (m_pNameTag != nullptr)
	{
		m_pNameTag->Tick(fTimeDelta);
	}

	if (m_eState == Skill4 or m_eState == Skill3 or m_eState == Skill2)
	{
		m_UsingMotionBlur = true;
	}
	else
	{
		m_UsingMotionBlur = false;
	}
	m_pParryingCollider->Update(m_pTransformCom->Get_World_Matrix());
}

void CPlayer::Late_Tick(_float fTimeDelta)
{


	if (m_pGameInstance->Get_CameraState() == CS_WORLDMAP)
	{
		return;
	}

	if (m_bIsMount)
	{
		m_pRiding->Late_Tick(fTimeDelta);
	}

	if (m_bStartGame)
	{
		CEvent_Manager::Get_Instance()->Late_Tick(fTimeDelta);

	}

	m_pModelCom->Set_Animation(m_Animation);
	if (m_UsingMotionBlur)
	{
		m_ShaderIndex = 2;
	}
	else
	{
		m_ShaderIndex = 1;
	}

	if (!m_bStartGame && m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
	{

		m_pTransformCom->Set_Scale(_vec3(1.0f));
		Add_Info();

		m_strPlayerName = m_pGameInstance->Get_InputString();
		dynamic_cast<CNameTag*>(m_pNameTag)->Set_Text(m_strPlayerName);

		m_bStartGame = true;
		CEvent_Manager::Get_Instance()->Init();



		Change_Weapon(WP_SWORD, SWORD0);

	}



	if (m_pTest_Trail)
	{
		m_pTest_Trail->Late_Tick(fTimeDelta);
	}

#ifdef _DEBUG
	//m_pRendererCom->Add_DebugComponent(m_pHitCollider);
	m_pRendererCom->Add_DebugComponent(m_pParryingCollider);

	//for (int i = 0; i < AT_End; i++)
	{
		/*m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill1]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill2]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill3]);
		m_pRendererCom->Add_DebugComponent(m_pAttCollider[AT_Sword_Skill4]);*/
	}

#endif // DEBUG

	m_pModelCom->Play_Animation(fTimeDelta, m_bAttacked);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}


	if (m_pNameTag != nullptr)
	{
		m_pNameTag->Late_Tick(fTimeDelta);
	}


}

HRESULT CPlayer::Render()
{


	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (m_Body_CurrentIndex >= 0)
	{
		Render_Parts(PT_BODY, m_Body_CurrentIndex);
	}
	if (m_Hair_CurrentIndex >= 0 and m_bHelmet_Hide)
	{
		Render_Parts(PT_HAIR, m_Hair_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex <= 0)
	{
		Render_Parts(PT_HAIR, m_Hair_CurrentIndex);
	}
	if (m_Face_CurrentIndex >= 0 and m_bHelmet_Hide)
	{
		Render_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex <= 0)
	{
		Render_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex == 1)
	{
		Render_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	else if (m_Hair_CurrentIndex >= 0 and m_Helmet_CurrentIndex == 2)
	{
		Render_Parts(PT_FACE, m_Face_CurrentIndex);
	}
	if (m_Helmet_CurrentIndex >= 0 && !m_bHelmet_Hide)
	{
		Render_Parts(PT_HELMET, m_Helmet_CurrentIndex);
	}
	if (m_Weapon_CurrentIndex < WP_UNEQUIP)
	{
		Render_Parts(PT_WEAPON, (_uint)m_Weapon_CurrentIndex);
	}


	return S_OK;
}

HRESULT CPlayer::Add_Info()
{
	CNameTag::NAMETAG_DESC NameTagDesc = {};
	NameTagDesc.eLevelID = LEVEL_STATIC;
	NameTagDesc.fFontSize = 0.32f;
	NameTagDesc.pParentTransform = m_pTransformCom;
	NameTagDesc.strNameTag = m_strPlayerName;
	NameTagDesc.vColor = _vec4(0.5f, 0.7f, 0.5f, 1.f);
	NameTagDesc.vTextPosition = _vec2(0.f, 1.9f);

	m_pNameTag = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NameTag"), &NameTagDesc);
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
	{
		return E_FAIL;
	}

	Desc.FileName = "body1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "body5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.ePartType = PT_HELMET;
	Desc.FileName = "helmet0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "helmet1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "helmet2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "helmet3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "helmet4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "helmet5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.ePartType = PT_FACE;
	Desc.FileName = "face0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "face5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.ePartType = PT_HAIR;
	Desc.FileName = "hair0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair6";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair7";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "hair8";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.ePartType = PT_WEAPON;

	Desc.FileName = "bow0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "bow1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "bow2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "bow3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "bow4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "sword0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "sword1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "sword2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	Desc.FileName = "sword3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}
	Desc.FileName = "sword4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Render_Parts(PART_TYPE Parts, _uint Index)
{
	if (m_bWeapon_Unequip && Parts == PT_WEAPON)
	{
		return S_OK;
	}

	for (size_t k = 0; k < m_pModelCom->Get_Num_PartMeshes(Parts, Index); k++)
	{
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_DiffuseTexture", TextureType::Diffuse, (_uint)Parts, (_uint)Index, k)))
		{
			continue;
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_NormalTexture", TextureType::Normals, (_uint)Parts, (_uint)Index, k)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		_bool g_HasMaskTex{};
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_MaskTexture", TextureType::Shininess, (_uint)Parts, (_uint)Index, k)))
		{
			g_HasMaskTex = false;
		}
		else
		{
			g_HasMaskTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &g_HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (Parts == PT_HAIR)
		{
			if (FAILED(m_pShaderCom->Begin(m_ShaderIndex + 2)))
			{
				return E_FAIL;
			}
			//if (FAILED(m_pShaderCom->Begin(5)))
			//	return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(m_ShaderIndex)))
			{
				return E_FAIL;
			}
		}
		if (FAILED(m_pModelCom->Render_Part((_uint)Parts, (_uint)Index, k)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CPlayer::Add_Riding()
{
	Riding_Desc riding_desc{};
	riding_desc.Type = Bird;

	m_pRiding = (CRiding*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Riding"), &riding_desc);

	if (m_pRiding == nullptr)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Set_Damage(_int iDamage, _uint MonAttType)
{
	if (m_eState == Revival_Start or m_eState == Revival_End or m_eState == Die)
	{
		return;
	}

	if (m_eState == Aim_Idle or m_eState == SkillR)
	{
		m_pGameInstance->Set_AimMode(false);
	}
	if (m_iShield > 0)
	{
		m_iShield--;
		// 보호막 깨지는 이펙트
		return;

	}


	m_Status.Current_Hp -= (iDamage - iDamage * (_int)(m_Status.Armor / 0.01));



	CHitEffect::HITEFFECT_DESC Desc{};
	_int iRandomX = rand() % 100;
	_int iRandomY = rand() % 50 + 130;
	Desc.iDamage = iDamage;
	Desc.pParentTransform = m_pTransformCom;
	Desc.isPlayer = true;
	Desc.vTextPosition = _vec2((_float)(iRandomX - 50) * 0.01f, (_float)iRandomY * 0.01f);
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_HitEffect"), TEXT("Prototype_GameObject_HitEffect"), &Desc)))
	{
		return;
	}
	CUI_Manager::Get_Instance()->Set_Hp(m_Status.Current_Hp, m_Status.Max_Hp);

	if (m_Status.Current_Hp < 0)
	{
		m_Status.Current_Hp = 0;
		CUI_Manager::Get_Instance()->Set_Hp(m_Status.Current_Hp, m_Status.Max_Hp);
		m_eState = Die;
	}
	else
	{
		if (m_iSuperArmor > 0 && MonAttType != Parrying_Succescc)
		{
			return;
		}

		switch (MonAttType)
		{
		case MonAtt_Hit:
		{
			m_eState = Hit;
			m_Animation.iAnimIndex = Anim_Stun_start;
			m_Animation.fDurationRatio = 0.4f;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fStartAnimPos = 18.f;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case MonAtt_KnockDown:
		{
			m_eState = KnockDown;
		}
		break;
		case MonAtt_Stun:
		{
			m_eState = Stun_Start;
		}
		break;
		case Parrying_Succescc:
		{
			m_iShield++;
			m_fBoostSpeedTimmer = 5.f;
			m_fBoostSpeed = 3.f;
			//보호막 생성
		}
		default:
			break;
		}
	}

	m_StartRegen = 0.f;
}

void CPlayer::Change_Parts(PART_TYPE PartsType, _int ChangeIndex)
{
	switch (PartsType)
	{
	case Client::PT_HAIR:
	{
		m_Hair_CurrentIndex = ChangeIndex;
	}
	break;
	case Client::PT_FACE:
	{
		m_Face_CurrentIndex = ChangeIndex;
	}
	break;
	case Client::PT_BODY:
	{
		m_Body_CurrentIndex = ChangeIndex;
	}
	break;
	case Client::PT_HELMET:
	{
		m_Helmet_CurrentIndex = ChangeIndex;
	}
	break;
	default:
		break;
	}
}

void CPlayer::Change_Weapon(WEAPON_TYPE PartsType, WEAPON_INDEX ChangeIndex)
{
	if (ChangeIndex == WP_UNEQUIP)
	{
		m_bWeapon_Unequip = true;
	}
	else
	{
		m_Weapon_CurrentIndex = ChangeIndex;
		m_Current_Weapon = PartsType;
		m_bWeapon_Unequip = false;
	}
}


void CPlayer::Front_Ray_Check()
{
	PxRaycastBuffer Buffer{};
	_vec4 vRayDir{};
	_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 PlayerCenter = m_pTransformCom->Get_CenterPos();
	PlayerCenter.y += 0.6f;
	vRayDir = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_float fDist = XMVectorGetX(XMVector3Length(vRayDir)) - 0.4f;
	if (m_pGameInstance->Raycast(PlayerCenter + vRayDir * 0.4f, vRayDir, fDist, Buffer))
	{
		m_bReady_Climb = true;
	}
	else
	{
		m_bReady_Climb = false;
	}
}

_bool CPlayer::Turn_Ray_Check(_bool bRight)
{
	PxRaycastBuffer Buffer{};
	_vec4 vRayDir{};
	_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 vRight = m_pTransformCom->Get_State(State::Right).Get_Normalized();
	_vec4 PlayerCenter = m_pTransformCom->Get_CenterPos();
	PlayerCenter.y += 0.6f;
	if (bRight)
	{
		PlayerCenter += vRight * 0.5f;
	}
	else
	{
		PlayerCenter -= vRight * 0.5f;
	}

	vRayDir = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_float fDist = XMVectorGetX(XMVector3Length(vRayDir)) - 0.4f;

	return (m_pGameInstance->Raycast(PlayerCenter + vRayDir * 0.4f, vRayDir, fDist, Buffer));
}

void CPlayer::Health_Regen(_float fTImeDelta)
{
	if (m_StartRegen > 5.f && m_Status.Max_Hp > m_Status.Current_Hp)
	{
		m_fHpRegenTime += fTImeDelta;
	}

	if (m_Status.Max_Mp > m_Status.Current_Mp)
	{
		m_fMpRegenTime += fTImeDelta;
	}

	if (m_fHpRegenTime >= 1.f)
	{
		m_Status.Current_Hp += 1;
		m_fHpRegenTime = 0;
	}

	if (m_fMpRegenTime >= 1.f)
	{
		m_Status.Current_Mp += 1;
		m_fMpRegenTime = 0;
	}


	if (m_Status.Max_Hp < m_Status.Current_Hp)
	{
		m_Status.Current_Hp = m_Status.Max_Hp;
	}
	if (m_Status.Max_Mp < m_Status.Current_Mp)
	{
		m_Status.Current_Mp = m_Status.Max_Mp;
	}

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

	if (m_eState == Revival_Start or m_eState == Revival_End
		or m_eState == KnockDown or m_eState == Stun_Start
		or m_eState == Stun or m_eState == Die)
	{
		return;
	}

	if (!m_bReadySwim && m_Weapon_CurrentIndex < WP_UNEQUIP)
	{


		if (m_eState == SkillR || m_eState == Aim_Idle)
		{
			m_pTransformCom->LookAt_Dir(m_pCameraTransform->Get_State(State::Look));

		}


		//if (m_pGameInstance->Key_Down(DIK_1))
		//{
		//	if (m_eState != Skill1)
		//	{
		//		Ready_Skill(ST_Skill1); // 1번창에 있던 스킬 넣어주기
		//		return;
		//	}
		//}

		//if (m_pGameInstance->Key_Down(DIK_2))
		//{
		//	if (m_eState != Skill2)
		//	{
		//		Ready_Skill(ST_Skill2);
		//		return;
		//	}
		//}


		//if (m_pGameInstance->Key_Down(DIK_3))
		//{
		//	if (m_eState != Skill3)
		//	{
		//		Ready_Skill(ST_Skill3);
		//		return;
		//	}
		//}

		//if (m_pGameInstance->Key_Down(DIK_4))
		//{
		//	if (m_eState != Skill4)
		//	{
		//		Ready_Skill(ST_Skill4);
		//		return;
		//	}

		//}


		CSkillBlock::SKILLSLOT eSlotIdx{};
		_bool isPress = false;
		if (m_pGameInstance->Key_Down(DIK_1))
		{
			eSlotIdx = CSkillBlock::SKILL1;
			isPress = true;
		}
		if (m_pGameInstance->Key_Down(DIK_2))
		{
			eSlotIdx = CSkillBlock::SKILL2;
			isPress = true;
		}
		if (m_pGameInstance->Key_Down(DIK_3))
		{
			eSlotIdx = CSkillBlock::SKILL3;
			isPress = true;
		}
		if (m_pGameInstance->Key_Down(DIK_4))
		{
			eSlotIdx = CSkillBlock::SKILL4;
			isPress = true;
		}
		if (isPress && m_fSkiilTimer > 1.2f)
		{
			_int iSkillNum = 0;
			if (CUI_Manager::Get_Instance()->Use_Skill(m_Current_Weapon, eSlotIdx, &iSkillNum))
			{
				Ready_Skill((Skill_Type)iSkillNum);
				return;
			}

		}

		if (m_pGameInstance->Key_Down(DIK_5))
		{
			if (m_eState == Idle)
			{
				if (m_Current_Weapon == WP_SWORD)
				{
					Change_Weapon(WP_BOW, BOW0);
				}
				else
				{
					Change_Weapon(WP_SWORD, SWORD0);
				}
			}
		}

		if (m_pGameInstance->Mouse_Down(DIM_RBUTTON))
		{
			if (m_Current_Weapon == WP_BOW)
			{
				if (m_eState == Skill1 or m_eState == Skill2 or m_eState == Skill3 or m_eState == Skill4)
				{
					return;
				}

				m_bLockOn = !m_bLockOn;

				if (m_bLockOn)
				{
					m_pGameInstance->Set_AimMode(true);
				}
				else
				{
					m_pGameInstance->Set_AimMode(false);
				}
				/*if (m_eState == Idle)
				{
					m_eState = Aim_Idle;
					m_Animation.iAnimIndex = Anim_LoadingScene_Pose_Sniper;
					m_iCurrentSkill_Index = Aim_Idle;
					m_pGameInstance->Set_AimMode(true);
				}
				else if (m_eState == Aim_Idle)
				{
					m_eState = Attack_Idle;
					m_pGameInstance->Set_AimMode(false);

					vDirection += vForwardDir;
					m_pTransformCom->LookAt_Dir(vDirection);
				}*/
			}
		}

		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
		{
			if (m_eState == Aim_Idle)
			{
				SkillR_Attack();
				m_eState = SkillR;
				m_iCurrentSkill_Index = SkillR;
				m_fAttTimer = 0.f;
			}
			else if (m_fAttTimer > 0.2f and m_eState == SkillR)
			{
				SkillR_Attack();
				m_eState = SkillR;
				m_iCurrentSkill_Index = SkillR;
				m_fAttTimer = 0.f;
			}
		}


		if (m_eState == Jump)
		{
			if (!m_pTransformCom->Is_Jumping())
			{
				m_eState = Jump_End;
			}
		}
		if (m_fAttTimer > 0.8f && m_eState == Attack)
		{
			m_bReady_Move = true;
		}
		else if (m_eState != Attack)
		{
			m_bReady_Move = false;
		}
	}
	if ((m_fSkiilTimer > 1.2f && m_eState != SkillR && m_eState != Aim_Idle))
	{
		//퀘스트 개수에 따라 bool로 통과하도록 한번 거쳐야할듯 아니면 계속 맵에서 찾아야되니까 
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			CEvent_Manager::Get_Instance()->Set_TutorialSeq(T_OPENINVEN);
			vDirection += vForwardDir;

			hasMoved = true;

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

		if (m_pGameInstance->Key_Down(DIK_F))
		{
			if (m_bIsClimb or m_bReadySwim or m_hasJumped)
			{
				return;
			}



			if (vDirection != _vec4())
			{
				m_pTransformCom->LookAt_Dir(vDirection);
			}

			Common_Attack();
			m_eState = Attack;
			m_ReadyArrow = true;
			hasMoved = false;
		}
		if (m_pGameInstance->Key_Down(DIK_SPACE))
		{
			if (m_bReadySwim)
			{
				return;
			}
			//if (!m_hasJumped)
			{
				m_pTransformCom->Jump(8.f);
				m_eState = Jump_Start;
				CEvent_Manager::Get_Instance()->Update_Quest(TEXT("점프하기"));
			}
			if (m_bReady_Climb)
			{
				m_eState = Climb;
				m_bIsClimb = true;
			}
		}
		if (m_bReadySwim && hasMoved)
		{
			if (m_eState == Swim_Idle or m_eState == Swim)
			{
				m_eState = Swim;
			}
			else
			{
				return;
			}

			m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed);
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
		else if (m_bReadySwim && !hasMoved)
		{
			if (m_eState != Swim_collect)
			{
				m_eState = Swim_Idle;
			}
		}
		else if (hasMoved && !m_bIsClimb && m_eState != Attack && !m_bReadySwim)
		{

			if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
			{
				if (m_eState == Walk or
					m_eState == Idle)
				{
					m_eState = Run_Start;

				}
				else if (m_eState == Attack or
					m_eState == Skill1 or
					m_eState == Skill2 or
					m_eState == Skill3 or
					m_eState == Skill4)
				{
					m_eState = Attack_Run;
				}
				else if (/*m_pTransformCom->Is_OnGround() and*/
					m_eState == Run or
					m_eState == Run_End or
					m_pModelCom->IsAnimationFinished(Anim_Normal_run_start))
				{
					m_eState = Run;

				}
				else if (m_eState == Attack_Idle)
				{
					m_eState = Attack_Run;

				}
				else if (m_eState == Jump_End)
				{
					m_eState = Jump_Run;

				}
				m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed);
			}
			else
			{
				if (m_eState == Run or
					m_eState == Run_End or
					m_eState == Run_Start
					)
				{
					m_eState = Run;
					m_pTransformCom->Set_Speed(m_fRunSpeed + m_Status.Speed + m_fBoostSpeed);
				}
				else
				{
					if (/*m_pTransformCom->Is_OnGround() and*/
						m_eState == Idle or
						m_eState == Walk or
						m_eState == Attack_Idle or
						/*	m_eState == Attack or*/
						m_eState == Skill1 or
						m_eState == Skill2 or
						m_eState == Skill3 or
						m_eState == Skill4)
					{
						m_eState = Walk;

					}
					if (m_eState != Attack)
					{
						m_pTransformCom->Set_Speed(m_fWalkSpeed + m_Status.Speed / 3.f);
					}
				}


			}



			if (m_eState != Attack or (m_eState == Attack && m_fAttTimer > 0.75f))
			{
				m_pTransformCom->Go_To_Dir(vDirection, fTimeDelta);
			}


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
		else if (m_eState == Walk or m_eState == Run_Start or
			m_pModelCom->IsAnimationFinished(Anim_B_idle_end))
		{
			m_eState = Idle;
		}
		if (m_eState == Jump_Start)
		{
			if (!m_pTransformCom->Is_Jumping())
			{
				m_eState = Idle;
			}
		}
	}



	m_ReturnZoomTime += (fTimeDelta * 0.5f);

	if (m_iCurrentSkill_Index == 0 && m_fAttTimer > 1.0f)
	{
		_float fZoom = Lerp(m_fAttackZoom, 0.f, m_ReturnZoomTime);
		m_pGameInstance->Set_CameraAttackZoom(fZoom);

	}
	else if (m_iCurrentSkill_Index != 0 && m_fSkiilTimer > 1.0f)
	{
		_float fZoom = Lerp(m_fAttackZoom, 0.f, m_ReturnZoomTime);
		m_pGameInstance->Set_CameraAttackZoom(fZoom);

	}

	if (!m_bIsClimb or m_eState == Jump_Start)
	{
		m_pTransformCom->Gravity(fTimeDelta);
	}

}

void CPlayer::Is_Climb(_float fTimeDelta)
{
	m_pTransformCom->Set_Speed(0.9f);
	_bool hasMove{};
	if (m_eState == Jump_Start)
	{
		if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_jump_start)
		{
			_float Index = m_pModelCom->Get_CurrentAnimPos();
			if (!m_bReady_Climb && Index > 48)
			{
				m_eState = Idle;
				m_bIsClimb = false;
				return;
			}
		}
	}
	if (m_eState == Jump)
	{
		m_bIsClimb = false;
		return;
	}

	if (m_eState != Jump_Start)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
			m_pTransformCom->Go_Up(fTimeDelta);
			if (!m_bReady_Climb)
			{
				m_pTransformCom->Jump(8.f);
				m_eState = Jump_Start;
				m_bIsClimb = false;
				return;
			}
			hasMove = true;
			m_eState = Climb_U;

		}
		else if (m_pGameInstance->Key_Pressing(DIK_S))
		{
			m_pTransformCom->Go_Down(fTimeDelta);
			if (!m_pTransformCom->Is_Jumping())
			{
				m_eState = Idle;
				m_bIsClimb = false;
				return;
			}
			hasMove = true;
			m_eState = Climb_D;

		}

		if (m_pGameInstance->Key_Pressing(DIK_D))
		{
			m_pTransformCom->Go_Right(fTimeDelta);
			hasMove = true;
			m_eState = Climb_R;
		}
		else if (m_pGameInstance->Key_Pressing(DIK_A))
		{
			m_pTransformCom->Go_Left(fTimeDelta);

			hasMove = true;
			m_eState = Climb_L;
		}
	}
	if (m_eState == Climb_R)
	{
		if (!Turn_Ray_Check(true))
		{
			m_pTransformCom->Set_RotationPerSec(-40.f);
			m_pTransformCom->Turn(_vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
	}
	if (m_eState == Climb_L)
	{
		if (!Turn_Ray_Check(false))
		{
			m_pTransformCom->Set_RotationPerSec(40.f);
			m_pTransformCom->Turn(_vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}
	}
	if (!hasMove && m_eState != Jump_Start)
	{
		m_eState = Climb;
	}


	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (m_eState == Jump_Start)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_jump_start)
			{
				_float Index = m_pModelCom->Get_CurrentAnimPos();
				if (Index > 48)
				{
					if (m_bReady_Climb)
					{
						m_eState = Climb;
					}
				}
			}
		}
		else
		{
			m_pTransformCom->Jump(8.f);
			m_eState = Jump_Start;
			m_bIsClimb = false;
		}

	}


}

void CPlayer::Common_Attack()
{
	if (m_fAttTimer < 0.75f)
	{
		return;
	}

	if (m_fAttTimer > 1.5f || m_iAttackCombo > 3 || (m_eState != Attack_Idle && m_eState != Attack))
	{
		m_iAttackCombo = 0;
	}

	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	m_iCurrentSkill_Index = 0;
	if (m_Current_Weapon == WP_SWORD)
	{
		CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		if (pMonCollider != nullptr)
		{
			_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
			vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
			m_pTransformCom->LookAt(vMonPos);
		}

		switch (m_iAttackCombo)
		{
		case 0:
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack01_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}
		break;
		case 1:
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack02_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			m_fAttackZoom = 1.f;
		}
		break;
		case 2:
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack03_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
			m_fAttackZoom = 1.5f;
		}
		break;
		case 3:
		{
			m_Animation.fAnimSpeedRatio = 3.5f;
			m_Animation.iAnimIndex = Anim_Assassin_Attack04_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}

		break;

		default:
			break;
		}
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_bLockOn)
		{
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
			}
		}
		switch (m_iAttackCombo)
		{
		case 0:
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack01_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}
		break;
		case 1:
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack02_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}
		break;
		case 2:
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack03_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}
		break;
		case 3:
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_attack04_A;
			m_fAttTimer = 0.f;
			m_iAttackCombo++;
		}
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

		CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		if (pMonCollider != nullptr)
		{
			_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
			vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
			m_pTransformCom->LookAt(vMonPos);
		}
		m_pTransformCom->Set_Speed(6.f);
		m_Animation.iAnimIndex = m_SwordSkill[0];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_fSkiilTimer = 0.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_bLockOn)
		{
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
			}
		}

		m_Animation.iAnimIndex = m_BowSkill[0];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_Animation.fAnimSpeedRatio = 2.7f;
		m_Animation.fDurationRatio = 0.8f;
		m_fSkiilTimer = 0.f;
	}

}
void CPlayer::Skill2_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		if (pMonCollider != nullptr)
		{
			_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
			vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
			m_pTransformCom->LookAt(vMonPos);
		}

		m_Animation.iAnimIndex = m_SwordSkill[1];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_fSkiilTimer = 0.f;
		m_fSkillSpeed = 10.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_bLockOn)
		{
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
			}
		}

		m_pTransformCom->Set_Speed(10.f);
		m_Animation.iAnimIndex = m_BowSkill[1];
		m_Animation.isLoop = false;
		m_hasJumped = false;

		m_iSuperArmor = { 1.f };
		m_fSkiilTimer = 0.f;
	}

}
void CPlayer::Skill3_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{

		CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		if (pMonCollider != nullptr)
		{
			_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
			vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
			m_pTransformCom->LookAt(vMonPos);
		}

		m_Animation.iAnimIndex = m_SwordSkill[2];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_fSkiilTimer = 0.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_bLockOn)
		{
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				m_vArrowRainPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
			}
		}

		m_Animation.iAnimIndex = m_BowSkill[2];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_fSkiilTimer = 0.f;
	}

}

void CPlayer::Skill4_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		_vec4 vLook = m_pGameInstance->Get_CameraLook();
		vLook.y = m_pTransformCom->Get_State(State::Look).y;
		m_pTransformCom->LookAt_Dir(vLook);
		m_Animation.iAnimIndex = m_SwordSkill[3];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_fSkiilTimer = 0.f;
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_bLockOn)
		{
			CCollider* pMonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
			if (pMonCollider != nullptr)
			{
				_vec4 vMonPos = _vec4(pMonCollider->Get_ColliderPos(), 1.f);
				vMonPos.y = m_pTransformCom->Get_State(State::Pos).y;
				m_pTransformCom->LookAt(vMonPos);
			}
		}

		m_Animation.iAnimIndex = m_BowSkill[3];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = { 1.f };
		m_fSkiilTimer = 0.f;
	}

}
void CPlayer::SkillR_Attack()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		/*	m_Animation.iAnimIndex = m_SwordSkill[4];
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
			m_fSkiilTimer = 0.f;*/
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		m_Animation.iAnimIndex = m_BowSkill[4];
		m_Animation.isLoop = false;
		m_hasJumped = false;
		m_iSuperArmor = {};
		m_fSkiilTimer = 0.f;
	}
}
void CPlayer::Ready_Skill(Skill_Type Type)
{
	switch (Type)
	{
	case Client::ST_Skill1:
	{
		m_eState = Skill1;
		m_iCurrentSkill_Index = Skill1;
	}
	break;
	case Client::ST_Skill2:
	{
		m_eState = Skill2;
		m_iCurrentSkill_Index = Skill2;
	}
	break;
	case Client::ST_Skill3:
	{
		m_eState = Skill3;
		m_iCurrentSkill_Index = Skill3;
	}
	break;
	case Client::ST_Skill4:
	{
		m_eState = Skill4;
		m_iCurrentSkill_Index = Skill4;
	}
	break;

	default: // 쿨탐이거나 얻지 못한 스킬일때 처리
		break;
	}
	m_ReadyArrow = true;

}
void CPlayer::Cam_AttackZoom(_float fZoom)
{

	m_fAttackZoom = fZoom;



	m_pGameInstance->Set_CameraAttackZoom(m_fAttackZoom);

	m_ReturnZoomTime = 0.f;

}
void CPlayer::Return_Attack_IdleForm()
{
	if (m_Current_Weapon == WP_SWORD)
	{
		if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack01_A))
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack01_B;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack02_A))
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack02_B;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack03_A))
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack03_B;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_Assassin_Attack04_A))
		{
			m_Animation.iAnimIndex = Anim_Assassin_Attack04_B;
		}
	}
	else if (m_Current_Weapon == WP_BOW)
	{
		if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack01_A))
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_01_B;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack02_A))
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_02_B;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack03_A))
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_03_B;
		}
		else if (m_pModelCom->IsAnimationFinished(Anim_ID_Sniper_attack04_A))
		{
			m_Animation.iAnimIndex = Anim_ID_Sniper_Attack_04_B;
		}
	}

}
void CPlayer::After_CommonAtt(_float fTimeDelta)
{
	if (m_Current_Weapon == WP_SWORD)
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

void CPlayer::Check_Att_Collider(ATTACK_TYPE Att_Type)
{


	_int Random = rand() % 100 + 1;
	_int RandomDmg = rand() % 30;
	_uint Critical{};
	if (Random <= m_Status.Critical)
	{
		Critical = m_Status.Attack * (_uint)(m_Status.Critical_Dmg * 0.01) - m_Status.Attack;
	}


	switch (Att_Type)
	{
	case Client::AT_Sword_Common:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], m_Status.Attack + Critical + RandomDmg, Att_Type);
	}
	break;
	case Client::AT_Sword_Skill1:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], (m_Status.Attack * 2) + Critical + RandomDmg, Att_Type);
	}
	break;
	case Client::AT_Sword_Skill2:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], (_uint)(m_Status.Attack * 1.5) + Critical + RandomDmg, Att_Type);
	}
	break;
	case Client::AT_Sword_Skill3:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], (m_Status.Attack * 3) + Critical + RandomDmg, Att_Type);
	}
	break;
	case Client::AT_Sword_Skill4:
	{
		m_pGameInstance->Attack_Monster(m_pAttCollider[Att_Type], (m_Status.Attack * 2) + Critical + RandomDmg, Att_Type);
	}
	break;
	default:
		break;
	}
}
void CPlayer::After_SwordAtt(_float fTimeDelta)
{

	if (m_eState == Attack)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (m_iAttackCombo == 1)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Assassin_Attack01_A)
			{
				if (Index >= 19.f && Index <= 30.f)
				{
					m_pLeft_Trail[m_Weapon_CurrentIndex - 5]->Late_Tick(fTimeDelta);
				}

				if (Index >= 19.f && Index <= 22.f)
				{
					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.7f);
							m_bAttacked = true;
							m_pGameInstance->Set_ShakeCam(true);
						}
					}
				}
				else
				{
					m_pGameInstance->Set_TimeRatio(1.f);
				}
			}
		}
		else if (m_iAttackCombo == 2)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Assassin_Attack02_A)
			{
				if (Index >= 20.5f && Index <= 34.f)
				{
					m_pLeft_Trail[m_Weapon_CurrentIndex - 5]->Late_Tick(fTimeDelta);
				}

				if (Index >= 22.f && Index <= 25.f)
				{
					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.7f);
							m_bAttacked = true;
							m_pGameInstance->Set_ShakeCam(true);
						}
					}
				}
				else
				{
					m_pGameInstance->Set_TimeRatio(1.f);
				}
			}
		}
		else if (m_iAttackCombo == 3)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Assassin_Attack03_A)
			{
				if (Index >= 15.5f && Index <= 27.f)
				{
					m_pLeft_Trail[m_Weapon_CurrentIndex - 5]->Late_Tick(fTimeDelta);
				}
				if (Index >= 19.f && Index <= 22.f)
				{


					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.7f);
							m_bAttacked = true;
							m_pGameInstance->Set_ShakeCam(true);
						}
					}
				}
				else
				{
					m_pGameInstance->Set_TimeRatio(1.f);
				}
			}
		}
		else if (m_iAttackCombo == 4)
		{
			if (m_pModelCom->Get_CurrentAnimationIndex() == Anim_Assassin_Attack04_A)
			{
				if (Index >= 19.f && Index <= 22.f)
				{
					if (!m_bAttacked)
					{
						Check_Att_Collider(AT_Sword_Common);
						if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Common]))
						{
							m_pGameInstance->Set_TimeRatio(0.2f);
							m_bAttacked = true;
							m_pGameInstance->Set_ShakeCam(true);
						}
					}
				}
				else
				{
					m_pGameInstance->Set_TimeRatio(1.f);
				}
			}
		}

	}
	else if (m_eState == Skill1)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 10.f && Index <= 15.f)
		{
			m_iHP = 1;
		}
		else
		{
			m_iHP = 0;
		}

		if (Index >= 5.f && Index < 15.f)
		{

			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (Index >= 5.f && Index < 9.f)
		{
			Cam_AttackZoom(1.3f);
		}
		if (Index >= 11.f && Index <= 13.f)
		{
			Cam_AttackZoom(2.6f);
			m_fSkiilTimer = 0.8f;
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill1);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill1]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_pGameInstance->Set_ShakeCam(true);

				}
			}
		}
		else if (Index > 12.f && Index <= 13.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 14.f && Index <= 16.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill1);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill1]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_pGameInstance->Set_ShakeCam(true);
				}

			}
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}

	}
	if (m_eState == Skill2)
	{

		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 5.f && Index <= 31.f)
		{
			m_pTransformCom->Set_Speed(m_fSkillSpeed);
			m_pTransformCom->Go_Straight(fTimeDelta);
			m_fSkiilTimer = 1.f;
		}

		if (Index >= 55.f && Index <= 67.f)
		{
			m_pTransformCom->Set_Speed(m_fSkillSpeed / 3);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (Index >= 30.f && Index <= 32.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill2);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill2]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_pGameInstance->Set_ShakeCam(true);
					m_fSkillSpeed = 3.f;
				}
			}

		}
		else if (Index > 33.f && Index <= 35.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 36.f && Index <= 40.f)
		{
			m_fSkiilTimer = 0;
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill2);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill2]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_fSkillSpeed = 3.f;
					m_pGameInstance->Set_ShakeCam(true);
				}

			}
		}
		else if (Index > 40.f && Index <= 43.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 52.f && Index <= 54.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill2);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill2]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_fSkillSpeed = 3.f;
					m_pGameInstance->Set_ShakeCam(true);
				}

			}
		}
		else if (Index > 55.f && Index <= 57.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 65.f && Index <= 67.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill2);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill2]))
				{
					m_pGameInstance->Set_TimeRatio(0.4f);
					m_bAttacked = true;
					m_fSkillSpeed = 3.f;
					m_pGameInstance->Set_ShakeCam(true);
				}

			}
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}
	}

	else if (m_eState == Skill3)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();

		if (Index >= 2.f && Index < 12.f)
		{
			Cam_AttackZoom(0.5f);
			m_bHide = true;
		}
		else if (Index >= 13.f && Index < 15.f)
		{
			m_bHide = false;
		}
		else if (Index >= 15.f && Index <= 27.f)
		{

			m_pTransformCom->Set_Speed(15.f);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}


		if (Index >= 27.f && Index <= 29.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill3);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill3]))
				{
					m_pGameInstance->Set_TimeRatio(0.2f);
					m_bAttacked = true;
					m_pGameInstance->Set_ShakeCam(true);
				}

			}


		}
		else if (Index > 29.f && Index <= 31.f)
		{
			m_bAttacked = false;
		}
		else if (Index >= 32.f && Index <= 34.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill3);
				if (m_pGameInstance->CheckCollision_Monster(m_pAttCollider[AT_Sword_Skill3]))
				{
					m_pGameInstance->Set_TimeRatio(0.2f);
					m_bAttacked = true;
					m_pGameInstance->Set_ShakeCam(true);
				}

			}
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}

	}
	else if (m_eState == Skill4)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 14.f && Index <= 20.f)
		{
			m_pGameInstance->Set_TimeRatio(0.1f);
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				m_bAttacked = true;
			}
		}
		else if (Index >= 26.f && Index <= 30.f)
		{
			m_pGameInstance->Set_TimeRatio(0.1f);

		}
		else if (Index >= 35.f && Index <= 40.f)
		{
			m_pGameInstance->Set_TimeRatio(0.5f);

		}
		else if (Index >= 39.f && Index <= 44.f)
		{
			//m_pGameInstance->Set_TimeRatio(0.1f);

		}
		else if (Index >= 56.f && Index <= 59.f)
		{
			m_pGameInstance->Set_TimeRatio(0.1f);

		}
		else if (Index >= 67.f && Index <= 70.f)
		{
			m_pGameInstance->Set_TimeRatio(0.1f);

		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}

		if (Index >= 14.f && Index <= 16.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				m_bAttacked = true;
			}

			if (!m_pGameInstance->Get_AimMode())
			{
				m_SaveCamPos = m_pCameraTransform->Get_State(State::Pos);
				m_SaveCamLook = m_pCameraTransform->Get_State(State::Look);
			}

			m_pGameInstance->Set_AimMode(true, _vec3(-1.f, 2.f, 1.f));

			m_pGameInstance->Set_ShakeCam(true);
		}
		else if (Index >= 26.f && Index <= 28.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				m_bAttacked = true;
			}

			m_pGameInstance->Set_AimMode(true, _vec3(1.f, 2.f, 1.f));
			m_pCameraTransform->LookAt(m_pTransformCom->Get_CenterPos());
			m_pGameInstance->Set_ShakeCam(true);
		}
		else if (Index >= 33.f && Index <= 34.f)
		{

			//m_pGameInstance->Set_AimMode(true, _vec3(0.f, 6.f, 0.7f));
			m_pCameraTransform->LookAt(m_pTransformCom->Get_CenterPos());
		}
		else if (Index >= 39.f && Index <= 40.f)
		{
			//m_pGameInstance->Set_AimMode(true, _vec3(0.f, 6.f, 0.7f));
			m_pCameraTransform->LookAt(m_pTransformCom->Get_CenterPos());
		}
		else if (Index >= 56.f && Index <= 57.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				m_bAttacked = true;
			}

			m_pGameInstance->Set_AimMode(false);
			m_pGameInstance->Set_ShakeCam(true);

			Cam_AttackZoom(4.f);

		}
		else if (Index >= 67.f && Index <= 68.f)
		{
			if (!m_bAttacked)
			{
				Check_Att_Collider(AT_Sword_Skill4);
				m_bAttacked = true;
			}

			m_pGameInstance->Set_ShakeCam(true);
			Cam_AttackZoom(5.f);
		}
		else if (Index > 40.f)
		{
			_vec4 vLerpPos{};
			vLerpPos = XMVectorLerp(m_pCameraTransform->Get_State(State::Pos), m_SaveCamPos, 0.1f);
			m_pCameraTransform->Set_State(State::Pos, vLerpPos);
			_vec4 vLerpLook{};
			vLerpLook = XMVectorLerp(m_pCameraTransform->Get_State(State::Look), m_SaveCamLook, 0.2f);
			m_pCameraTransform->LookAt_Dir(vLerpLook);

		}
		else
		{
			m_pGameInstance->Set_AimMode(false);
		}
	}

}
void CPlayer::After_BowAtt(_float fTimeDelta)
{
	_float Index = m_pModelCom->Get_CurrentAnimPos();
	if (m_eState == Attack)
	{
		if (Index >= 7.f && Index <= 8.f && m_ReadyArrow)
		{


			Create_Arrow(AT_Bow_Common);
			m_ReadyArrow = false;
		}
	}
	else if (m_eState == Skill1)
	{

		if (Index >= 2.f && Index <= 4.f)
		{
			if (!m_bLockOn)
			{
				m_pGameInstance->Set_AimMode(true, _vec3(0.5f, 1.1f, 1.7f));
			}
		}
		if (Index >= 16.f && Index <= 19.f && m_ReadyArrow)
		{

			Create_Arrow(AT_Bow_Skill1);
			m_ReadyArrow = false;
		}
		else if (Index > 20.f && Index <= 25.f && !m_ReadyArrow)
		{
			m_ReadyArrow = true;
		}
		else if (Index >= 30.f && Index <= 32.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill1);
			m_ReadyArrow = false;
		}
		else if (Index > 33.f && Index <= 35.f && !m_ReadyArrow)
		{
			m_ReadyArrow = true;
		}
		else if (Index >= 47.f && Index <= 49.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill1);
			m_ReadyArrow = false;
		}
		if (Index >= 70.f)
		{
			if (!m_bLockOn)
			{
				m_pGameInstance->Set_AimMode(false);
			}
		}
	}
	else if (m_eState == Skill2)
	{

		if (Index > 5.f && Index < 25.f)
		{
			m_pTransformCom->Go_Backward(fTimeDelta);
		}
		if (Index >= 5.f && Index <= 6.f)
		{
			m_pTransformCom->Jump(7.f);
		}
		else if (Index >= 32.f && Index <= 33.f)
		{
			m_pGameInstance->Set_TimeRatio(0.2f);

		}
		else if (Index >= 34.f && Index <= 36.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill2);
			m_ReadyArrow = false;
		}
		else if (Index >= 44.f && Index <= 45.f)
		{
			m_pGameInstance->Set_ShakeCam(true);

		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}
	}
	else if (m_eState == Skill3)
	{
		if (Index >= 3.f && Index < 55.f)
		{
			Cam_AttackZoom(-3.f);
		}

		if (Index >= 19.f && Index <= 20.f)
		{
			m_pGameInstance->Set_TimeRatio(0.2f);
			Create_Arrow(AT_Bow_Skill3_Start);

			m_UsingMotionBlur = true;
		}
		else if (Index >= 50.f && m_ReadyArrow)
		{
			Create_Arrow(AT_Bow_Skill3);
			m_ReadyArrow = false;
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
			m_UsingMotionBlur = false;
		}
	}
	else if (m_eState == Skill4)
	{
		if (Index >= 2.f && Index < 50.f)
		{
			Cam_AttackZoom(-2.f);

		}
		if (Index >= 26.f && Index <= 27.f)
		{
			m_pGameInstance->Set_ShakeCam(true);
			m_pGameInstance->Set_TimeRatio(0.1f);
			m_UsingMotionBlur = true;
		}
		else
		{
			m_pGameInstance->Set_TimeRatio(1.f);
			m_UsingMotionBlur = false;
		}
	}
	else if (m_eState == SkillR)
	{
		if (Index >= 8.f && Index <= 9.f)
		{
			Create_Arrow(AT_Bow_SkillR);
			Cam_AttackZoom(2.f);
			m_pGameInstance->Set_ShakeCam(true, 0.1f);
		}
	}

}
void CPlayer::Create_Arrow(ATTACK_TYPE Att_Type)
{
	_mat offet = _mat::CreateTranslation(_vec3(0.f, 0.9f, 0.f));
	_mat bone = (*m_pModelCom->Get_BoneMatrix("bowstring"));
	_mat world = m_pTransformCom->Get_World_Matrix();
	world = offet * bone * world;
	Arrow_Type type{};





	switch (Att_Type)
	{
	case Client::AT_Bow_Common:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.Att_Type = AT_Bow_Common;
		if (m_bLockOn)
		{
			type.MonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		}
		else
		{
			type.MonCollider = nullptr;
		}

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_Skill1:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.Att_Type = AT_Bow_Skill1;
		if (m_bLockOn)
		{
			type.MonCollider = m_pGameInstance->Get_Nearest_MonsterCollider();
		}
		else
		{
			type.MonCollider = nullptr;
		}
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_Skill2:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.vLook.y -= 0.2f;
		type.Att_Type = AT_Bow_Skill2;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_Skill3_Start:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.vLook.y += 0.6f;
		type.Att_Type = AT_Bow_Skill3_Start;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_Skill3:
	{
		m_bArrowRain_Start = true;
		m_iArrowRain = 0;
	}
	break;
	case Client::AT_Bow_Skill4:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.vLook.y -= 0.25f;
		type.Att_Type = AT_Bow_Skill4;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case Client::AT_Bow_SkillR:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.Att_Type = AT_Bow_SkillR;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;

	default:
		break;
	}



}
void CPlayer::Summon_Riding(Riding_Type Type)
{

	if (Type == Bird)
	{
		m_pGameInstance->Set_FlyCam(true);
	}


	Riding_Desc Desc{};
	Desc.Type = Type;
	Desc.vSummonPos = m_pTransformCom->Get_State(State::Pos);

	wstring strName{};

	strName = TEXT("Prototype_GameObject_Riding");

	m_pRiding = dynamic_cast<CRiding*>(m_pGameInstance->Clone_Object(strName, &Desc));

	if (m_pRiding == nullptr)
	{
		return;
	}
}
void CPlayer::Tick_Riding()
{
	if (m_pRiding == nullptr)
	{
		return;
	}

	if (m_pRiding->Get_Delete())
	{
		if (m_pRiding->Get_RidingType() != Bird)
		{
			m_pTransformCom->Set_Position(_vec3(m_pRiding->Get_Pos() + _vec3(0.f, 2.f, 0.f)));
			m_bIsMount = false;
			Safe_Release(m_pRiding);
		}
		else
		{
			Safe_Release(m_pRiding);
			m_bIsMount = false;
			m_pGameInstance->Set_FlyCam(false);
			if (m_pTransformCom->Get_CenterPos().x > 1500.f)
				m_pGameInstance->Set_GoHome(true);
			else
				m_pGameInstance->Set_GoDungeon(true);
		}

		m_eState = Jump_End;
		m_Animation.iAnimIndex = Anim_jump_end;
		return;
	}

	m_pTransformCom->Set_Matrix(m_pRiding->Get_Mat());
	//m_pTransformCom->Set_Position(_vec3(m_pRiding->Get_Pos()));
	Riding_State State = m_pRiding->Get_State();
	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 2.f;
	if (m_Riding_State != State)
	{
		if (State == Riding_Run)
		{
			m_Animation.iAnimIndex = Anim_Mount_Run;
			m_Animation.isLoop = true;

		}
		else if (State == Riding_Idle)
		{
			m_Animation.iAnimIndex = Anim_Mount_Idle;
			m_Animation.isLoop = true;
		}
		else if (State == Riding_Walk)
		{
			m_Animation.iAnimIndex = Anim_Mount_Walk;
			m_Animation.isLoop = true;
		}
		m_Riding_State = State;
	}
}


void CPlayer::Arrow_Rain()
{

	if (m_iArrowRain == 0)
	{
		m_vArrowLook = m_pTransformCom->Get_State(State::Look);
	}

	if (m_iArrowRain < 80)
	{
		Arrow_Type Type{};
		Type.Att_Type = AT_Bow_Skill3;
		_float random = (_float)(rand() % 100);
		_int randommos = rand() % 2;
		if (randommos == 0)
		{
			random *= -1;
		}
		random *= 0.05f;
		_float  random2 = (_float)(rand() % 101);
		int randommo = rand() % 2;
		if (randommo == 0)
		{
			random2 *= -1;
		}
		random2 *= 0.05f;
		if (m_vArrowRainPos == _vec4())
		{
			Type.vPos = m_pTransformCom->Get_State(State::Pos) + m_vArrowLook * 10.f + _vec4(random, 10.f, random2, 0.f)/* + m_pTransformCom->Get_State(State::Right) * 4.f*/;
		}
		else
		{
			Type.vPos = m_vArrowRainPos + _vec4(random, 10.f, random2, 0.f);
		}
		Type.vLook = _vec4(0.01f, -1.f, 0.f, 0.f);

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &Type)))
		{
			return;
		}
		m_iArrowRain++;
	}
	else
	{
		m_vArrowRainPos = _vec4();
		m_bArrowRain_Start = false;
	}
}

void CPlayer::Init_State()
{
	if (m_eState != m_ePrevState)
	{
		m_Animation.isLoop = false;
		m_Animation.bRestartAnimation = false;
		m_Animation.bSkipInterpolation = false;
		m_Animation.fDurationRatio = 1.f;
		m_Animation.fStartAnimPos = 0.f;
		m_iSuperArmor = 0;
		m_Animation.fAnimSpeedRatio = 2.f;
		if (m_pGameInstance->Get_TimeRatio() < 1.f)
		{
			m_pGameInstance->Set_TimeRatio(1.f);
		}


		switch (m_eState)
		{
		case Client::CPlayer::Idle:
		{
			m_Animation.iAnimIndex = Anim_idle_00;
			m_Animation.isLoop = true;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Walk:
		{
			m_Animation.iAnimIndex = Anim_Normal_Walk;
			m_Animation.isLoop = true;
			m_iSuperArmor = {};
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Run_Start:
		{
			m_Animation.iAnimIndex = Anim_Normal_run_start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Run:
		{
			m_Animation.iAnimIndex = Anim_Normal_run;
			m_Animation.isLoop = true;
			m_iSuperArmor = {};
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Run_End:
		{
			m_Animation.iAnimIndex = Anim_Normal_run_stop;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump_Start:
		{
			m_Animation.iAnimIndex = Anim_jump_start;
			m_Animation.isLoop = false;
			m_hasJumped = true;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump:
		{
			m_Animation.iAnimIndex = Anim_jump_loop;
			m_Animation.isLoop = true;
			m_hasJumped = true;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump_Run:
		{
			m_Animation.iAnimIndex = Anim_jump_end_run;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump_End:
		{
			m_Animation.iAnimIndex = Anim_jump_end;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Jump_Long_End:
		{
			m_Animation.iAnimIndex = Anim_jump_end_long;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Attack:
		{
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Attack_Idle:
			if (m_Current_Weapon == WP_SWORD)
			{
				m_Animation.iAnimIndex = Anim_Assassin_Battle_Idle;
				m_Animation.isLoop = true;
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				m_Animation.iAnimIndex = Anim_B_idle_end;
				m_Animation.isLoop = false;
			}

			m_hasJumped = false;
			m_iSuperArmor = {};
			break;
		case Client::CPlayer::Attack_Run:
		{
			if (m_Current_Weapon == WP_SWORD)
			{
				m_Animation.iAnimIndex = Anim_Assassin_Battle_Run_end;
			}
			else if (m_Current_Weapon == WP_BOW)
			{
				m_Animation.iAnimIndex = Anim_Sniper_Battle_Run_end;
			}
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Skill1:
			Skill1_Attack();
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
		{
			m_Animation.iAnimIndex = Anim_Sniper_B_idle;
			m_Animation.isLoop = true;
			m_hasJumped = false;
			m_iSuperArmor = {};
		}
		break;
		case Client::CPlayer::Climb:
		{
			m_Animation.iAnimIndex = Anim_Climb_wait;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Climb_U:
		{
			m_Animation.iAnimIndex = Anim_Climb_F;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Climb_D:
		{
			m_Animation.iAnimIndex = Anim_Climb_B;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Climb_R:
		{
			m_Animation.iAnimIndex = Anim_Climb_R;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Climb_L:
		{
			m_Animation.iAnimIndex = Anim_Climb_L;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Swim_Idle:
		{
			m_Animation.iAnimIndex = Anim_Swim_Idle;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Swim:
		{
			m_Animation.iAnimIndex = Anim_swim_F;
			m_Animation.isLoop = true;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Swim_collect:
		{
			m_Animation.iAnimIndex = Anim_swim_collect;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Stun_Start:
		{
			m_Animation.iAnimIndex = Anim_Stun_start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Stun:
		{
			m_Animation.iAnimIndex = Anim_stun;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Hit:
		{
			m_Animation.iAnimIndex = Anim_Stun_start;
			m_Animation.fDurationRatio = 0.4f;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fStartAnimPos = 18.f;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::KnockDown:
		{
			if (m_Current_Weapon == WP_SWORD)
				m_Animation.iAnimIndex = Anim_Assassin_knockdown;
			else
				m_Animation.iAnimIndex = Anim_Sniper_knockdown;

			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Revival_Start:
		{
			m_Animation.iAnimIndex = Anim_revival_start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Revival_End:
		{
			m_Animation.iAnimIndex = Anim_revival_end;
			m_Animation.isLoop = false;
			m_hasJumped = false;
			m_Status.Current_Hp = m_Status.Max_Hp;
			m_Status.Current_Mp = m_Status.Max_Mp;
			CUI_Manager::Get_Instance()->Set_Hp(m_Status.Current_Hp, m_Status.Max_Hp);
		}
		break;
		case Client::CPlayer::Die:
		{
			m_Animation.iAnimIndex = Anim_die;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Collect_Start:
		{
			m_Animation.iAnimIndex = Anim_Collect_Start;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Collect_Loop:
		{
			m_Animation.iAnimIndex = Anim_Collect_loop;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Collect_End:
		{
			m_Animation.iAnimIndex = Anim_Collect_end;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Mining:
		{
			m_Animation.iAnimIndex = Anim_Mining;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
		break;
		case Client::CPlayer::Logging:
		{
			m_Animation.iAnimIndex = Anim_logging;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
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
		Return_Attack_IdleForm();
		if (m_Current_Weapon == WP_SWORD)
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

	}
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
		if (m_Current_Weapon == WP_SWORD)
		{

			if (m_pModelCom->IsAnimationFinished(m_SwordSkill[0]))
			{
				m_eState = Attack_Idle;

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
	case Client::CPlayer::Skill2:
	{
		if (m_Current_Weapon == WP_SWORD)
		{

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
				m_eState = Aim_Idle;

			}
		}

		break;
	case Client::CPlayer::Aim_Idle:

		break;
	case Client::CPlayer::Jump_Start:
		if (m_pModelCom->IsAnimationFinished(Anim_jump_start))
		{
			m_eState = Jump;
		}
		break;
	case Client::CPlayer::Jump:
		break;
	case Client::CPlayer::Jump_End:
		if (m_pModelCom->IsAnimationFinished(Anim_jump_end))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Jump_Long_End:
		if (m_pModelCom->IsAnimationFinished(Anim_jump_end_long))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Jump_Run:
		if (m_pModelCom->IsAnimationFinished(Anim_jump_end_run))
		{
			m_eState = Run;
		}
		break;
	case Client::CPlayer::Stun_Start:
		if (m_pModelCom->IsAnimationFinished(Anim_Stun_start))
		{
			m_eState = Stun;
		}
		break;
	case Client::CPlayer::Stun:
		if (m_pModelCom->IsAnimationFinished(Anim_stun))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Swim_Idle:
		break;
	case Client::CPlayer::Swim:
		if (!m_bReadySwim)
		{
			m_eState = Idle;
		}
		if (m_pModelCom->IsAnimationFinished(Anim_swim_F))
		{
			m_eState = Swim_Idle;
		}
		break;
	case Client::CPlayer::Swim_collect:
		m_Animation.bRestartAnimation = false;
		if (m_pModelCom->IsAnimationFinished(Anim_swim_collect))
		{
			m_iSwimCollectCount++;
			if (m_iSwimCollectCount >= 3)
			{
				m_eState = Swim_Idle;
				m_Animation.iAnimIndex = Anim_Swim_Idle;
			}
			else
			{
				m_Animation.iAnimIndex = Anim_swim_collect;
				m_Animation.bRestartAnimation = true;
			}
		}
		break;
	case Client::CPlayer::Hit:
		if (m_pModelCom->IsAnimationFinished(Anim_Stun_start))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::KnockDown:
		if (m_Current_Weapon == WP_SWORD)
			if (m_pModelCom->IsAnimationFinished(Anim_Assassin_knockdown))
			{
				m_eState = Idle;
			}
			else
			{
				if (m_pModelCom->IsAnimationFinished(Anim_Sniper_knockdown))
				{
					m_eState = Idle;
				}
			}
		break;
	case Client::CPlayer::Revival_Start:
		if (m_pModelCom->IsAnimationFinished(Anim_revival_start))
		{
			m_eState = Revival_End;
		}
		break;
	case Client::CPlayer::Revival_End:
		if (m_pModelCom->IsAnimationFinished(Anim_revival_end))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Die:
		if (m_pModelCom->IsAnimationFinished(Anim_die))
		{
			m_eState = Revival_Start;
		}
		break;
	case Client::CPlayer::Collect_Start:
		if (m_pModelCom->IsAnimationFinished(Anim_Collect_Start))
		{
			m_eState = Collect_Loop;
		}
		break;
	case Client::CPlayer::Collect_Loop:
		if (m_pModelCom->IsAnimationFinished(Anim_Collect_loop))
		{
			m_eState = Collect_End;
		}
		break;
	case Client::CPlayer::Collect_End:
		if (m_pModelCom->IsAnimationFinished(Anim_Collect_end))
		{
			m_eState = Idle;
		}
		break;
	case Client::CPlayer::Mining:
		m_Animation.bRestartAnimation = false;
		if (m_pModelCom->IsAnimationFinished(Anim_Mining))
		{
			m_iMiningCount++;
			if (m_iMiningCount >= 3)
			{
				m_eState = Idle;
				m_Animation.iAnimIndex = Anim_idle_00;
			}
			else
			{
				m_Animation.iAnimIndex = Anim_Mining;
				m_Animation.bRestartAnimation = true;
			}
		}
		break;
	case Client::CPlayer::Logging:
		m_Animation.bRestartAnimation = false;
		if (m_pModelCom->IsAnimationFinished(Anim_logging))
		{
			m_iLoggingCount++;
			if (m_iLoggingCount >= 3)
			{
				m_eState = Idle;
				m_Animation.iAnimIndex = Anim_idle_00;

			}
			else
			{
				m_Animation.iAnimIndex = Anim_logging;
				m_Animation.bRestartAnimation = true;
			}
		}
		break;
	default:
		break;
	}

}

void CPlayer::Update_Trail(_float fTimeDelta)
{
	_mat _Matrix{};
	if (m_pGameInstance->Get_CameraModeIndex() == CM_MAIN)
	{
		if (m_pLeft_Trail != nullptr)
		{
			_Matrix = _mat::CreateTranslation(0.f, -0.8f, 0.f) * *m_Left_Mat * m_pTransformCom->Get_World_Matrix();
			_mat _Matrix2 = _mat::CreateTranslation(0.f, -1.2f, 0.f) * *m_Left_Mat * m_pTransformCom->Get_World_Matrix();
			for (int i = 0; i < 5; i++)
			{
				m_pLeft_Trail[i]->Tick(_Matrix2.Position_vec3(), _Matrix.Position_vec3());

			}
		}

		if (m_pRight_Trail != nullptr)
		{
			_Matrix = _mat::CreateTranslation(0.f, 0.8f, 0.f) * *m_Right_Mat * m_pTransformCom->Get_World_Matrix();
			_mat _Matrix2 = _mat::CreateTranslation(0.f, 1.2f, 0.f) * *m_Right_Mat * m_pTransformCom->Get_World_Matrix();
			for (int i = 0; i < 5; i++)
			{
				m_pRight_Trail[i]->Tick(_Matrix2.Position_vec3(), _Matrix.Position_vec3());

			}
		}
	}
}

HRESULT CPlayer::Add_Components()
{

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_RTVTF"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::OBB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.4f, 1.3f, 0.4f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.8f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Player_Hit_OBB"), (CComponent**)&m_pHitCollider, &CollDesc)))
	{
		return E_FAIL;
	}

	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(1.2f, 2.f, 1.2f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 0.4f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Common_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Common]), &CollDesc)))
	{
		return E_FAIL;
	}





	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(2.f, 2.f, 1.2f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill1_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill1]), &CollDesc)))
	{
		return E_FAIL;
	}

	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(1.5f, 2.0f, 0.3f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.3f, 0.8f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Parrying"), reinterpret_cast<CComponent**>(&m_pParryingCollider), &CollDesc)))
	{
		return E_FAIL;
	}

	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.65f, 2.f, 1.5f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill3_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill3]), &CollDesc)))
	{
		return E_FAIL;
	}
	CollDesc.vExtents = _vec3(2.3f, 2.f, 1.8f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 0.2f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill2_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill2]), &CollDesc)))
	{
		return E_FAIL;
	}
	CollDesc.vExtents = _vec3(2.3f, 2.f, 2.3f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.2f, 0.2f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Skill4_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Sword_Skill4]), &CollDesc)))
	{
		return E_FAIL;
	}


	CollDesc.eType = ColliderType::Frustum;
	_mat matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_mat matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(50.f), 2.f, 0.01f, 20.f);


	CollDesc.matFrustum = matView * matProj;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_CommonBow_Att"), reinterpret_cast<CComponent**>(&m_pAttCollider[AT_Bow_Common]), &CollDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{

	// WorldMatrix 바인드
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	// ViewMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	// ProjMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	// 카메라 Far 바인드
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &CUI_Manager::Get_Instance()->Get_HairColor(), sizeof _vec4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
	{
		return E_FAIL;
	}

	// 모션블러용 이전프레임 WorldMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldWorldMatrix", m_OldWorldMatrix)))
	{
		return E_FAIL;
	}

	// 모션블러용 이전프레임 ViewMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix_vec4x4())))
	{
		return E_FAIL;
	}

	m_pModelCom->Set_UsingMotionBlur(m_UsingMotionBlur);

	// 뼈 바인드
	if (FAILED(m_pModelCom->Bind_Bone(m_pShaderCom)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
	{
		return E_FAIL;
	}

	_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof(_float4))))
	{
		return E_FAIL;
	}



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
	{
		Safe_Release(m_pRiding);
	}

	for (int i = 0; i < 5; i++)
	{
		Safe_Release(m_pLeft_Trail[i]);
		Safe_Release(m_pRight_Trail[i]);
	}


	Safe_Release(m_pTest_Trail);
	Safe_Release(m_pNameTag);
	Safe_Release(m_pParryingCollider);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCameraTransform);
	Safe_Release(m_pHitCollider);
}
