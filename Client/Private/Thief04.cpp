#include "Thief04.h"

const _float CThief04::m_fChaseRange = 7.f;
const _float CThief04::m_fAttackRange = 2.f;

CThief04::CThief04(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CThief04::CThief04(const CThief04& rhs)
	: CMonster(rhs)
{
}

HRESULT CThief04::Init_Prototype()
{
    return S_OK;
}

HRESULT CThief04::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Thief04");

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

	m_iHP = 30;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

    return S_OK;
}

void CThief04::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_T))
	{
		Set_Damage(0, WP_BOW);
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_MonsterCollider();
}

void CThief04::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CThief04::Render()
{
	__super::Render();

    return S_OK;
}

void CThief04::Set_Damage(_int iDamage, _uint iDamageType)
{
	m_iHP -= iDamage;
	m_bDamaged = true;

	m_eCurState = STATE_HIT;

	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	m_pTransformCom->LookAt(vPlayerPos);

	if (iDamageType == WP_BOW)
	{
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();

		m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);
	}

	else if (iDamageType == WP_SWORD)
	{
	}
}

void CThief04::Init_State(_float fTimeDelta)
{
	if (m_iHP <= 0)
	{
		m_eCurState = STATE_DIE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CThief04::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(1.5f);
			break;

		case Client::CThief04::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CThief04::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(3.f);
			break;

		case Client::CThief04::STATE_ATTACK:
			m_bDamaged = false;
			break;

		case Client::CThief04::STATE_HIT:
		{
			_uint iHitPattern = rand() % 2;

			switch (iHitPattern)
			{
			case 0:
				m_Animation.iAnimIndex = L_HIT;
				m_Animation.isLoop = false;
				break;
			case 1:
				m_Animation.iAnimIndex = R_HIT;
				m_Animation.isLoop = false;
				break;
			}
		}
			break;

		case Client::CThief04::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CThief04::Tick_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CThief04::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 2.f)
		{
			m_eCurState = STATE_WALK;
			m_fIdleTime = 0.f;
		}

		//_float fDistance = __super::Compute_PlayerDistance();
		//if (fDistance <= m_fChaseRange)
		//{
		//	m_eCurState = STATE_CHASE;
		//}

		break;

	case Client::CThief04::STATE_WALK:
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (m_pModelCom->IsAnimationFinished(WALK))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case Client::CThief04::STATE_CHASE:
	{
		_vec4 vPlayerPos = __super::Compute_PlayerPos();
		_float fDistance = __super::Compute_PlayerDistance();

		m_pTransformCom->LookAt(vPlayerPos);
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (fDistance > m_fChaseRange && !m_bDamaged)
		{
			m_eCurState = STATE_IDLE;
		}

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
			m_Animation.isLoop = true;
		}

	}
		break;

	case Client::CThief04::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03) || m_pModelCom->IsAnimationFinished(ATTACK04) ||
				m_pModelCom->IsAnimationFinished(ATTACK05))
			{
				m_iAttackPattern = rand() % 5;
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
				if (fAnimpos >= 46.f && fAnimpos <= 48.f && !m_bAttacked)
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
				if (fAnimpos >= 24.f && fAnimpos <= 26.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
				if (fAnimpos >= 47.f && fAnimpos <= 49.f && !m_bAttacked2)
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
				if (fAnimpos >= 37.f && fAnimpos <= 39.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
			}
			break;
		case 3:
			m_Animation.iAnimIndex = ATTACK04;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 33.f && fAnimpos <= 35.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
			}
			break;
		case 4:
			m_Animation.iAnimIndex = ATTACK05;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 44.f && fAnimpos <= 46.f && !m_bAttacked)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked = true;
				}
			}
			break;
		}

		if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
			m_pModelCom->IsAnimationFinished(ATTACK03) || m_pModelCom->IsAnimationFinished(ATTACK04) ||
			m_pModelCom->IsAnimationFinished(ATTACK05))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CThief04::STATE_HIT:

		if (m_pModelCom->IsAnimationFinished(L_HIT) || m_pModelCom->IsAnimationFinished(R_HIT))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CThief04::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_iPassIndex = AnimPass_Dissolve;
		}

		break;
	}
}

HRESULT CThief04::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.3f, 0.8f, 0.5f);
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
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1.f, 0.01f, 2.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

    return S_OK;
}

void CThief04::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 0.8f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CThief04* CThief04::Create(_dev pDevice, _context pContext)
{
	CThief04* pInstance = new CThief04(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CThief04");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CThief04::Clone(void* pArg)
{
	CThief04* pInstance = new CThief04(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CThief04");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThief04::Free()
{
	__super::Free();
}
