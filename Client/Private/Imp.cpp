#include "Imp.h"

const _float CImp::m_fChaseRange = 7.f;
const _float CImp::m_fAttackRange = 1.5f;

CImp::CImp(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CImp::CImp(const CImp& rhs)
	: CMonster(rhs)
{
}

HRESULT CImp::Init_Prototype()
{
    return S_OK;
}

HRESULT CImp::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Imp");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_eCurState = STATE_IDLE;

	m_iHP = 1000;
	m_iDamageAccMax = 300;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.4f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
	ControllerDesc.radius = 0.4f; // ���Ʒ� �ݱ��� ������
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
	ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
	ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	//m_pTransformCom->Set_Position(_vec3(100.f, 8.f, 108.f));

	m_MonsterHpBarPos = _vec3(0.f, 1.2f, 0.f);

	//if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

    return S_OK;
}

void CImp::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	//__super::Update_BodyCollider();

	m_pTransformCom->Gravity(fTimeDelta);

}

void CImp::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif
}

HRESULT CImp::Render()
{
	__super::Render();

    return S_OK;
}

void CImp::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (iDamage > 0)
	{
		m_eCurState = STATE_HIT;

		m_iHP -= iDamage;
		m_bDamaged = true;
		m_bChangePass = true;
		m_fIdleTime = 0.f;

		m_fHittedTime = 6.f;

		CUI_Manager::Get_Instance()->Set_HitEffect(m_pTransformCom, iDamage, _vec2(0.f, 2.f), (ATTACK_TYPE)iDamageType);

		_vec4 vPlayerPos = __super::Compute_PlayerPos();
		m_pTransformCom->LookAt(vPlayerPos);

	}

	if (m_bHit == false)
	{
		m_iDamageAcc += iDamage;
	}

	if (iDamageType == AT_Sword_Common || iDamageType == AT_Sword_Skill1 || iDamageType == AT_Sword_Skill2 ||
		iDamageType == AT_Sword_Skill3 || iDamageType == AT_Sword_Skill4 || iDamageType == AT_Bow_Skill2 || iDamageType == AT_Bow_Skill4)
	{
		// ����
		m_Animation.fAnimSpeedRatio = 1.f;
	}

	if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	{
		// �з�����
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
		m_pTransformCom->Go_To_Dir(vDir, 0.3f);

		m_Animation.fAnimSpeedRatio = 2.5f;
	}

	if (iDamageType == AT_Bow_Skill3)
	{
		// �̼� ��������
		m_bSlow = true;
		m_Animation.fAnimSpeedRatio = 1.f;
	}

	if (iDamageType == AT_OutLine && !m_bChangePass)
	{
		m_iPassIndex = AnimPass_OutLine;
	}

}

