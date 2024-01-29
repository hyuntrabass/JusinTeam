#include "Groar_Boss.h"

#include "Missile.h"

const _float CGroar_Boss::m_fChaseRange = 7.f;
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

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 30) + 60.f, 0.f, static_cast<_float>(rand() % 30) + 60.f, 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(71.f, 0.f, 97.f, 1.f));

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
		m_eCurState = STATE_SCENE01;
		/*m_eCurState = STATE_BOSS;
		m_eBossCurState = BOSS_STATE_ROAR;*/
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	Update_Collider();
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

#ifdef _DEBUGTEST
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

	switch (m_eCurState)
	{
	case Client::CGroar_Boss::STATE_NPC:
		for (_uint i = 0; i < m_pNPCModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pNPCModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pNPCModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
			{
				HasNorTex = false;
			}
			else
			{
				HasNorTex = true;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pNPCModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pNPCModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
		break;
	case Client::CGroar_Boss::STATE_SCENE01:
		for (_uint i = 0; i < m_pScene01ModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pScene01ModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pScene01ModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
			{
				HasNorTex = false;
			}
			else
			{
				HasNorTex = true;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pScene01ModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pScene01ModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
		break;
	case Client::CGroar_Boss::STATE_SCENE02:
		for (_uint i = 0; i < m_pScene02ModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pScene02ModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pScene02ModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
			{
				HasNorTex = false;
			}
			else
			{
				HasNorTex = true;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pScene02ModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pScene02ModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
		break;
	case Client::CGroar_Boss::STATE_BOSS:
		for (_uint i = 0; i < m_pBossModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pBossModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pBossModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
			{
				HasNorTex = false;
			}
			else
			{
				HasNorTex = true;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pBossModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pBossModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
		break;
	}

	return S_OK;
}

void CGroar_Boss::Init_State(_float fTimeDelta)
{
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

			break;

		case Client::CGroar_Boss::BOSS_STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_bSelectAttackPattern = false;

			m_bCreateMissile = false;

			break;

		case Client::CGroar_Boss::BOSS_STATE_THROW_ATTACK: // 00, 01
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK00;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;

		case Client::CGroar_Boss::BOSS_STATE_FLOOR_ATTACK: // 03, 06, 07, 08 

		{
			_uint iFloorAttackPattern = rand() % 4;

			switch (iFloorAttackPattern)
			{
			case 0:
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK03;
				m_Animation.isLoop = false;
				m_Animation.fAnimSpeedRatio = 2.f;

				break;

			case 1:
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK06;
				m_Animation.isLoop = false;
				m_Animation.fAnimSpeedRatio = 2.f;

				break;

			case 2:
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK07;
				m_Animation.isLoop = false;
				m_Animation.fAnimSpeedRatio = 2.f;

				break;

			case 3:
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK08;
				m_Animation.isLoop = false;
				m_Animation.fAnimSpeedRatio = 2.f;

				break;
			}
		}

			break;

		case Client::CGroar_Boss::BOSS_STATE_SIX_MISSILE: // 02
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK02;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_WEB: // 04
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK04;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CGroar_Boss::BOSS_STATE_SPIDER: // 05
			m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK05;
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

		m_pGameInstance->Set_ShakeCam(true, 0.5f);

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK_RAGE))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_CHASE:
	{
		CTransform* pPlayerTransform = GET_TRANSFORM("Layer_ModelTest", LEVEL_GAMEPLAY);
		_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

		_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
		_float fDistance = (vPlayerPos - vPos).Length();

		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		m_pTransformCom->LookAt_Dir(vDir);
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (fDistance <= m_fAttackRange)
		{
			//m_eBossCurState = BOSS_STATE_SPIDER; // 패턴 시작
			m_Animation.isLoop = false;

			if (!m_bSelectAttackPattern)
			{
				m_iAttackPattern = rand() % 5;
				switch (m_iAttackPattern)
				{
				case 0:
					m_eBossCurState = BOSS_STATE_THROW_ATTACK;
					break;
				case 1:
					m_eBossCurState = BOSS_STATE_FLOOR_ATTACK;
					break;
				case 2:
					m_eBossCurState = BOSS_STATE_SIX_MISSILE;
					break;
				case 3:
					m_eBossCurState = BOSS_STATE_WEB;
					break;
				case 4:
					m_eBossCurState = BOSS_STATE_SPIDER;
					break;
				}
				//switch (m_eBossPreState)
				//{
				//case Client::CGroar_Boss::BOSS_STATE_CHASE:
				//	m_eBossCurState = BOSS_STATE_THROW_ATTACK;
				//	break;

				//case Client::CGroar_Boss::BOSS_STATE_THROW_ATTACK:

				//	m_iAttackPattern = rand() % 3;
				//	switch (m_iAttackPattern)
				//	{
				//	case 0:
				//		m_eBossCurState = BOSS_STATE_FLOOR_ATTACK;
				//		break;
				//	case 1:
				//		m_eBossCurState = BOSS_STATE_WEB;
				//		break;
				//	case 2:
				//		m_eBossCurState = BOSS_STATE_SPIDER;
				//		break;
				//	}

				//	break;
				//case Client::CGroar_Boss::BOSS_STATE_FLOOR_ATTACK:

				//	m_iAttackPattern = rand() % 4;
				//	switch (m_iAttackPattern)
				//	{
				//	case 0:
				//		m_eBossCurState = BOSS_STATE_THROW_ATTACK;
				//		break;
				//	case 1:
				//		m_eBossCurState = BOSS_STATE_SIX_MISSILE;
				//		break;
				//	case 2:
				//		m_eBossCurState = BOSS_STATE_WEB;
				//		break;
				//	case 3:
				//		m_eBossCurState = BOSS_STATE_SPIDER;
				//		break;
				//	}

				//	break;
				//case Client::CGroar_Boss::BOSS_STATE_SIX_MISSILE:

				//	m_iAttackPattern = rand() % 3;
				//	switch (m_iAttackPattern)
				//	{
				//	case 0:
				//		m_eBossCurState = BOSS_STATE_FLOOR_ATTACK;
				//		break;
				//	case 1:
				//		m_eBossCurState = BOSS_STATE_WEB;
				//		break;
				//	case 2:
				//		m_eBossCurState = BOSS_STATE_SPIDER;
				//		break;
				//	}

				//	break;
				//case Client::CGroar_Boss::BOSS_STATE_WEB:

				//	m_iAttackPattern = rand() % 4;
				//	switch (m_iAttackPattern)
				//	{
				//	case 0:
				//		m_eBossCurState = BOSS_STATE_THROW_ATTACK;
				//		break;
				//	case 1:
				//		m_eBossCurState = BOSS_STATE_FLOOR_ATTACK;
				//		break;
				//	case 2:
				//		m_eBossCurState = BOSS_STATE_SIX_MISSILE;
				//		break;
				//	case 3:
				//		m_eBossCurState = BOSS_STATE_SPIDER;
				//		break;
				//	}

				//	break;
				//case Client::CGroar_Boss::BOSS_STATE_SPIDER:

				//	m_iAttackPattern = rand() % 4;
				//	switch (m_iAttackPattern)
				//	{
				//	case 0:
				//		m_eBossCurState = BOSS_STATE_THROW_ATTACK;
				//		break;
				//	case 1:
				//		m_eBossCurState = BOSS_STATE_FLOOR_ATTACK;
				//		break;
				//	case 2:
				//		m_eBossCurState = BOSS_STATE_SIX_MISSILE;
				//		break;
				//	case 3:
				//		m_eBossCurState = BOSS_STATE_WEB;
				//		break;
				//	}

				//	break;
				//}

				m_bSelectAttackPattern = true;
			}
		}
	}

		break;

	case Client::CGroar_Boss::BOSS_STATE_THROW_ATTACK:

		if (m_pBossModelCom->Get_CurrentAnimationIndex() == MON_GROAR_ASGARD_ATTACK00)
		{
			if (m_pBossModelCom->Get_CurrentAnimPos() >= 10.f /*&& m_pBossModelCom->Get_CurrentAnimPos() <= 12.f*/ && !m_bCreateMissile)
			{
				CMissile::MISSILE_TYPE eType = CMissile::RIGHT_THROW;
				m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
				m_bCreateMissile = true;

				++m_iThrowAttackCombo;
			}

			if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK00))
			{
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK01;

				//m_bCreateMissile = false;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() < 10.f)
			{
				m_bCreateMissile = false;
			}

		}

		else if (m_pBossModelCom->Get_CurrentAnimationIndex() == MON_GROAR_ASGARD_ATTACK01)
		{
			if (m_pBossModelCom->Get_CurrentAnimPos() >= 15.f /*&& m_pBossModelCom->Get_CurrentAnimPos() <= 17.f*/ && !m_bCreateMissile)
			{
				CMissile::MISSILE_TYPE eType = CMissile::LEFT_THROW;
				m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
				m_bCreateMissile = true;

				++m_iThrowAttackCombo;
			}

			if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK01))
			{
				m_Animation.iAnimIndex = MON_GROAR_ASGARD_ATTACK00;

				//m_bCreateMissile = false;
				//m_eBossCurState = BOSS_STATE_CHASE;
			}

			if (m_pBossModelCom->Get_CurrentAnimPos() < 15.f)
			{
				m_bCreateMissile = false;
			}

		}

		//if (m_pBossModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
		//{
		//	//m_bCreateMissile = false;
		//	m_eBossCurState = BOSS_STATE_CHASE;
		//}

		if (m_iThrowAttackCombo == 4)
		{
			if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK00) || m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK01))
			{
				m_iThrowAttackCombo = 0;
				m_eBossCurState = BOSS_STATE_CHASE;
			}
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_FLOOR_ATTACK:

		switch (m_Animation.iAnimIndex)
		{
		case MON_GROAR_ASGARD_ATTACK03:
			break;

		case MON_GROAR_ASGARD_ATTACK06:
			break;

		case MON_GROAR_ASGARD_ATTACK07:
			break;

		case MON_GROAR_ASGARD_ATTACK08:
			break;
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK03) || m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK06) ||
			m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK07) || m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK08))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_SIX_MISSILE:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 60.f && !m_bCreateMissile)
		{
			for (size_t i = 0; i < 6; i++)
			{
				CMissile::MISSILE_TYPE eType = CMissile::SIX_MISSILE;
				m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Missile"), TEXT("Prototype_GameObject_Missile"), &eType);
			}			

			m_bCreateMissile = true;
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK02))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_WEB:

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK04))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
		}

		break;

	case Client::CGroar_Boss::BOSS_STATE_SPIDER:

		if (m_pBossModelCom->Get_CurrentAnimPos() >= 80.f && !m_bCreateSpider)
		{
			for (size_t i = 0; i < 3; i++)
			{
				m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Spider"), TEXT("Prototype_GameObject_Spider"));
			}

			m_bCreateSpider = true;
		}

		if (m_pBossModelCom->IsAnimationFinished(MON_GROAR_ASGARD_ATTACK05))
		{
			m_eBossCurState = BOSS_STATE_CHASE;
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
		return E_FAIL;

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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Groar"), TEXT("Com_NPC_Model"), reinterpret_cast<CComponent**>(&m_pNPCModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_GroarScene01"), TEXT("Com_Scene01_Model"), reinterpret_cast<CComponent**>(&m_pScene01ModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_GroarScene02"), TEXT("Com_Scene02_Model"), reinterpret_cast<CComponent**>(&m_pScene02ModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Groar_Boss"), TEXT("Com_Boss_Model"), reinterpret_cast<CComponent**>(&m_pBossModelCom))))
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

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
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
