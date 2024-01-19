#include "Thief04.h"

const _float CThief04::g_fChaseRange = 7.f;
const _float CThief04::g_fAttackRange = 2.f;

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

	m_iHP = 10;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

    return S_OK;
}

void CThief04::Tick(_float fTimeDelta)
{
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
	m_pRendererCom->Add_DebugComponent(m_pAxeColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pKnifeColliderCom);

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

	if (iDamageType == WP_BOW)
	{
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();

		m_pTransformCom->Go_To_Dir(vDir, 0.1f);
	}

	else if (iDamageType == WP_SWORD)
	{
	}
}

void CThief04::Init_State(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
	{
		m_eCurState = STATE_IDLE;
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
			break;

		case Client::CThief04::STATE_HIT:
			m_iHitPattern = rand() % 2;
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
	Attack(fTimeDelta);

	switch (m_eCurState)
	{
	case Client::CThief04::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 2.f)
		{
			m_eCurState = STATE_WALK;
			m_fIdleTime = 0.f;
		}
		break;

	case Client::CThief04::STATE_WALK:
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;

	case Client::CThief04::STATE_CHASE:
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

	case Client::CThief04::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03) || m_pModelCom->IsAnimationFinished(ATTACK04) ||
				m_pModelCom->IsAnimationFinished(ATTACK05))
			{
				m_iAttackPattern = rand() % 5;
				m_bSelectAttackPattern = true;
			}
		}

		switch (m_iAttackPattern)
		{
		case 0:
			m_Animation.iAnimIndex = ATTACK01;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			break;
		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			break;
		case 2:
			m_Animation.iAnimIndex = ATTACK03;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			break;
		case 3:
			m_Animation.iAnimIndex = ATTACK04;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			break;
		case 4:
			m_Animation.iAnimIndex = ATTACK05;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			break;
		}
		break;

	case Client::CThief04::STATE_HIT:

		switch (m_iHitPattern)
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
		break;

	case Client::CThief04::STATE_DIE:
		break;
	}

}

void CThief04::Attack(_float fTimeDelta)
{
	_float fDistance = __super::Compute_PlayerDistance();

	if (fDistance <= g_fChaseRange)
	{
		if (m_eCurState == STATE_ATTACK)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03) || m_pModelCom->IsAnimationFinished(ATTACK04)||
				m_pModelCom->IsAnimationFinished(ATTACK05))
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

HRESULT CThief04::Add_Collider()
{
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 0.1f;
	CollDesc.vCenter = _vec3(0.35f, 0.f, 0.15f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_AxeCollider_Sphere"), (CComponent**)&m_pAxeColliderCom, &CollDesc)))
		return E_FAIL;

	CollDesc.fRadius = 0.05f;
	CollDesc.vCenter = _vec3(0.3f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_KnifeCollider_Sphere"), (CComponent**)&m_pKnifeColliderCom, &CollDesc)))
		return E_FAIL;

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
	_mat AxeMatrix = *(m_pModelCom->Get_BoneMatrix("bone_R_weapon_01"));
	AxeMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pAxeColliderCom->Update(AxeMatrix);

	_mat KnifeMatrix = *(m_pModelCom->Get_BoneMatrix("bone_L_weapon_01"));
	KnifeMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pKnifeColliderCom->Update(KnifeMatrix);

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

	Safe_Release(m_pAxeColliderCom);
	Safe_Release(m_pKnifeColliderCom);
}
