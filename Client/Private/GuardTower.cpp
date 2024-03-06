#include "GuardTower.h"
#include "UI_Manager.h"
#include "Effect_Dummy.h"
#include "Trigger_Manager.h"

_int CGuardTower::m_iLightID = {};


CGuardTower::CGuardTower(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CGuardTower::CGuardTower(const CGuardTower& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGuardTower::Init_Prototype()
{
	m_isPrototype = true;

	return S_OK;
}

HRESULT CGuardTower::Init(void* pArg)
{
	m_Info = *(GuardTowerInfo*)pArg;
	m_Pattern_Type = (PATTERN_TYPE)m_Info.iIndex;
	m_GuardTowerMatrix = m_Info.mMatrix;
	m_LazerMatrix = m_GuardTowerMatrix;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Collider()))
		return E_FAIL;

	if (FAILED(Add_Attack_Collider()))
		return E_FAIL;


	m_iHP = 10000;

	m_pTransformCom->Set_Matrix(m_GuardTowerMatrix);

	m_pShaderCom->Set_PassIndex(VTF_InstPass_Default);

	if (m_Pattern_Type == PATTERN_1)
		Create_Range_Pattern_1();
	else if (m_Pattern_Type == PATTERN_2)
		Create_Range_Pattern_2();
	else
		Create_Range_Pattern_3();

	Create_Lazer();
	Create_Attack_Lazer();


	m_eCurState = STATE_IDLE;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 2.f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.8f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	CUI_Manager::Get_Instance()->Set_RadarPos(CUI_Manager::MONSTER, m_pTransformCom);


	LIGHT_DESC LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE::Point;
	LightDesc.vSpecular = _vec4(1.f);
	LightDesc.vDiffuse = _vec4(0.5f);
	LightDesc.vAmbient = _vec4(0.1f);
	LightDesc.vPosition = m_pTransformCom->Get_State(State::Pos) + _vec4(0.f, 1.5f, 0.f, 0.f);
	LightDesc.vAttenuation = LIGHT_RANGE_50;

	m_strLightTag = L"Light_GuardTower_" + to_wstring(m_iLightID++);
	if (FAILED(m_pGameInstance->Add_Light(LEVEL_TOWER, m_strLightTag, LightDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CGuardTower::Tick(_float fTimeDelta)
{
	if (true == m_isTurnOff) {
		if (true == m_LightTurnOff)
			return;
		if (0.f >= m_fEffectScale) {
			m_pGameInstance->Delete_Light(LEVEL_TOWER, m_strLightTag);
			m_LightTurnOff = true;
			return;
		}
		m_fEffectScale -= fTimeDelta * 5.f;
		LIGHT_DESC* Desc = m_pGameInstance->Get_LightDesc(LEVEL_TOWER, m_strLightTag);
		Desc->vDiffuse -= _vec3(fTimeDelta * 0.1f);
		Desc->vSpecular -= _vec3(fTimeDelta * 0.1f);
		Desc->vAmbient -= _vec3(fTimeDelta * 0.1f);
		Desc->vAttenuation -= _vec4(fTimeDelta * 0.1f, 0.f, 0.f, 0.f);
	}


	if (m_bAttacked == true)
	{
		m_fAttackDelay += fTimeDelta;
		if (m_fAttackDelay >= 1.5f)
		{
			m_bAttacked = false;
			m_fAttackDelay = 0;
		}
	}
	if (m_eCurState == STATE_DETECT)
	{
		if (m_pFrameEffect)
			m_pFrameEffect->Tick(fTimeDelta);
		if (m_pBaseEffect)
			m_pBaseEffect->Tick(fTimeDelta);
	}
	if (m_eCurState == STATE_ATTACK_READY)
	{
		if (m_pThreatEffect)
			m_pThreatEffect->Tick(fTimeDelta);
	}
	if (m_eCurState == STATE_ATTACK)
	{
		if (m_pAttackEffect)
			m_pAttackEffect->Tick(fTimeDelta);
	}
	m_pTransformCom->Set_OldMatrix();
	m_pModelCom->Get_CurrentAnimPos();

	Init_State(fTimeDelta);
	if (m_Pattern_Type == PATTERN_1)
	{
		Tick_State_Pattern_1(fTimeDelta);
	}
	else if (m_Pattern_Type == PATTERN_2)
	{
		Tick_State_Pattern_2(fTimeDelta);
	}
	else if (m_Pattern_Type == PATTERN_3)
	{
		Tick_State_Pattern_3(fTimeDelta);
	}


	m_pModelCom->Set_Animation(m_Animation);
	Update_Collider();
}

void CGuardTower::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);

	if (m_eCurState == STATE_DETECT)
	{
		if (m_pFrameEffect)
			m_pFrameEffect->Late_Tick(fTimeDelta);

		if (m_pBaseEffect)
			m_pBaseEffect->Late_Tick(fTimeDelta);
	}

	if (m_eCurState == STATE_ATTACK_READY)
	{
		if (m_pThreatEffect)
			m_pThreatEffect->Late_Tick(fTimeDelta);
	}
	if (m_eCurState == STATE_ATTACK)
	{
		if (m_pAttackEffect)
			m_pAttackEffect->Late_Tick(fTimeDelta);
	}

	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_CenterPos()))
	{
		m_pRendererCom->Add_RenderGroup(RG_AnimNonBlend_Instance, this);
		m_pModelCom->Set_DissolveRatio(m_fDissolveRatio);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);

#endif // _DEBUG

}

