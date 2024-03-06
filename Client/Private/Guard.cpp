#include "Guard.h"
#include "UI_Manager.h"
#include "Effect_Dummy.h"

_int CGuard::m_iLightID = {};

CGuard::CGuard(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CGuard::CGuard(const CGuard& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGuard::Init_Prototype()
{
	return S_OK;
}

HRESULT CGuard::Init(void* pArg)
{


	m_Info = *(GuardInfo*)pArg;
	m_OriginMatrix = m_Info.mMatrix * _mat::CreateTranslation(0.f, 1.f, 0.f);
	m_EffectMatrix = m_Info.mMatrix;

	m_ePattern = (GUARD_PATTERN)m_Info.iIndex;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Collider()))
		return E_FAIL;


	m_eCurState = STATE_IDLE;

	m_iHP = 1;

	m_pTransformCom->Set_Matrix(m_Info.mMatrix);

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);
	m_pShaderCom->Set_PassIndex(VTF_InstPass_Default);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.2f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.4f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	CUI_Manager::Get_Instance()->Set_RadarPos(CUI_Manager::MONSTER, m_pTransformCom);

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	Create_Range();


	LIGHT_DESC LightDesc{};
	LightDesc.eType = LIGHT_DESC::TYPE::Point;
	LightDesc.vSpecular = _vec4(1.f);
	LightDesc.vDiffuse = _vec4(0.25f);
	LightDesc.vAmbient = _vec4(0.05f);
	LightDesc.vPosition = m_pTransformCom->Get_CenterPos();
	LightDesc.vAttenuation = LIGHT_RANGE_32;

	m_strLightTag = L"Light_Guard_" + to_wstring(m_iLightID++);
	if (FAILED(m_pGameInstance->Add_Light(LEVEL_TOWER, m_strLightTag, LightDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CGuard::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

	if (m_bAttacked == true)
	{
		m_fAttackDelay += fTimeDelta;
		if (m_fAttackDelay >= 2.f)
		{
			m_bAttacked = false;
			m_fAttackDelay = 0;
		}
	}

	if (m_eCurState == STATE_IDLE || m_eCurState == STATE_PATROL || m_eCurState == STATE_TURN)
	{
		m_pBaseEffect->Tick(fTimeDelta);
		m_pFrameEffect->Tick(fTimeDelta);
	}

	if (true == m_bChangePass) {
		m_fHitTime += fTimeDelta;

		if (0.3f <= m_fHitTime) {
			m_fHitTime = 0.f;
			m_bChangePass = false;
		}
	}

	Init_State(fTimeDelta);
	if(m_ePattern == PATTERN_1)
		Tick_State_Pattern1(fTimeDelta);
	else if(m_ePattern == PATTERN_2)
		Tick_State_Pattern2(fTimeDelta);
	else
		Tick_State_Pattern3(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	m_pTransformCom->Gravity(fTimeDelta);
	Update_Collider();

	LIGHT_DESC* Desc = m_pGameInstance->Get_LightDesc(LEVEL_TOWER, m_strLightTag);
	Desc->vPosition = m_pTransformCom->Get_CenterPos();


}

void CGuard::Late_Tick(_float fTimeDelta)
{
		m_pModelCom->Play_Animation(fTimeDelta);
	if (m_eCurState == STATE_IDLE || m_eCurState == STATE_PATROL || m_eCurState == STATE_TURN)
	{
		m_pBaseEffect->Late_Tick(fTimeDelta);
		m_pFrameEffect->Late_Tick(fTimeDelta);
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

HRESULT CGuard::Render()
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

	//	_bool HasGlowTex{};
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_GlowTexture", i, TextureType::Specular)))
	//	{
	//		HasGlowTex = false;
	//	}
	//	else
	//	{
	//		HasGlowTex = true;
	//	}

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
	//	{
	//		return E_FAIL;
	//	}

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
	//	{
	//		return E_FAIL;
	//	}

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_HasGlowTex", &HasGlowTex, sizeof _bool)))
	//	{
	//		return E_FAIL;
	//	}

	//	//if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
	//	//	return E_FAIL;



	//	if (FAILED(m_pModelCom->Render(i)))
	//		return E_FAIL;
	//}
	//if (!m_bChangePass && m_iHP > 0)
	//{
	//	m_iPassIndex = AnimPass_Default;
	//}
	return S_OK;
}

void CGuard::Set_Damage(_int iDamage, _uint iDamageType)
{

	if (m_isDetected == false)
	{
		if (iDamageType == AT_Sword_Common)
		{
			m_bChangePass = true;
			m_iHP -= iDamage;
		}
		CUI_Manager::Get_Instance()->Set_HitEffect(m_pTransformCom, iDamage, _vec2(0.f, 2.f), (ATTACK_TYPE)iDamageType);
	}
}


void CGuard::Init_State(_float fTimeDelta)
{
	if (m_iHP <= 0 || true == m_isDead)
	{
		m_eCurState = STATE_DIE;
	}

	if (m_ePreState != m_eCurState) {
		switch (m_eCurState)
		{
		case Client::CGuard::STATE_IDLE:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			break;
		case Client::CGuard::STATE_PATROL:
			m_Animation.iAnimIndex = ANIM_WALK;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			m_pTransformCom->Set_Speed(1.f);
			break;
		case Client::CGuard::STATE_TURN:
			m_Animation.iAnimIndex = ANIM_WALK;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			m_pTransformCom->Set_Speed(1.f);
			break;
		case Client::CGuard::STATE_CHASE:
			m_Animation.iAnimIndex = ANIM_RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			break;
		case Client::CGuard::STATE_ATTACK_SWING:
			m_Animation.iAnimIndex = ANIM_SWING;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		case Client::CGuard::STATE_ATTACK_STEP:
			m_Animation.iAnimIndex = ANIM_STEP;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		case Client::CGuard::STATE_BACK:
			m_Animation.iAnimIndex = ANIM_RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		case Client::CGuard::STATE_DIE:
			m_Animation.iAnimIndex = ANIM_DIE;
			m_Animation.isLoop = false;
			m_pTransformCom->Delete_Controller();
			m_pGameInstance->Delete_CollisionObject(this);
			break;
		}

		m_ePreState = m_eCurState;
	}

}

void CGuard::Tick_State_Pattern1(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
	_vec4 vTargetPos = pPlayerTransform->Get_State(State::Pos);
	_vec4 vPos = m_pTransformCom->Get_CenterPos();
	_vec4 vToPlayer = vPlayerPos - vPos;
	_vec3 vNormalToPlayer = vToPlayer.Get_Normalized();
	_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_float fDist = vToPlayer.Length();
	_float fAngle = acosf(vLook.Dot(vNormalToPlayer));
	_float fDistance{ 10.f };
	_vec4 vMyPosition = m_pTransformCom->Get_State(State::Pos);

	Detect_Range(fAngle, fDist, vNormalToPlayer);
	PxRaycastBuffer pBuffer{};
	_bool isCollision{ false };
	_bool isBack{ false };

	_randInt iAttackInt(0, 1);
	_int iRandomAttack = iAttackInt(m_iRandomAttack);

	_float Degree{};

	if(m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), m_pTransformCom->Get_State(State::Look), 2.f, pBuffer))
	{
		m_eCurState = STATE_BACK;
	}
	switch (m_eCurState)
	{

	case STATE_IDLE:
		m_pTransformCom->Set_Speed(1.f);
		vIdlePos = m_pTransformCom->Get_State(State::Pos);
		m_fIdleTime += fTimeDelta;
		if (m_isDetected == true)
			m_eCurState = STATE_CHASE;

		if (m_isArrived == true)
		{
			if (m_fIdleTime > 1.f)
			{
				m_vTurnAngle = -m_pTransformCom->Get_State(State::Look);
				m_eCurState = STATE_TURN;
				m_isArrived = false;
				m_fIdleTime = 0.f;
			}
		}
		if (2.f < m_fIdleTime)
		{
			m_eCurState = STATE_PATROL;
			m_fIdleTime = 0.f;
		}
		
		break;

	case STATE_PATROL:
		m_fPatrolTime += fTimeDelta;
		vPatrolPos = m_pTransformCom->Get_State(State::Pos);
		if (m_isDetected == true)
			m_eCurState = STATE_CHASE;
		if((vIdlePos - vPatrolPos).Length() > 10.f)
		{
			m_eCurState = STATE_IDLE;
			m_isArrived = true;
			
		} 
		else
			m_pTransformCom->Go_Straight(fTimeDelta);
		


		break;
	case STATE_TURN:
		
		m_fTurnTime += fTimeDelta;
		m_pTransformCom->Turn(_vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		if (m_isDetected == true)
			m_eCurState = STATE_CHASE;

		Degree = XMConvertToDegrees(acosf(m_vTurnAngle.Dot(vLook)));
		if (3.f >= Degree) {
			m_vTurnAngle.y = 0.f;
			m_pTransformCom->LookAt_Dir(m_vTurnAngle);
			m_eCurState = STATE_IDLE;
		}
		//if (m_fTurnTime > 2.f)
		//{
		//	m_eCurState = STATE_IDLE;
		//	m_fTurnTime = 0;
		//}

		break;
	case STATE_CHASE:
		m_pTransformCom->Set_Speed(7.f);
		m_fAttackTime += fTimeDelta;

		if(fDist < 2.f)
		{
			if (m_fAttackTime > 1.f)
			{
				if (iRandomAttack == 0)
					m_eCurState = STATE_ATTACK_SWING;
				else
					m_eCurState = STATE_ATTACK_STEP;

				m_fAttackTime = 0.f;
			}
			vNormalToPlayer.y = 0.f;
			m_pTransformCom->LookAt_Dir(vNormalToPlayer);

		}
		else
		{
			vNormalToPlayer.y = 0.f;
			m_pTransformCom->LookAt_Dir(vNormalToPlayer);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (fDist > 10.f)
			m_eCurState = STATE_BACK;

		break;
	case STATE_ATTACK_STEP:
			if (m_bAttacked == false)
			{
				if (m_pModelCom->Get_CurrentAnimPos() > 39.f && m_pModelCom->Get_CurrentAnimPos() < 42.f)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
					m_bAttacked = true;
				}
			}

			if (m_pModelCom->IsAnimationFinished(ANIM_STEP))
			{
				m_eCurState = STATE_CHASE;
				m_fAttackTime = 0.f;
			}
			
		break;

	case STATE_ATTACK_SWING:

		if (m_bAttacked == false)
		{
			if (m_pModelCom->Get_CurrentAnimPos() > 39.f && m_pModelCom->Get_CurrentAnimPos() < 42.f )
			{
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
				m_bAttacked = true;
			}
		}

		if (m_pModelCom->IsAnimationFinished(ANIM_SWING))
		{
			m_eCurState = STATE_CHASE;
			m_fAttackTime = 0.f;

		}
		

		break;

	case STATE_BACK:
		m_pTransformCom->Set_Speed(5.f);

		m_pTransformCom->LookAt(m_OriginMatrix.Position());

		isBack = m_pTransformCom->Go_To(m_OriginMatrix.Position(), fTimeDelta * 2.f);
		
		if (isBack)
		{
			m_pTransformCom->Set_Matrix(m_OriginMatrix);
			//m_pTransformCom->Set_State(State::Look, m_OriginMatrix.Look());
			m_eCurState = STATE_IDLE;
		}
		

		break;

	case STATE_DIE:

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

		break;
	}
}


void CGuard::Tick_State_Pattern2(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
	_vec4 vTargetPos = pPlayerTransform->Get_State(State::Pos);
	_vec4 vPos = m_pTransformCom->Get_CenterPos();
	_vec4 vToPlayer = vPlayerPos - vPos;
	_vec3 vNormalToPlayer = vToPlayer.Get_Normalized();
	_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_float fDist = vToPlayer.Length();
	_float fAngle = acosf(vLook.Dot(vNormalToPlayer));
	_float fDistance{ 10.f };
	_vec4 vMyPosition = m_pTransformCom->Get_State(State::Pos);

	Detect_Range(fAngle, fDist, vNormalToPlayer);
	PxRaycastBuffer pBuffer{};
	_bool isCollision{ false };
	_bool isBack{ false };

	_randInt iAttackInt(0, 1);
	_int iRandomAttack = iAttackInt(m_iRandomAttack);

	_float Degree{};
	switch (m_eCurState)
	{

	case STATE_IDLE:
		m_pTransformCom->Set_Speed(1.f);
		vIdlePos = m_pTransformCom->Get_State(State::Pos);
		m_fIdleTime += fTimeDelta;
		if (m_isDetected == true)
			m_eCurState = STATE_CHASE;

		if (m_isArrived == true)
		{
			if (m_fIdleTime > 1.f)
			{
				m_eCurState = STATE_TURN;
				m_isArrived = false;
				m_fIdleTime = 0.f;
			}
		}
		if (2.f < m_fIdleTime)
		{
			m_eCurState = STATE_PATROL;
			m_fIdleTime = 0.f;
		}

		break;

	case STATE_PATROL:
		m_fPatrolTime += fTimeDelta;
		vPatrolPos = m_pTransformCom->Get_State(State::Pos);
		if (m_isDetected == true)
			m_eCurState = STATE_CHASE;

		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), m_pTransformCom->Get_State(State::Look), 5.f, pBuffer))
		{
			m_eCurState = STATE_TURN;
			m_vTurnAngle = m_pTransformCom->Get_State(State::Right);
		}
		
		else
			m_pTransformCom->Go_Straight(fTimeDelta);



		break;
	case STATE_TURN:

		m_fTurnTime += fTimeDelta;
		m_pTransformCom->Turn(_vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		if (m_isDetected == true)
			m_eCurState = STATE_CHASE;

		Degree = XMConvertToDegrees(acosf(m_vTurnAngle.Dot(vLook)));
		if (3.f >= Degree) {
			m_vTurnAngle.y = 0.f;
			m_pTransformCom->LookAt_Dir(m_vTurnAngle);
			m_eCurState = STATE_IDLE;
		}

		//if (m_fTurnTime > 1.f)
		//{
		//	m_eCurState = STATE_IDLE;
		//	m_fTurnTime = 0;
		//}

		break;
	case STATE_CHASE:
		m_pTransformCom->Set_Speed(7.f);
		m_fAttackTime += fTimeDelta;

		if (fDist < 2.f)
		{
			if (m_fAttackTime > 1.f)
			{
				if (iRandomAttack == 0)
					m_eCurState = STATE_ATTACK_SWING;
				else
					m_eCurState = STATE_ATTACK_STEP;

				m_fAttackTime = 0.f;
			}
			vNormalToPlayer.y = 0.f;
			m_pTransformCom->LookAt_Dir(vNormalToPlayer);

		}
		else
		{
			vNormalToPlayer.y = 0.f;
			m_pTransformCom->LookAt_Dir(vNormalToPlayer);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (fDist > 10.f)
			m_eCurState = STATE_BACK;

		break;
	case STATE_ATTACK_STEP:
		if (m_bAttacked == false)
		{
			if (m_pModelCom->Get_CurrentAnimPos() > 39.f && m_pModelCom->Get_CurrentAnimPos() < 42.f)
			{
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
				m_bAttacked = true;
			}
		}

		if (m_pModelCom->IsAnimationFinished(ANIM_STEP))
		{
			m_eCurState = STATE_CHASE;
			m_fAttackTime = 0.f;
		}

		break;

	case STATE_ATTACK_SWING:

		if (m_bAttacked == false)
		{
			if (m_pModelCom->Get_CurrentAnimPos() > 39.f && m_pModelCom->Get_CurrentAnimPos() < 42.f)
			{
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
				m_bAttacked = true;
			}
		}

		if (m_pModelCom->IsAnimationFinished(ANIM_SWING))
		{
			m_eCurState = STATE_CHASE;
			m_fAttackTime = 0.f;

		}


		break;

	case STATE_BACK:
		m_pTransformCom->Set_Speed(5.f);

		m_pTransformCom->LookAt(m_OriginMatrix.Position());

		isBack = m_pTransformCom->Go_To(m_OriginMatrix.Position(), fTimeDelta * 2.f);

		if (isBack)
		{
			m_pTransformCom->Set_Matrix(m_OriginMatrix);
			//m_pTransformCom->Set_State(State::Look, m_OriginMatrix.Look());
			m_eCurState = STATE_IDLE;
		}


		break;

	case STATE_DIE:
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

		break;
	}
}


void CGuard::Tick_State_Pattern3(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
	_vec4 vTargetPos = pPlayerTransform->Get_State(State::Pos);
	_vec4 vPos = m_pTransformCom->Get_CenterPos();
	_vec4 vToPlayer = vPlayerPos - vPos;
	_vec3 vNormalToPlayer = vToPlayer.Get_Normalized();
	_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_float fDist = vToPlayer.Length();
	_float fAngle = acosf(vLook.Dot(vNormalToPlayer));
	_float fDistance{ 10.f };
	_vec4 vMyPosition = m_pTransformCom->Get_State(State::Pos);

	Detect_Range(fAngle, fDist, vNormalToPlayer);
	PxRaycastBuffer pBuffer{};
	_bool isCollision{ false };
	_bool isBack{ false };

	_randInt iAttackInt(0, 1);
	_int iRandomAttack = iAttackInt(m_iRandomAttack);

	if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), m_pTransformCom->Get_State(State::Look), 2.f, pBuffer))
	{
		m_eCurState = STATE_BACK;
	}
	switch (m_eCurState)
	{

	case STATE_IDLE:
		m_pTransformCom->Set_Speed(1.f);
		vIdlePos = m_pTransformCom->Get_State(State::Pos);
		m_fIdleTime += fTimeDelta;
		if (m_isDetected == true)
			m_eCurState = STATE_CHASE;

		break;

	case STATE_CHASE:
		m_pTransformCom->Set_Speed(5.f);
		m_fAttackTime += fTimeDelta;

		if (fDist < 2.f)
		{
			if (m_fAttackTime > 1.f)
			{
				if (iRandomAttack == 0)
					m_eCurState = STATE_ATTACK_SWING;
				else
					m_eCurState = STATE_ATTACK_STEP;

				m_fAttackTime = 0.f;
			}
			vNormalToPlayer.y = 0.f;
			m_pTransformCom->LookAt_Dir(vNormalToPlayer);

		}
		else
		{
			vNormalToPlayer.y = 0.f;
			m_pTransformCom->LookAt_Dir(vNormalToPlayer);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
		if (fDist > 10.f)
			m_eCurState = STATE_BACK;

		break;

	case STATE_ATTACK_STEP:
		if (m_bAttacked == false)
		{
			if (m_pModelCom->Get_CurrentAnimPos() > 39.f && m_pModelCom->Get_CurrentAnimPos() < 42.f)
			{
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
				m_bAttacked = true;
			}
		}

		if (m_pModelCom->IsAnimationFinished(ANIM_STEP))
		{
			m_eCurState = STATE_CHASE;
			m_fAttackTime = 0.f;
		}

		break;

	case STATE_ATTACK_SWING:

		if (m_bAttacked == false)
		{
			if (m_pModelCom->Get_CurrentAnimPos() > 39.f && m_pModelCom->Get_CurrentAnimPos() < 42.f)
			{
				m_pGameInstance->Attack_Player(m_pAttackColliderCom, 999);
				m_bAttacked = true;
			}
		}

		if (m_pModelCom->IsAnimationFinished(ANIM_SWING))
		{
			m_eCurState = STATE_CHASE;
			m_fAttackTime = 0.f;

		}


		break;

	case STATE_BACK:
		m_pTransformCom->Set_Speed(5.f);

		m_pTransformCom->LookAt(m_OriginMatrix.Position());

		isBack = m_pTransformCom->Go_To(m_OriginMatrix.Position(), fTimeDelta * 2.f);

		if (isBack)
		{
			m_pTransformCom->Set_Matrix(m_OriginMatrix);
			//m_pTransformCom->Set_State(State::Look, m_OriginMatrix.Look());
			m_eCurState = STATE_IDLE;
		}


		break;

	case STATE_DIE:
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

		break;
	}
}
_vec4 CGuard::Compute_PlayerPos()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_CenterPos();
}

