#include "Void05.h"

const _float CVoid05::m_fChaseRange = 7.f;
const _float CVoid05::m_fAttackRange = 3.f;

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

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 30) + 60.f, 0.f, static_cast<_float>(rand() % 30) + 60.f, 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;

	m_eCurState = STATE_IDLE;

	m_iHP = 1000;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	TRAIL_DESC Desc{};
	Desc.vColor = Colors::BurlyWood;
	Desc.vPSize = _vec2(0.03f, 0.03f);
	Desc.iNumVertices = 10.f;
	m_pLeftTrail = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);
	m_pRightTrail = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &Desc);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.5f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
	ControllerDesc.radius = 1.1f; // ���Ʒ� �ݱ��� ������
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
	ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
	ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	m_pTransformCom->Set_Position(_vec3(100.f, 8.f, 108.f));

	if (pArg)
	{
		m_pInfo = *(MonsterInfo*)pArg;
		_mat WorldPos = m_pInfo.MonsterWorldMat;
		m_pTransformCom->Set_Matrix(WorldPos);
	}

	return S_OK;
}

void CVoid05::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_5))
	{
		Set_Damage(0, AT_Sword_Common);
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);
	
	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_MonsterCollider();

	Update_Trail(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);

	__super::Tick(fTimeDelta);
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
	m_bDamaged = true;

	m_eCurState = STATE_HIT;

	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	m_pTransformCom->LookAt(vPlayerPos);

	if (iDamageType == AT_Sword_Common || iDamageType == AT_Sword_Skill1 || iDamageType == AT_Sword_Skill2 ||
		iDamageType == AT_Sword_Skill3 || iDamageType == AT_Sword_Skill4 || iDamageType == AT_Bow_Skill2 || iDamageType == AT_Bow_Skill4)
	{
		// ����
		//m_Animation.fAnimSpeedRatio = 4.f;
	}

	if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	{
		// �з�����
		_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
		m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

		//m_Animation.fAnimSpeedRatio = 3.f;
	}

	if (iDamageType == AT_Bow_Skill3)
	{
		// �̼� ��������
		m_bSlow = true;
		//m_Animation.fAnimSpeedRatio = 0.8f;
	}
}

void CVoid05::Init_State(_float fTimeDelta)
{
	if (m_iHP <= 0)
	{
		m_eCurState = STATE_DIE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CVoid05::STATE_IDLE:
			m_Animation.iAnimIndex = TU02_SC02_MON_ATTACK_LOOP;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_pTransformCom->Set_Speed(1.5f);
			break;

		case Client::CVoid05::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			if (m_bSlow == true)
			{
				m_pTransformCom->Set_Speed(1.f);
			}
			else
			{
				m_pTransformCom->Set_Speed(4.f);
			}
			break;

		case Client::CVoid05::STATE_ATTACK:
			m_bDamaged = false;
			break;

		case Client::CVoid05::STATE_HIT:
			m_Animation.iAnimIndex = ROAR;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 10.f;

			//m_iHitPercentage = rand() % 3;
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
	switch (m_eCurState)
	{
	case Client::CVoid05::STATE_IDLE:
		m_Animation.fAnimSpeedRatio = 3.f;
		break;

	case Client::CVoid05::STATE_CHASE:
	{
		_vec4 vPlayerPos = __super::Compute_PlayerPos();
		_float fDistance = __super::Compute_PlayerDistance();
		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		m_pTransformCom->LookAt_Dir(vDir);
		m_pTransformCom->Go_Straight(fTimeDelta);

		//if (fDistance > m_fChaseRange && !m_bDamaged)
		//{
		//	m_eCurState = STATE_IDLE;
		//	m_bSlow = false;
		//}

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
			m_Animation.isLoop = true;
			m_bSlow = false;
		}
	}
		break;

	case Client::CVoid05::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03))
			{
				m_iAttackPattern++;

				if (m_iAttackPattern == 3)
				{
					m_iAttackPattern = 0;
				}

				//m_iAttackPattern = rand() % 3;
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
					_uint iDamage = m_iSmallDamage / 2 + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 43.f && fAnimpos <= 52.f)
				{
					m_pLeftTrail->Late_Tick(fTimeDelta);
					m_pRightTrail->Late_Tick(fTimeDelta);
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
					_uint iDamage = m_iSmallDamage / 2 + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
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
					_uint iDamage = m_iSmallDamage / 2 + rand() % 10;
					m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
					m_bAttacked = true;
				}
				if (fAnimpos >= 17.f && fAnimpos <= 50.f)
				{
					m_pLeftTrail->Late_Tick(fTimeDelta);
					m_pRightTrail->Late_Tick(fTimeDelta);
				}
			}
			break;
		}

		if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
			m_pModelCom->IsAnimationFinished(ATTACK03))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CVoid05::STATE_HIT:

		if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CVoid05::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_iPassIndex = AnimPass_Dissolve;
		}

		break;
	}
}

void CVoid05::Update_Trail(_float fTimeDelta)
{
	_mat LMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-L-Finger22");
	LMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pLeftTrail->Tick(LMatrix.Position_vec3());

	_mat RMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Finger22");
	RMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pRightTrail->Tick(RMatrix.Position_vec3());
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
	// 1���� : ����ü ����(����), 2���� : Aspect, 3���� : Near, 4���� : Far(����ü ����)
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

	Safe_Release(m_pLeftTrail);
	Safe_Release(m_pRightTrail);
}