HRESULT CGuardTower::Render()
{
	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;

	//for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i) {
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
	//		return E_FAIL;

	//	_bool HasNorTex{};
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
	//	{
	//		HasNorTex = false;
	//	}
	//	else
	//	{
	//		HasNorTex = true;
	//	}

	//	_bool HasMaskTex{};
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
	//	{
	//		HasMaskTex = false;
	//	}
	//	else
	//	{
	//		HasMaskTex = true;
	//	}

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
	//	{
	//		return E_FAIL;
	//	}

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
	//	{
	//		return E_FAIL;
	//	}

	//	//if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
	//	//	return E_FAIL;

	//	if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
	//		return E_FAIL;

	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}

	//if (!m_bChangePass && m_iHP > 0)
	//{
	//	m_iPassIndex = AnimPass_Default;
	//}

	return S_OK;
}

void CGuardTower::Set_Damage(_int iDamage, _uint iDamageType)
{

	if (iDamageType == AT_Sword_Common)
	{
		m_iHP -= iDamage;
		m_bChangePass = true;
	}
	CUI_Manager::Get_Instance()->Set_HitEffect(m_pTransformCom, iDamage, _vec2(0.f, 2.f), (ATTACK_TYPE)iDamageType);

}

void CGuardTower::Init_State(_float fTimeDelta)
{
	if (m_iHP <= 0)
	{
		m_eCurState = STATE_DIE;
	}


	_vec4 vPlayerPos = Compute_PlayerPos();
	_float fDistance = Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_CenterPos()).Get_Normalized();

	if (m_ePreState != m_eCurState) {
		switch (m_eCurState)
		{
		case Client::CGuardTower::STATE_IDLE:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fStartAnimPos = 0.f;
			break;
		case Client::CGuardTower::STATE_DETECT:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			break;
		case Client::CGuardTower::STATE_ATTACK_READY:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = false;
			m_Animation.fStartAnimPos = 50.f;
			m_Animation.fAnimSpeedRatio = 10.f;

			break;
		case Client::CGuardTower::STATE_ATTACK:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 10.f;
			break;
		case Client::CGuardTower::STATE_DIE:
			m_Animation.iAnimIndex = ANIM_DIE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		}

		m_ePreState = m_eCurState;
	}

}

