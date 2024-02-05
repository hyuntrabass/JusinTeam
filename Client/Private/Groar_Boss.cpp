#include "Groar_Boss.h"

#include "Missile.h"

const _float CGroar_Boss::m_fChaseRange = 10.f;
const _float CGroar_Boss::m_fAttackRange = 6.f;

CGroar_Boss::CGroar_Boss(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CGroar_Boss::CGroar_Boss(const CGroar_Boss& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGroar_Boss::Init_Prototype()
{
	return S_OK;
}

HRESULT CGroar_Boss::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(2173.f, -20.f, 2095.f, 1.f));
	m_pTransformCom->Set_Position(_vec3(2173.f, -20.f, 2095.f));
	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);
	m_eCurState = STATE_NPC;

	m_Animation.iAnimIndex = NPC_IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_iHP = 100000;

	return S_OK;
}

void CGroar_Boss::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_G))
	{
		//m_eCurState = STATE_SCENE01;
		m_eCurState = STATE_BOSS;
		m_eBossCurState = BOSS_STATE_ROAR;
	}

	if (m_pGameInstance->Key_Down(DIK_C))
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Tentacle"), TEXT("Prototype_GameObject_Tentacle"))))
		{

		}
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	Update_Collider();

	m_pTransformCom->Gravity(fTimeDelta);
}

void CGroar_Boss::Late_Tick(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CGroar_Boss::STATE_NPC:
		m_pNPCModelCom->Play_Animation(fTimeDelta);
		break;
	case Client::CGroar_Boss::STATE_SCENE01:
		m_pScene01ModelCom->Play_Animation(fTimeDelta);
		break;
	case Client::CGroar_Boss::STATE_SCENE02:
		m_pScene02ModelCom->Play_Animation(fTimeDelta);
		break;
	case Client::CGroar_Boss::STATE_BOSS:
		m_pBossModelCom->Play_Animation(fTimeDelta);
		break;
	}

	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif

}

