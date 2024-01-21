#include "Nastron03.h"

const _float CNastron03::g_fChaseRange = 7.f;
const _float CNastron03::g_fAttackRange = 3.f;

CNastron03::CNastron03(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CNastron03::CNastron03(const CNastron03& rhs)
	: CMonster(rhs)
{
}

HRESULT CNastron03::Init_Prototype()
{
	return S_OK;
}

HRESULT CNastron03::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Nastron03");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(5.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_Speed(3.f);

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_eCurState = STATE_IDLE;

	m_iHP = 30;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	return S_OK;
}

void CNastron03::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_B))
	{
		Set_Damage(1, WP_BOW);
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_MonsterCollider();
}

void CNastron03::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);

	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CNastron03::Render()
{
	__super::Render();

	return S_OK;
}

void CNastron03::Set_Damage(_int iDamage, _uint iDamageType)
{
	m_iHP -= iDamage;

	if (iDamageType == WP_BOW)
	{
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();

		m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

		m_eCurState = STATE_HIT;
	}

	else if (iDamageType == WP_SWORD)
	{
		m_eCurState = STATE_HIT;
	}
}

void CNastron03::Init_State(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
	{
		m_eCurState = STATE_IDLE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CNastron03::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(3.f);
			break;

		case Client::CNastron03::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;

			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CNastron03::STATE_CHASE:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(5.f);
			break;

		case Client::CNastron03::STATE_ATTACK:
			break;

		case Client::CNastron03::STATE_HIT:
			m_iHitPattern = rand() % 2;
			break;

		case Client::CNastron03::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CNastron03::Tick_State(_float fTimeDelta)
{
	Attack(fTimeDelta);

	switch (m_eCurState)
	{
	case Client::CNastron03::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 2.f)
		{
			m_eCurState = STATE_WALK;
			m_fIdleTime = 0.f;
		}
		break;

	case Client::CNastron03::STATE_WALK:
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;

	case Client::CNastron03::STATE_CHASE:
	{
		_vec4 vPlayerPos = __super::Compute_PlayerPos();
		_float fDistance = __super::Compute_PlayerDistance();

		m_pTransformCom->LookAt(vPlayerPos);
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (fDistance > g_fChaseRange)
		{
			m_eCurState = STATE_IDLE;
		}
	}
		break;

	case Client::CNastron03::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03))
			{
				m_iAttackPattern = rand() % 3;
				m_bSelectAttackPattern = true;
				m_bAttacked = false;
				m_bAttacked2 = false;
			}
		}

		switch (m_iAttackPattern)
		{
		case 0:
			m_Animation.iAnimIndex = ATTACK01;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 51.f && fAnimpos <= 53.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
			}
			break;
		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 37.f && fAnimpos <= 39.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
				if (fAnimpos >= 68.f && fAnimpos <= 70.f && !m_bAttacked2)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked2 = true;
				}
			}
			break;
		case 2:
			m_Animation.iAnimIndex = ATTACK03;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 110.f && fAnimpos <= 112.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
			}
			break;
		}
		break;

	case Client::CNastron03::STATE_HIT:

		switch (m_iHitPattern)
		{
		case 0:
			m_Animation.iAnimIndex = HIT_L;
			m_Animation.isLoop = false;
			break;
		case 1:
			m_Animation.iAnimIndex = HIT_R;
			m_Animation.isLoop = false;
			break;
		}
		break;

	case Client::CNastron03::STATE_DIE:
		break;
	}
}

void CNastron03::Attack(_float fTimeDelta)
{
	_float fDistance = __super::Compute_PlayerDistance();

	if (fDistance <= g_fChaseRange)
	{
		if (m_eCurState == STATE_ATTACK)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03))
			{
				m_eCurState = STATE_CHASE;
			}
		}

		else
		{
			m_eCurState = STATE_CHASE;
		}
	}

	if (fDistance <= g_fAttackRange)
	{
		m_eCurState = STATE_ATTACK;
		m_Animation.isLoop = true;
	}
}

HRESULT CNastron03::Add_Collider()
{
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 0.3f;
	CollDesc.vCenter = _vec3(0.f, 0.7f, -1.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.6f, 1.4f, 0.4f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	// 1인자 : 절두체 각도(범위), 2인자 : Aspect, 3인자 : Near, 4인자 : Far(절두체 깊이)
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 2.f, 0.01f, 2.5f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CNastron03::Update_Collider()
{
	_mat Matrix = *(m_pModelCom->Get_BoneMatrix("Bip001-Prop1_end"));
	Matrix *= m_pTransformCom->Get_World_Matrix();

	m_pColliderCom->Update(Matrix);

	_mat Offset = _mat::CreateTranslation(0.f, 1.5f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CNastron03* CNastron03::Create(_dev pDevice, _context pContext)
{
	CNastron03* pInstance = new CNastron03(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNastron03");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNastron03::Clone(void* pArg)
{
	CNastron03* pInstance = new CNastron03(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNastron03");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNastron03::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