void CGuardTower::Tick_State_Pattern_1(_float fTimeDelta)
{
	_vec4 vPlayerPos = Compute_PlayerPos();
	_float fDistance = Compute_PlayerDistance();
	_vec4 vLazerLook{};
	_float fDist = 1.0f;
	View_Detect_Range_Pattern_1(m_fDetectTime);

	switch (m_eCurState)
	{
	case Client::CGuardTower::STATE_IDLE:
		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			m_eCurState = STATE_DETECT;
			m_fIdleTime = 0.f;
		}

		break;

	case Client::CGuardTower::STATE_DETECT:


		m_fDetectTime += fTimeDelta * 20.f;
		if (m_bInit == true)
		{
			m_fDetectTime = 0.f;
			m_bInit = false;
		}

		if (m_isDetected == true)
		{
			m_eCurState = STATE_ATTACK_READY;
		}

		break;
	case Client::CGuardTower::STATE_ATTACK_READY:
		//Delete_Range();

		m_fAttackTime += fTimeDelta;

		if (m_isDetected == false)
		{
			m_eCurState = STATE_IDLE;
			m_fAttackTime = 0.f;

		}
		else
		{
			if (m_fAttackTime > 2.f)
			{
				m_eCurState = STATE_ATTACK;
				m_vCurPlayerPos = Compute_PlayerPos();
				if (m_pAttackEffect->isDead() == true)
					Create_Attack_Lazer();
				m_pAttackColliderCom->Change_Extents(_vec3(0.25f, 6.f, 0.25f));
				m_fAttackTime = 0.f;
			}
		}

		break;
	case Client::CGuardTower::STATE_ATTACK:

		m_fAttackTime += fTimeDelta;

		if (m_bAttacked == false)
		{
			m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
			m_bAttacked = true;
		}

		if (m_fAttackTime > 1.f)
		{
			if (m_isDetected == false)
				m_eCurState = STATE_DETECT;
			else
			{
				m_eCurState = STATE_ATTACK_READY;
			}
			m_fAttackTime = 0.f;
		}
		break;

	case Client::CGuardTower::STATE_DIE:
		if (m_pModelCom->IsAnimationFinished(ANIM_DIE))
		{
			m_fDeadTime += fTimeDelta;
		}


		break;
	}
}

void CGuardTower::View_Detect_Range_Pattern_1(_float fTimeDelta)
{
	_float fRadian = fTimeDelta;
	_float fDistance = 11.5f;
	m_EffectMatrix = _mat::CreateScale(m_fEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRadian)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 0.1f, 0.f);
	m_LazerMatrix = _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 3.f, 0.f);

	_vec4 vTowerLook = m_EffectMatrix.Up().Get_Normalized();
	//_vec4 vLazerLook = Compute_Player_To_Dir(m_LazerMatrix.Position()).Get_Normalized();

	//m_LazerMatrix.Up(Compute_PlayerPos());
	Compute_Lazer_Dir();

	_float fDetectRadian = XMConvertToDegrees(acosf(vTowerLook.Dot(Compute_Player_To_Dir(m_pTransformCom->Get_State(State::Pos)))));

	PxRaycastBuffer pBuffer{};

	if (fDetectRadian < 22.5f && Compute_PlayerDistance() <= fDistance)
	{
		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), Compute_Player_To_Dir(m_pTransformCom->Get_CenterPos()), 100.f, pBuffer))
		{
			if (pBuffer.block.distance > Compute_PlayerDistance())
			{
				if (CUI_Manager::Get_Instance()->Get_Hp().x > 0)
					m_isDetected = true;
				else
					m_isDetected = false;
			}
			else
				m_isDetected = false;
		}
		else
			m_isDetected = false;
	}
	else
	{
		m_isDetected = false;
	}

	if (fRadian >= 360.f)
	{
		m_bInit = true;
	}
}