HRESULT CGroar_Boss::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	CModel* pModel = { nullptr };

	switch (m_eCurState)
	{
	case Client::CGroar_Boss::STATE_NPC:
		pModel = m_pNPCModelCom;
		break;
	case Client::CGroar_Boss::STATE_SCENE01:
		pModel = m_pScene01ModelCom;
		break;
	case Client::CGroar_Boss::STATE_SCENE02:
		pModel = m_pScene02ModelCom;
		break;
	case Client::CGroar_Boss::STATE_BOSS:
		pModel = m_pBossModelCom;
		break;
	}

	for (_uint i = 0; i < pModel->Get_NumMeshes(); i++)
	{
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		_bool HasNorTex{};
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		_bool HasMaskTex{};
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(pModel->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
		{
			return E_FAIL;
		}

		if (FAILED(pModel->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CGroar_Boss::Init_State(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CGroar_Boss::STATE_NPC:
			break;

		case Client::CGroar_Boss::STATE_SCENE01:
			m_Animation.iAnimIndex = 0;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 1.5f;

			break;

		case Client::CGroar_Boss::STATE_SCENE02:
			m_Animation.iAnimIndex = 0;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::STATE_BOSS:
			break;
		}

		m_ePreState = m_eCurState;
	}

	if (m_iHP <= 0)
	{
		m_eBossCurState = BOSS_STATE_DIE;
	}

	if (m_eBossPreState != m_eBossCurState)
	{
		switch (m_eBossCurState)
		{
		case Client::CGroar_Boss::BOSS_STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;

			break;

		case Client::CGroar_Boss::BOSS_STATE_ROAR:
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK_RAGE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->LookAt_Dir(vDir);
			break;

		case Client::CGroar_Boss::BOSS_STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_bSelectAttackPattern = false;

			m_bCreateMissile = false;
			m_bCreateSpider = false;

			m_bAttacked1 = false;
			m_bAttacked2 = false;
			m_bAttacked3 = false;

			break;

		case Client::CGroar_Boss::BOSS_STATE_THROW_ATTACK: // 00, 01

			if (m_iThrowAttackCombo % 2 == 0)
			{
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK00;
			}
			else
			{
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK01;
			}

			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;

		case Client::CGroar_Boss::BOSS_STATE_SIX_MISSILE: // 02
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK02;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_TAKE_DOWN: // 03
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK03;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;

		case Client::CGroar_Boss::BOSS_STATE_SPIDER: // 05
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK05;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_TENTACLE: // 06
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK06;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 1.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_XBEAM: // 07
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK07;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_RAGE: // 08
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK08;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_DIE:
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_DIE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;
		}

		m_eBossPreState = m_eBossCurState;
	}
}

void CGroar_Boss::Tick_State(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	_float fDistance = (vPlayerPos - vPos).Length();

	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	switch (m_eCurState)
	{
	case Client::CGroar_Boss::STATE_NPC:
		m_pNPCModelCom->Set_Animation(m_Animation);
		break;

	case Client::CGroar_Boss::STATE_SCENE01:
		if (m_pScene01ModelCom->IsAnimationFinished(0))
		{
			m_eCurState = STATE_SCENE02;
		}

		m_pScene01ModelCom->Set_Animation(m_Animation);
		break;

	case Client::CGroar_Boss::STATE_SCENE02:
		if (m_pScene02ModelCom->IsAnimationFinished(0))
		{
			m_eCurState = STATE_BOSS;
			m_eBossCurState = BOSS_STATE_ROAR;

			m_Animation.iAnimIndex = 0;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
		}
		else
		{
			m_Animation.fAnimSpeedRatio = 4.5f;
		}

		m_pScene02ModelCom->Set_Animation(m_Animation);
		break;

	case Client::CGroar_Boss::STATE_BOSS:
		m_pBossModelCom->Set_Animation(m_Animation);
		break;
	}

	switch (m_eBossCurState)
	{
	case Client::CGroar_Boss::BOSS_STATE_IDLE:
		break;

	case Client::CGroar_Boss::BOSS_STATE_ROAR:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 95.f && m_pBossModelCom->Get_CurrentAnimPos() <= 175.f)
		{
			m_pGameInstance->Set_ShakeCam(true, 0.5f);
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK_RAGE))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_CHASE:
	{
		GROAR_BOSS_STATE eTempBossCurState = { BOSS_STATE_END };

		if (!m_bSelectAttackPattern)
		{
			_bool bReset = { true };
			for (size_t i = 0; i < ATTACK_END; i++)
			{
				if (m_bAttack_Selected[i] == false)
				{
					bReset = false;
					break;
				}
			}

			if (bReset == true)
			{
				for (auto& it : m_bAttack_Selected)
				{
					it = false;
				}
			}

			GROAR_ATTACK eAttackRandom = ATTACK_END;
			eAttackRandom = static_cast<GROAR_ATTACK>(rand() % ATTACK_END);

			// 랜덤 방지용
			while (m_bAttack_Selected[eAttackRandom] == true)
			{
				eAttackRandom = static_cast<GROAR_ATTACK>(rand() % ATTACK_END);
			}

			switch (eAttackRandom)
			{
			case Client::CGroar_Boss::ATTACK_THROW:
				eTempBossCurState = BOSS_STATE_THROW_ATTACK;
				break;
			case Client::CGroar_Boss::ATTACK_SIX_MISSILE:
				eTempBossCurState = BOSS_STATE_SIX_MISSILE;
				break;
			case Client::CGroar_Boss::ATTACK_TAKE_DOWN:
				eTempBossCurState = BOSS_STATE_TAKE_DOWN;
				break;
			case Client::CGroar_Boss::ATTACK_SPIDER:
				eTempBossCurState = BOSS_STATE_SPIDER;
				break;
			case Client::CGroar_Boss::ATTACK_TENTACLE:
				eTempBossCurState = BOSS_STATE_TENTACLE;
				break;
			case Client::CGroar_Boss::ATTACK_XBEAM:
				eTempBossCurState = BOSS_STATE_XBEAM;
				break;
			case Client::CGroar_Boss::ATTACK_YELL:
				eTempBossCurState = BOSS_STATE_RAGE;
				break;
			}

			if (m_iThrowAttackCombo > 0)
			{
				eTempBossCurState = BOSS_STATE_THROW_ATTACK;
			}

			m_bSelectAttackPattern = true;
		}

		if (eTempBossCurState == BOSS_STATE_THROW_ATTACK || eTempBossCurState == BOSS_STATE_SIX_MISSILE || eTempBossCurState == BOSS_STATE_XBEAM)
		{
			if (fDistance >= m_fChaseRange)
			{
				m_pTransformCom->LookAt_Dir(vDir);
				m_pTransformCom->Go_Straight(fTimeDelta);
				m_bSelectAttackPattern = false;
			}
			else
			{
				m_eBossCurState = eTempBossCurState;
			}

		}
		else
		{
			if (fDistance >= m_fAttackRange)
			{
				m_pTransformCom->LookAt_Dir(vDir);
				m_pTransformCom->Go_Straight(fTimeDelta);
				m_bSelectAttackPattern = false;
			}
			else
			{
				m_eBossCurState = eTempBossCurState;
			}
		}

		//m_eBossCurState = BOSS_STATE_SPIDER; // 테스트용
	}

	break;

	case Client::CGroar_Boss::BOSS_STATE_THROW_ATTACK:

		if (m_pBossModelCom->Get_CurrentAnimationIndex() == MON_GROAR_ASGARD_ATTACK00)
		{
			if (m_pBossModelCom->Get_CurrentAnimPos() >= 10.f && !m_bCreateMissile)
			{
				CMissile::MISSILE_TYPE eType = CMissile::RIGHT_THROW;
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
				m_bCreateMissile = true;

				++m_iThrowAttackCombo;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() < 10.f)
			{
				m_bCreateMissile = false;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() <= 38.f)
			{
				m_pTransformCom->LookAt_Dir(vDir);
			}

		}

		else if (m_pBossModelCom->Get_CurrentAnimationIndex() == MON_GROAR_ASGARD_ATTACK01)
		{
			if (m_pBossModelCom->Get_CurrentAnimPos() >= 15.f && !m_bCreateMissile)
			{
				CMissile::MISSILE_TYPE eType = CMissile::LEFT_THROW;
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
				m_bCreateMissile = true;

				++m_iThrowAttackCombo;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() < 15.f)
			{
				m_bCreateMissile = false;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() <= 51.f)
			{
				m_pTransformCom->LookAt_Dir(vDir);
			}

		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK00) || m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK01))
		{
			if (m_iThrowAttackCombo >= 3)
			{
				m_iThrowAttackCombo = 0;
				m_bAttack_Selected[ATTACK_THROW] = true;
			}

			m_eBossCurState = BOSS_STATE_CHASE;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_SIX_MISSILE:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 60.f && !m_bCreateMissile)
		{
			for (size_t i = 0; i < 6; i++)
			{
				CMissile::MISSILE_TYPE eType = CMissile::SIX_MISSILE;
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
			}

			m_bCreateMissile = true;
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() <= 100.f)
		{
			m_pTransformCom->LookAt_Dir(vDir);
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 99.f && m_pBossModelCom->Get_CurrentAnimPos() <= 110.f)
		{
			m_pGameInstance->Set_ShakeCam(true);
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK02))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_SIX_MISSILE] = true;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_TAKE_DOWN:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 69.f && m_pBossModelCom->Get_CurrentAnimPos() <= 72.f)
		{
			m_pGameInstance->Set_ShakeCam(true);

			if (fDistance <= 7.5f)
			{
				if (!m_bAttacked1)
				{
					_uint iDamage = 50 + rand() % 20;
					m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_Hit);
					m_bAttacked1 = true;
				}
			}
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 97.f && m_pBossModelCom->Get_CurrentAnimPos() <= 100.f)
		{
			m_pGameInstance->Set_ShakeCam(true);

			if (fDistance <= 7.5f)
			{
				if (!m_bAttacked2)
				{
					_uint iDamage = 50 + rand() % 20;
					m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_Hit);
					m_bAttacked2 = true;
				}
			}
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 135.f && m_pBossModelCom->Get_CurrentAnimPos() <= 138.f)
		{
			m_pGameInstance->Set_ShakeCam(true);

			if (fDistance <= 7.5f)
			{
				if (!m_bAttacked3)
				{
					_uint iDamage = 80 + rand() % 20;
					m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_Stun);
					m_bAttacked3 = true;
				}
			}
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK03))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_TAKE_DOWN] = true;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_SPIDER:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 80.f && !m_bCreateSpider)
		{
			for (size_t i = 0; i < 3; i++)
			{
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Spider"), TEXT("Prototype_GameObject_Spider"));
			}

			m_bCreateSpider = true;
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK05))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_SPIDER] = true;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_TENTACLE:
		if (m_pBossModelCom->Get_CurrentAnimPos() >= 48.f)
		{
			m_fTentacleTime += fTimeDelta;
		}

		if (m_fTentacleTime >= 0.7f)
		{
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Tentacle"), TEXT("Prototype_GameObject_Tentacle"));
			m_fTentacleTime = 0.f;
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK06))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_TENTACLE] = true;
			m_fTentacleTime = 0.f;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_XBEAM:

		if (m_pBossModelCom->Get_CurrentAnimPos() < 102.f)
		{
			m_pTransformCom->LookAt_Dir(vDir);
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 102.f && !m_bCreateXBeam)
		{
			_vec3 vPos = m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 3.f, 0.f);
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_XBeam"), TEXT("Prototype_GameObject_XBeam"), &vPos);

			m_bCreateXBeam = true;
		}

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 108.f && m_pBossModelCom->Get_CurrentAnimPos() <= 115.f)
		{
			m_pGameInstance->Set_ShakeCam(true);
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK07))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_XBEAM] = true;
			m_bCreateXBeam = false;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_RAGE:
		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK08))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
			m_bAttack_Selected[ATTACK_YELL] = true;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_DIE:
		break;
	}
}

