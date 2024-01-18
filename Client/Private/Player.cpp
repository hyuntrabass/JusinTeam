#include "Player.h"
#include "BodyPart.h"
#include "UI_Manager.h"
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
	Add_Parts();
	m_pTransformCom->Set_Speed(1);
	m_pCameraTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	Safe_AddRef(m_pCameraTransform);
	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{

	PART_TYPE eType = CUI_Manager::Get_Instance()->Is_CustomPartChanged();
	if (PART_TYPE::PT_END != eType)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		Change_Parts(eType, CUI_Manager::Get_Instance()->Get_CustomPart(eType));
	}

	m_fAttTimer += fTimeDelta;
	
	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_CUSTOM&&m_vecParts[PT_BODY]->Get_ModelIndex()!=0)
	{
		
		Set_Key(fTimeDelta);
		
		Move(fTimeDelta);
		Init_State();
		Tick_State(fTimeDelta);
	}


	_float fMouseSensor = 0.1f;
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CUSTOM)
	{
		if (!CUI_Manager::Get_Instance()->Is_Picking_UI() && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
		{
			_long dwMouseMove;

			if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -1.f * 0.1f);
			}
		}
	}

	if(m_Current_Weapon == WP_SWORD)
	Sword_Attack_Dash(fTimeDelta);

	for (int i = 0; i < m_vecParts.size(); i++)
	{
		m_vecParts[i]->Tick(fTimeDelta);
	}

	if(m_pWeapon!=nullptr)
	m_pWeapon->Tick(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (!m_bStartGame && m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
	{
		Change_Parts(PT_FACE, 7);
		Change_Parts(PT_HAIR, 9);
		Change_Parts(PT_BODY, 1);
		m_pTransformCom->Set_Scale(_vec3(0.1f));
		Add_Weapon();
		m_bStartGame = true;
	}

	for (int i = 0; i < m_vecParts.size();i++)
	{
		m_vecParts[i]->Late_Tick(fTimeDelta);
	}

	if (m_pWeapon != nullptr)
	m_pWeapon->Late_Tick(fTimeDelta);

}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Add_Parts()
{
	CBodyPart* pParts = { nullptr };

	BODYPART_DESC BodyParts_Desc{};
	BodyParts_Desc.pParentTransform = m_pTransformCom;
	BodyParts_Desc.Animation = &m_Animation;
	BodyParts_Desc.eType = PT_HAIR;
	BodyParts_Desc.iNumVariations = 10;
	
	wstring strName{};

	strName = TEXT("Prototype_GameObject_Body_Parts");
	pParts = dynamic_cast<CBodyPart*>(m_pGameInstance->Clone_Object(strName, &BodyParts_Desc));

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	BodyParts_Desc.eType = PT_FACE;
	BodyParts_Desc.iNumVariations = 8;


	pParts = dynamic_cast<CBodyPart*>(m_pGameInstance->Clone_Object(strName, &BodyParts_Desc));

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	BodyParts_Desc.eType = PT_BODY;
	BodyParts_Desc.iNumVariations = 4;

	pParts = dynamic_cast<CBodyPart*>(m_pGameInstance->Clone_Object(strName, &BodyParts_Desc));

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	return S_OK;
}

HRESULT CPlayer::Add_Weapon()
{

	

	WEAPONPART_DESC WeaponParts_Desc{};
	WeaponParts_Desc.pParentTransform = m_pTransformCom;
	WeaponParts_Desc.Animation = &m_Animation;
	WeaponParts_Desc.iNumVariations = 6;

	wstring strName{};

	strName = TEXT("Prototype_GameObject_Weapon");
	m_pWeapon = m_pGameInstance->Clone_Object(strName, &WeaponParts_Desc);

	if (m_pWeapon == nullptr)
		return E_FAIL;

	m_Current_Weapon = WP_SWORD;
	Reset_PartsAnim();
	return S_OK;
}

void CPlayer::Change_Parts(PART_TYPE PartsType, _int ChangeIndex)
{
	dynamic_cast<CBodyPart*>(m_vecParts[PartsType])->Set_ModelIndex(ChangeIndex);
	Reset_PartsAnim();
}

void CPlayer::Change_Weapon(WEAPON_TYPE PartsType, _int ChangeIndex)
{
	dynamic_cast<CWeapon*>(m_pWeapon)->Set_ModelIndex(PartsType);

}

void CPlayer::Reset_PartsAnim()
{
	dynamic_cast<CBodyPart*>(m_vecParts[PT_HAIR])->Reset_Model();
	dynamic_cast<CBodyPart*>(m_vecParts[PT_FACE])->Reset_Model();
	dynamic_cast<CBodyPart*>(m_vecParts[PT_BODY])->Reset_Model();

	if(m_pWeapon!=nullptr)
	dynamic_cast<CWeapon*>(m_pWeapon)->Reset_Model();
}

void CPlayer::Set_Key(_float fTimeDelta)
{

	

	
}

void CPlayer::Move(_float fTimeDelta)
{

	_bool hasMoved{};
	_vec4 vForwardDir = m_pGameInstance->Get_CameraLook();
	vForwardDir.y = 0.f; 
	_vec4 vRightDir = XMVector3Cross(m_pTransformCom->Get_State(State::Up), vForwardDir);
	
	_vec4 vDirection{};

	if(m_eState!=Attack)
	{
		if (m_pGameInstance->Key_Pressing(DIK_W))
		{
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
	}

	

	if (hasMoved)
	{

		if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if (/*m_pTransformCom->Is_OnGround() and*/
				m_eState == Walk or 
				m_eState == Idle or
				m_eState == Attack_Idle 

				)
			{
				m_eState = Run_Start;
				m_pTransformCom->Set_Speed(m_fRunSpeed);
			 }
			else if (/*m_pTransformCom->Is_OnGround() and*/
				m_eState == Run or 
				m_eState == Run_End or
				m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Normal_run_start))
			{
				m_eState = Run;
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

		

		_vec4 vLook = m_pTransformCom->Get_State(State::Look);
		_float fInterpolTime = 0.4f;
		m_vOriginalLook = vLook;
		/*if (m_fInterpolationRatio < fInterpolTime)
		{
			if (not m_isInterpolating)
			{
				
				m_isInterpolating = true;
			}


			
		}
		else
		{
			m_isInterpolating = false;
			m_fInterpolationRatio = 0.f;
		}*/

		m_fInterpolationRatio += fTimeDelta;
		_float fRatio = m_fInterpolationRatio / fInterpolTime;

		vDirection = Lerp(m_vOriginalLook, vDirection, m_fInterpolationRatio);
			m_pTransformCom->LookAt_Dir(vDirection);
			m_pTransformCom->Go_To_Dir(vDirection, fTimeDelta);
	}
	else if (m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Normal_Walk))
	{
		m_eState = Idle;
	}

	m_currentDir = Lerp(m_currentDir, vDirection, m_lerpFactor);

	//m_pTransformCom->LookAt_Dir(m_currentDir);
	//m_pTransformCom->Go_To_Dir(m_currentDir, fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_F))
	{
		vDirection += vForwardDir;
		//m_pTransformCom->LookAt_Dir(vDirection);
		m_pTransformCom->Set_Speed(0.5f);
		m_pTransformCom->Go_To_Dir(vDirection, fTimeDelta);

		if(m_Current_Weapon == WP_SWORD)
		Common_SwordAttack();

		m_eState = Attack;

	}

}
void CPlayer::Common_SwordAttack()
{
	if (m_fAttTimer < 0.55f)
		return;

	if (m_fAttTimer > 1.1f || m_iAttackCombo > 3 || (m_eState != Attack_Idle && m_eState != Attack))
		m_iAttackCombo = 0;

	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;
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
		break;
	case 2:
		m_Animation.iAnimIndex = Anim_Assassin_Attack03_A;
		m_fAttTimer = 0.f;
		m_iAttackCombo++;
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
void CPlayer::Return_Attack_IdleForm()
{
	if (m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Assassin_Attack01_A))
		m_Animation.iAnimIndex = Anim_Assassin_Attack01_B;
	else if(m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Assassin_Attack02_A))
		m_Animation.iAnimIndex = Anim_Assassin_Attack02_B;
	else if (m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Assassin_Attack03_A))
		m_Animation.iAnimIndex = Anim_Assassin_Attack03_B;
	else if (m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Assassin_Attack04_A))
		m_Animation.iAnimIndex = Anim_Assassin_Attack04_B;


}
void CPlayer::Sword_Attack_Dash(_float fTimeDelta)
{
	if (m_fAttTimer > 0.2f && m_fAttTimer < 0.37f)
	{
		if (m_iAttackCombo == 1)
		{
			m_pTransformCom->Set_Speed(7.f);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		else if (m_iAttackCombo == 2)
		{
			m_pTransformCom->Set_Speed(5.f);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		else if (m_iAttackCombo == 3)
		{
			m_pTransformCom->Set_Speed(5.f);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		else if (m_iAttackCombo == 4)
		{
			m_pTransformCom->Set_Speed(25.f);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
	}
}
void CPlayer::Init_State()
{
	if (m_eState != m_ePrevState)
	{
		m_Animation = {};
		m_Animation.fAnimSpeedRatio = 2.f;
		m_Animation.bSkipInterpolation = false;
		switch (m_eState)
		{
		case Client::CPlayer::Idle:
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
			if (m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Normal_run_stop))
			{
				m_eState = Idle;
				m_hasJumped = false;
			}
			break;
		case Client::CPlayer::Attack:
		{
			Return_Attack_IdleForm();

			if (m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Assassin_Attack01_B) or
				m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Assassin_Attack02_B) or
				m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Assassin_Attack03_B) or
				m_vecParts[PT_FACE]->IsAnimationFinished(Anim_Assassin_Attack04_B)
				)
			{
				m_eState = Attack_Idle;
			}
		}
			break;
		case Client::CPlayer::Attack_Idle:
			m_Animation.iAnimIndex = Anim_Assassin_Battle_Idle;
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
	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
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


	for (auto& iter : m_vecParts)
	{
		Safe_Release(iter);
	}

	m_vecParts.clear();

	Safe_Release(m_pWeapon);
	Safe_Release(m_pCameraTransform);

}