void CGuardTower::Tick_State_Pattern_2(_float fTimeDelta)
{
	_vec4 vPlayerPos = Compute_PlayerPos();
	_float fDistance = Compute_PlayerDistance();
	_vec4 vLazerLook{};
	_float fDist = 1.0f;

	View_Detect_Range_Pattern_2(m_Dir);

	switch (m_eCurState)
	{
	case Client::CGuardTower::STATE_IDLE:
		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			m_eCurState = STATE_DETECT;
			m_fIdleTime = 0.f;
		}

		break;

	case Client::CGuardTower::STATE_DETECT:


		m_fDetectTime += fTimeDelta;


		if (m_fDetectTime > 2.f && m_fDetectTime <= 4.f)
			m_Dir = DOWN;
		else if (m_fDetectTime > 4.f && m_fDetectTime <= 6.f)
			m_Dir = LEFT;
		else if (m_fDetectTime > 6.f && m_fDetectTime <= 8.f)
			m_Dir = UP;
		else if (m_fDetectTime > 8.f && m_fDetectTime <= 10.f)
		{
			m_Dir = RIGHT;
			m_fDetectTime = 0.f;
		}


		if (m_isDetected == true)
		{
			m_eCurState = STATE_ATTACK_READY;
		}

		break;
	case Client::CGuardTower::STATE_ATTACK_READY:
		//Delete_Range();

		m_fAttackTime += fTimeDelta;

		if (m_isDetected == false)
		{
			m_eCurState = STATE_IDLE;
			m_fAttackTime = 0.f;

		}
		else
		{
			if (m_fAttackTime > 2.f)
			{
				m_eCurState = STATE_ATTACK;
				m_vCurPlayerPos = Compute_PlayerPos();
				if (m_pAttackEffect->isDead() == true)
					Create_Attack_Lazer();
				m_pAttackColliderCom->Change_Extents(_vec3(0.25f, 6.f, 0.25f));
				m_fAttackTime = 0.f;
			}
		}

		break;
	case Client::CGuardTower::STATE_ATTACK:

		m_fAttackTime += fTimeDelta;

		if (m_bAttacked == false)
		{
			m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
			m_bAttacked = true;
		}

		if (m_fAttackTime > 1.f)
		{
			if (m_isDetected == false)
				m_eCurState = STATE_DETECT;
			else
			{
				m_eCurState = STATE_ATTACK_READY;
			}
			m_fAttackTime = 0.f;
		}
		break;

	case Client::CGuardTower::STATE_DIE:
		if (m_pModelCom->IsAnimationFinished(ANIM_DIE))
		{
			if (m_pModelCom->IsAnimationFinished(ANIM_DIE))
			{
				if (m_fDissolveRatio < 1.f)
				{
					m_fDissolveRatio += fTimeDelta;
					m_pShaderCom->Set_PassIndex(VTF_InstPass_Dissolve);
				}
				else
				{
					Kill();
				}
			}
		}

		break;
	}
}

void CGuardTower::View_Detect_Range_Pattern_2(EFFECT_DIR eDir)
{
	_float fRadian{};
	if (eDir == DOWN)
		fRadian = 0.f;
	if (eDir == LEFT)
		fRadian = 90.f;
	if (eDir == UP)
		fRadian = 180.f;
	if (eDir == RIGHT)
		fRadian = 270.f;

	_float fDistance = 11.5f;


	m_EffectMatrix = _mat::CreateScale(m_fEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRadian)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 0.1f, 0.f);
	m_LazerMatrix = _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 3.f, 0.f);

	_vec4 vTowerLook = m_EffectMatrix.Up().Get_Normalized();
	//_vec4 vLazerLook = Compute_Player_To_Dir(m_LazerMatrix.Position()).Get_Normalized();

	//m_LazerMatrix.Up(Compute_PlayerPos());
	Compute_Lazer_Dir();

	_float fDetectRadian = XMConvertToDegrees(acosf(vTowerLook.Dot(Compute_Player_To_Dir(m_pTransformCom->Get_State(State::Pos)))));

	PxRaycastBuffer pBuffer{};

	if (fDetectRadian < 45.f && Compute_PlayerDistance() <= fDistance)
	{
		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), Compute_Player_To_Dir(m_pTransformCom->Get_CenterPos()), 200.f, pBuffer))
		{
			if (pBuffer.block.distance > Compute_PlayerDistance())
			{
				if (CUI_Manager::Get_Instance()->Get_Hp().x > 0)
					m_isDetected = true;
				else
					m_isDetected = false;
			}
			else
				m_isDetected = false;
		}
		else
			m_isDetected = false;
	}
	else
	{
		m_isDetected = false;
	}

	if (fRadian >= 360.f)
	{
		m_bInit = true;
	}
}