HRESULT CGroar_Boss::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(2.f, 2.f, 2.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
	{
		return E_FAIL;
	}

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	// 1인자 : 절두체 각도(범위), 2인자 : Aspect, 3인자 : Near, 4인자 : Far(절두체 깊이)
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 0.01f, 3.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.8f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.6f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	return S_OK;
}

void CGroar_Boss::Update_Collider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	_mat Offset = _mat::CreateTranslation(0.f, 2.f, 1.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

HRESULT CGroar_Boss::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Groar"), TEXT("Com_NPC_Model"), reinterpret_cast<CComponent**>(&m_pNPCModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_GroarScene01"), TEXT("Com_Scene01_Model"), reinterpret_cast<CComponent**>(&m_pScene01ModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_GroarScene02"), TEXT("Com_Scene02_Model"), reinterpret_cast<CComponent**>(&m_pScene02ModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Groar_Boss"), TEXT("Com_Boss_Model"), reinterpret_cast<CComponent**>(&m_pBossModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGroar_Boss::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CGroar_Boss* CGroar_Boss::Create(_dev pDevice, _context pContext)
{
	CGroar_Boss* pInstance = new CGroar_Boss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGroar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGroar_Boss::Clone(void* pArg)
{
	CGroar_Boss* pInstance = new CGroar_Boss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGroar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGroar_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pNPCModelCom);
	Safe_Release(m_pScene01ModelCom);
	Safe_Release(m_pScene02ModelCom);
	Safe_Release(m_pBossModelCom);

	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);

}