_vec4 CGuard::Compute_PlayerLook()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Look).Get_Normalized();
}

_float CGuard::Compute_PlayerDistance()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_float fDistance = (vPlayerPos - vPos).Length();

	return fDistance;
}

void CGuard::Create_Range()
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
}

void CGuard::Detect_Range(_float fAngle, _float fDist, _vec4 vNormalToPlayer)
{
	PxRaycastBuffer Buffer{};
	m_EffectMatrix = _mat::CreateScale(20.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 0.1f, 0.f);

	if (XMConvertToRadians(45.f) >= fAngle && 8.f >= fDist)
	{
		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), vNormalToPlayer, 100.f, Buffer))
		{
			if (fDist < Buffer.block.distance)
			{
				if (CUI_Manager::Get_Instance()->Get_Hp().x > 0)
					m_isDetected = true;
				else
				{
					if (m_pModelCom->IsAnimationFinished(ANIM_SWING) || m_pModelCom->IsAnimationFinished(ANIM_STEP))
					{
						m_eCurState = STATE_BACK;
						m_isDetected = false;
					}
				}
			}
			else
			{
				m_isDetected = false;
			}
		}
		else
		{
			m_isDetected = false;
		}
	}
	else
	{
		m_isDetected = false;
	}
}

HRESULT CGuard::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_TOWER, TEXT("Prototype_Model_Guard"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CGuard::Bind_ShaderResources()
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

HRESULT CGuard::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.3f, 1.f, 0.2f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider", L"Com_Collider_OBB", (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc AttackColDesc{};
	AttackColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(m_pTransformCom->Get_State(State::Pos), m_pTransformCom->Get_State(State::Look), _vec3(0.f,1.f,0.f));
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 0.1f, 2.f);

	AttackColDesc.matFrustum = matView * matProj;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &AttackColDesc)))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CGuard::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 1.f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	
}

HRESULT CGuard::Render_Instance()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	return S_OK;
}

CGuard* CGuard::Create(_dev pDevice, _context pContext)
{
	CGuard* pInstance = new CGuard(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGuard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGuard::Clone(void* pArg)
{
	CGuard* pInstance = new CGuard(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGuard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGuard::Free()
{
	m_pGameInstance->Delete_Light(LEVEL_TOWER, m_strLightTag);
	CUI_Manager::Get_Instance()->Delete_RadarPos(CUI_Manager::MONSTER, m_pTransformCom);
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);
	Safe_Release(m_pDissolveTextureCom);
}