void CGuardTower::Tick_State_Pattern_3(_float fTimeDelta)
{
	_vec4 vPlayerPos = Compute_PlayerPos();
	_float fDistance = Compute_PlayerDistance();
	_vec4 vLazerLook{};
	_float fDist = 1.0f;

	View_Detect_Range_Pattern_3();

	switch (m_eCurState)
	{
	case Client::CGuardTower::STATE_IDLE:
		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			m_eCurState = STATE_DETECT;
			m_fIdleTime = 0.f;
		}

		break;

	case Client::CGuardTower::STATE_DETECT:


		m_fDetectTime += fTimeDelta;
		if (m_bInit == true)
		{
			m_fDetectTime = 0.f;
			m_bInit = false;
		}

		if (m_isDetected == true)
		{
			m_eCurState = STATE_ATTACK_READY;
		}

		break;
	case Client::CGuardTower::STATE_ATTACK_READY:
		//Delete_Range();

		m_fAttackTime += fTimeDelta;

		if (m_isDetected == false)
		{
			m_eCurState = STATE_IDLE;
			m_fAttackTime = 0.f;

		}
		else
		{
			if (m_fAttackTime > 2.f)
			{
				m_eCurState = STATE_ATTACK;
				m_vCurPlayerPos = Compute_PlayerPos();
				if (m_pAttackEffect->isDead() == true)
					Create_Attack_Lazer();
				m_pAttackColliderCom->Change_Extents(_vec3(0.25f, 6.f, 0.25f));
				m_fAttackTime = 0.f;
			}
		}

		break;
	case Client::CGuardTower::STATE_ATTACK:

		m_fAttackTime += fTimeDelta;

		if (m_bAttacked == false)
		{
			m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
			m_bAttacked = true;
		}

		if (m_fAttackTime > 1.f)
		{
			if (m_isDetected == false)
				m_eCurState = STATE_DETECT;
			else
			{
				m_eCurState = STATE_ATTACK_READY;
			}
			m_fAttackTime = 0.f;
		}
		break;

	case Client::CGuardTower::STATE_DIE:
		if (m_pModelCom->IsAnimationFinished(ANIM_DIE))
		{
			m_fDeadTime += fTimeDelta;
		}


		break;
	}
}

void CGuardTower::View_Detect_Range_Pattern_3()
{
	_float fRadian{};

	m_EffectMatrix = _mat::CreateScale(m_fEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 0.1f, 0.f);
	m_LazerMatrix = _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 3.f, 0.f);

	_vec4 vTowerLook = m_EffectMatrix.Up().Get_Normalized();
	Compute_Lazer_Dir();

	PxRaycastBuffer pBuffer{};

	if (Compute_PlayerDistance() <= 7.5f)
	{
		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), Compute_Player_To_Dir(m_pTransformCom->Get_CenterPos()), 100.f, pBuffer))
		{
			if (pBuffer.block.distance > Compute_PlayerDistance())
			{
				if (CUI_Manager::Get_Instance()->Get_Hp().x > 0)
					m_isDetected = true;
				else
					m_isDetected = false;
			}
			else
				m_isDetected = false;
		}
		else
			m_isDetected = false;
	}
	else
	{
		m_isDetected = false;
	}
}

_vec4 CGuardTower::Compute_PlayerPos()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	//return pPlayerTransform->Get_State(State::Pos);
	return pPlayerTransform->Get_CenterPos();
}

_vec4 CGuardTower::Compute_PlayerLook()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Look).Get_Normalized();
}

_float CGuardTower::Compute_PlayerDistance()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	//_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();

	//_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 vPos = m_pTransformCom->Get_CenterPos();

	_float fDistance = (vPlayerPos - vPos).Length();

	return fDistance;
}

_vec4 CGuardTower::Compute_Player_To_Dir(_vec4 vPos)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	//_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();

	_vec4 vDir = (vPlayerPos - vPos).Get_Normalized();

	return vDir;
}

void CGuardTower::Compute_Lazer_Dir()
{
	_vec3 m_vLazerDir{};
	if (m_eCurState == STATE_ATTACK_READY)
		m_vLazerDir = (Compute_PlayerPos() - m_LazerMatrix.Position()).Get_Normalized();
	else if (m_eCurState == STATE_ATTACK)
		m_vLazerDir = (m_vCurPlayerPos - m_LazerMatrix.Position()).Get_Normalized();


	_vec3 vUp = m_vLazerDir;
	_vec3 vRight = vUp.Cross(_vec3(0.f, 0.f, 1.f)).Get_Normalized();
	_vec3 vLook = vRight.Cross(vUp).Get_Normalized();

	m_LazerMatrix.Right(vRight);
	m_LazerMatrix.Up(vUp);
	m_LazerMatrix.Look(vLook);

	return;
}