void CImp::Init_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	if (m_iHP <= 0)
	{
		m_eCurState = STATE_DIE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CImp::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->Set_Speed(1.f);
			break;

		case Client::CImp::STATE_FLY:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 4.f;

			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CImp::STATE_CHASE:
			if (fDistance >= m_fAttackRange)
			{
				m_Animation.iAnimIndex = RUN;
			}

			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 4.f;

			if (m_bSlow == true)
			{
				m_pTransformCom->Set_Speed(2.f);
				m_Animation.fAnimSpeedRatio = 2.f;
			}
			else
			{
				m_pTransformCom->Set_Speed(4.f);
			}

			break;

		case Client::CImp::STATE_ATTACK:
			m_bDamaged = false;
			m_Animation.fAnimSpeedRatio = 3.f;
			m_bAttacking = true;

			m_pTransformCom->LookAt_Dir(vDir);

			break;

		case Client::CImp::STATE_HIT:
			if (m_bHit == true)
			{
				m_Animation.iAnimIndex = KNOCKDOWN;
			}

			else
			{
				_uint iHitPattern = rand() % 2;
				switch (iHitPattern)
				{
				case 0:
					m_Animation.iAnimIndex = HIT_L;
					break;
				case 1:
					m_Animation.iAnimIndex = HIT_R;
					break;
				}
			}

			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;

		case Client::CImp::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;

			_uint iRandomExp = rand() % 6;
			CUI_Manager::Get_Instance()->Set_Exp_ByPercent(10.f + static_cast<_float>(iRandomExp));
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CImp::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();

	switch (m_eCurState)
	{
	case Client::CImp::STATE_IDLE:
	{
		m_fIdleTime += fTimeDelta;

		if (m_bAttacking == true)
		{
			if (m_fIdleTime >= 1.f)
			{
				if (fDistance >= m_fAttackRange)
				{
					m_eCurState = STATE_CHASE;
				}
				else
				{
					m_eCurState = STATE_ATTACK;
				}

				m_fIdleTime = 0.f;
			}

		}
		else
		{
			if (m_fIdleTime >= 2.f)
			{
				m_eCurState = STATE_FLY;
				m_fIdleTime = 0.f;
			}

		}

		//if (fDistance <= m_fChaseRange)
		//{
		//	m_eCurState = STATE_CHASE;
		//}
	}
		break;

	case Client::CImp::STATE_FLY:
	{
		_float fDist = 1.2f;
		PxRaycastBuffer Buffer{};

		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(),
			m_pTransformCom->Get_State(State::Look).Get_Normalized(),
			fDist, Buffer))
		{
			m_pTransformCom->LookAt_Dir(PxVec3ToVector(Buffer.block.normal));
		}

		m_pTransformCom->Go_Straight(fTimeDelta);

		if (m_pModelCom->IsAnimationFinished(WALK))
		{
			m_eCurState = STATE_IDLE;
		}
	}

		break;

	case Client::CImp::STATE_CHASE:
	{
		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		if (fDistance > m_fChaseRange && !m_bDamaged)
		{
			m_eCurState = STATE_IDLE;
			m_bSlow = false;
			m_bAttacking = false;

			break;
		}

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
			m_Animation.isLoop = true;
			m_bSlow = false;
		}
		else
		{
			m_pTransformCom->LookAt_Dir(vDir);
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
	}
		break;

	case Client::CImp::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK04))
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
				if (fAnimpos >= 42.f && fAnimpos <= 44.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage + rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
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
				if (fAnimpos >= 36.f && fAnimpos <= 38.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage + rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
			}
			break;

		case 2:
			m_Animation.iAnimIndex = ATTACK04;
			m_Animation.isLoop = false;
			m_bSelectAttackPattern = false;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 74.f && fAnimpos <= 76.f && !m_bAttacked)
				{
					_uint iDamage = m_iSmallDamage + rand() % 20;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
			}
			break;
		}

		if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
			m_pModelCom->IsAnimationFinished(ATTACK04))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case Client::CImp::STATE_HIT:

		if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
		{
			m_eCurState = STATE_CHASE;
			m_fIdleTime = 0.f;

			if (m_bHit == true)
			{
				m_iDamageAcc = 0;
				m_bHit = false;
			}
		}

		break;

	case Client::CImp::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}

}

HRESULT CImp::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.3f, 0.4f, 0.2f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	// 1���� : ����ü ����(����), 2���� : Aspect, 3���� : Near, 4���� : Far(����ü ����)
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 0.5f, 0.01f, 1.5f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CImp::Update_Collider()
{
	_mat BodyOffset = _mat::CreateTranslation(0.f, 1.f, 0.f);
	m_pBodyColliderCom->Update(BodyOffset * m_pTransformCom->Get_World_Matrix());


	_mat Offset = _mat::CreateTranslation(0.f, 1.3f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

CImp* CImp::Create(_dev pDevice, _context pContext)
{
	CImp* pInstance = new CImp(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CImp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CImp::Clone(void* pArg)
{
	CImp* pInstance = new CImp(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CImp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CImp::Free()
{
	__super::Free();
}
