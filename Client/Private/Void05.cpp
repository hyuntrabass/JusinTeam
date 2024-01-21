#include "Void05.h"

const _float CVoid05::g_fChaseRange = 7.f;
const _float CVoid05::g_fAttackRange = 2.f;

CVoid05::CVoid05(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CVoid05::CVoid05(const CVoid05& rhs)
	: CMonster(rhs)
{
}

HRESULT CVoid05::Init_Prototype()
{
    return S_OK;
}

HRESULT CVoid05::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Void05");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(10.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_eCurState = STATE_IDLE;

	m_iHP = 10;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	return S_OK;
}

void CVoid05::Tick(_float fTimeDelta)
{
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_MonsterCollider();
}

void CVoid05::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CVoid05::Render()
{
	__super::Render();

    return S_OK;
}

void CVoid05::Set_Damage(_int iDamage, _uint iDamageType)
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
	}
}

void CVoid05::Init_State(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
	{
		m_eCurState = STATE_IDLE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CVoid05::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(1.5f);
			break;

		case Client::CVoid05::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(3.f);
			break;

		case Client::CVoid05::STATE_ATTACK:
			break;

		case Client::CVoid05::STATE_HIT:
			m_iHitPercentage = rand() % 3;
			break;

		case Client::CVoid05::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CVoid05::Tick_State(_float fTimeDelta)
{
	Attack(fTimeDelta);

	switch (m_eCurState)
	{
	case Client::CVoid05::STATE_IDLE:
		break;

	case Client::CVoid05::STATE_CHASE:
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

	case Client::CVoid05::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03))
			{
				m_iAttackPattern = rand() % 3;
				m_bSelectAttackPattern = true;
				m_bAttacked = false;
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
				if (fAnimpos >= 48.f && fAnimpos <= 50.f && !m_bAttacked)
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
				if (fAnimpos >= 51.f && fAnimpos <= 53.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
			}
			break;
		case 2:
			m_Animation.iAnimIndex = ATTACK03;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 22.f && fAnimpos <= 24.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
			}
			break;
		}

		break;

	case Client::CVoid05::STATE_HIT:

		if (m_iHitPercentage == 0)
		{
			m_Animation.iAnimIndex = ROAR;
			m_Animation.isLoop = false;
		}
		break;

	case Client::CVoid05::STATE_DIE:
		break;
	}
}

void CVoid05::Attack(_float fTimeDelta)
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

HRESULT CVoid05::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.5f, 0.5f, 1.1f);
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
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1.5f, 0.01f, 2.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CVoid05::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 0.5f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CVoid05* CVoid05::Create(_dev pDevice, _context pContext)
{
	CVoid05* pInstance = new CVoid05(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVoid05");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVoid05::Clone(void* pArg)
{
	CVoid05* pInstance = new CVoid05(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVoid05");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVoid05::Free()
{
	__super::Free();
}