void CGuardTower::Create_Range_Pattern_1()
{
	if (!m_pFrameEffect)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Frame");
		Info.pMatrix = &m_EffectMatrix;
		Info.isFollow = true;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}

	if (!m_pBaseEffect)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Base");
		Info.pMatrix = &m_EffectMatrix;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}

	m_fEffectScale = 30.f;
}

void CGuardTower::Create_Range_Pattern_2()
{
	if (!m_pFrameEffect)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_90_Frame");
		Info.pMatrix = &m_EffectMatrix;
		Info.isFollow = true;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}

	if (!m_pBaseEffect)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_90_Base");
		Info.pMatrix = &m_EffectMatrix;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}
	m_fEffectScale = 30.f;
}

void CGuardTower::Create_Range_Pattern_3()
{
	if (!m_pFrameEffect)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
		Info.pMatrix = &m_EffectMatrix;
		Info.isFollow = true;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}

	if (!m_pBaseEffect)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
		Info.pMatrix = &m_EffectMatrix;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}
	m_fEffectScale = 20.f;
}

void CGuardTower::Create_Lazer()
{
	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Cannon_Laser_Warning");
	Info.pMatrix = &m_LazerMatrix;
	Info.isFollow = true;
	m_pThreatEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
}

void CGuardTower::Create_Attack_Lazer()
{
	Safe_Release(m_pAttackEffect);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Cannon_Laser");
	Info.pMatrix = &m_LazerMatrix;
	Info.isFollow = true;
	Info.fLifeTime = 0.1f;

	m_pAttackEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
}


HRESULT CGuardTower::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VTF_Instance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_TOWER, TEXT("Prototype_Model_StoneTower"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CGuardTower::Bind_ShaderResources()
{
	if (true == m_pGameInstance->Get_TurnOnShadow()) {

		CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
			return E_FAIL;

	}

	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
	{
		return E_FAIL;
	}

	_vector vRimColor = { 1.f, 0.f, 0.f, 1.f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vRimColor, sizeof _vector)))
	{
		return E_FAIL;
	}

	_uint iOutlineColor = OutlineColor_Red;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_OutlineColor", &iOutlineColor, sizeof _uint)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGuardTower::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(1.f, 2.5f, 1.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider", L"Com_Collider_Body_OBB", (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CGuardTower::Add_Attack_Collider()
{
	Collider_Desc AttackCollDesc = {};
	AttackCollDesc.eType = ColliderType::OBB;
	AttackCollDesc.vExtents = _vec3(0.25f, 6.f, 0.25f);
	AttackCollDesc.vCenter = _vec3(0.f, AttackCollDesc.vExtents.y, 0.f);
	AttackCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider", L"Com_Collider_Attack_OBB", (CComponent**)&m_pAttackColliderCom, &AttackCollDesc)))
		return E_FAIL;
	return S_OK;
}

void CGuardTower::Update_Collider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pAttackColliderCom->Update(m_LazerMatrix);
}

CGuardTower* CGuardTower::Create(_dev pDevice, _context pContext)
{
	CGuardTower* pInstance = new CGuardTower(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGuardTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}
HRESULT CGuardTower::Render_Instance()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	return S_OK;
}
CGameObject* CGuardTower::Clone(void* pArg)
{
	CGuardTower* pInstance = new CGuardTower(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGuardTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGuardTower::Free()
{
	if (false == m_isPrototype) {
		CUI_Manager::Get_Instance()->Delete_RadarPos(CUI_Manager::MONSTER, m_pTransformCom);
		m_pGameInstance->Delete_Light(LEVEL_TOWER, m_strLightTag);
	}
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pThreatEffect);
	Safe_Release(m_pAttackEffect);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);
	Safe_Release(m_pDissolveTextureCom);
}
