#include "Rabbit.h"

const _float CRabbit::g_fChaseRange = 5.f;
const _float CRabbit::g_fAttackRange = 2.f;

CRabbit::CRabbit(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CRabbit::CRabbit(const CRabbit& rhs)
	: CMonster(rhs)
{
}

HRESULT CRabbit::Init_Prototype()
{
	return S_OK;
}

HRESULT CRabbit::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Rabbit");

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

	m_iHP = 10;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	return S_OK;
}

void CRabbit::Tick(_float fTimeDelta)
{
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_MonsterCollider();

}

void CRabbit::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	for (size_t i = 0; i < COLL_END; i++)
	{
		m_pRendererCom->Add_DebugComponent(m_pColliderCom[i]);
	}

	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CRabbit::Render()
{
	__super::Render();

	return S_OK;
}

void CRabbit::Set_Damage(_int iDamage, _uint iDamageType)
{
	m_iHP -= iDamage;

	if (iDamageType == WP_BOW)
	{
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();

		m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);
	}

	else if (iDamageType == WP_SWORD)
	{
	}
}

void CRabbit::Init_State(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
	{
		m_eCurState = STATE_IDLE;
	}

	if (m_iHP <= 0)
	{
		m_eCurState = STATE_DIE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CRabbit::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			break;
		case Client::CRabbit::STATE_ROAM:
			m_iRoamingPattern = rand() % 3;

			if (m_iRoamingPattern == 1 && m_pModelCom->IsAnimationFinished(RUN) == false)
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}

			break;
		case Client::CRabbit::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			break;
		case Client::CRabbit::STATE_ATTACK:
			break;
		case Client::CRabbit::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;

	}
}

void CRabbit::Tick_State(_float fTimeDelta)
{
	if (m_iHP > 0)
	{
		Attack(fTimeDelta);
	}

	if (m_pGameInstance->Key_Down(DIK_R, InputChannel::GamePlay))
	{
		m_eCurState = STATE_DIE;
	}

	switch (m_eCurState)
	{
	case Client::CRabbit::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 2.f)
		{
			m_eCurState = STATE_ROAM;
			m_fIdleTime = 0.f;
		}

		break;
	case Client::CRabbit::STATE_ROAM:

		switch (m_iRoamingPattern)
		{
		case 0:
			m_Animation.iAnimIndex = ROAR;
			m_Animation.isLoop = false;
			break;
		case 1:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = false;
			m_pTransformCom->Go_Straight(fTimeDelta);
			break;
		case 2:
			m_Animation.iAnimIndex = STUN;
			m_Animation.isLoop = false;
			break;
		}

		break;
	case Client::CRabbit::STATE_CHASE:

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
	case Client::CRabbit::STATE_ATTACK:
		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02))
			{
				m_iAttackPattern = rand() % 2;
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
				if (fAnimpos >= 45.f && fAnimpos <= 47.f && !m_bAttacked)
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
				if (fAnimpos >= 52.f && fAnimpos <= 54.f && !m_bAttacked2)
				{
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, 2, 0);
					m_bAttacked2 = true;
				}
			}
			break;
		}
		break;
	case Client::CRabbit::STATE_DIE:
		break;
	}


}

void CRabbit::Attack(_float fTimeDelta)
{
	_float fDistance = __super::Compute_PlayerDistance();

	if (fDistance <= g_fChaseRange)
	{
		if (m_eCurState == STATE_ATTACK)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02))
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

HRESULT CRabbit::Add_Collider()
{
	// Com_Collider
	Collider_Desc RCollDesc = {};
	RCollDesc.eType = ColliderType::Sphere;
	RCollDesc.fRadius = 0.1f;
	RCollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	RABBIT_COLL eColl = {};

	for (size_t i = 0; i < COLL_END; i++)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider_Sphere") + to_wstring(i), (CComponent**)&m_pColliderCom[i], &RCollDesc)))
			return E_FAIL;
	}

	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.2f, 0.4f, 0.6f);
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
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1.f / 2.f, 0.01f, 2.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CRabbit::Update_Collider()
{
	_mat LeftHandMatrix = *(m_pModelCom->Get_BoneMatrix("Bip001-L-Finger0"));
	LeftHandMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pColliderCom[LH]->Update(LeftHandMatrix);


	_mat RightHandMatrix = *(m_pModelCom->Get_BoneMatrix("Bip001-R-Finger0"));
	RightHandMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pColliderCom[RH]->Update(RightHandMatrix);

	_mat LeftFootMatrix = *(m_pModelCom->Get_BoneMatrix("Bip001-L-Toe0"));
	LeftFootMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pColliderCom[LF]->Update(LeftFootMatrix);


	_mat RightFootMatrix = *(m_pModelCom->Get_BoneMatrix("Bip001-R-Toe0"));
	RightFootMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pColliderCom[RF]->Update(RightFootMatrix);

	_mat Offset = _mat::CreateTranslation(0.f, 0.5f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CRabbit* CRabbit::Create(_dev pDevice, _context pContext)
{
	CRabbit* pInstance = new CRabbit(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRabbit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRabbit::Clone(void* pArg)
{
	CRabbit* pInstance = new CRabbit(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CRabbit");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRabbit::Free()
{
	__super::Free();

	for (size_t i = 0; i < COLL_END; i++)
	{
		Safe_Release(m_pColliderCom[i]);
	}
}
