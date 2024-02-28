#include "Void23.h"
#include "TreasureBox.h"

const _float CVoid23::m_fChaseRange = 7.f;
const _float CVoid23::m_fAttackRange = 5.f;

CVoid23::CVoid23(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CVoid23::CVoid23(const CVoid23& rhs)
	: CMonster(rhs)
{
}

HRESULT CVoid23::Init_Prototype()
{
	return S_OK;
}

HRESULT CVoid23::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Void23");

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

	m_eCurState = STATE_IDLE;

	m_iHP = 7000;
	m_iDamageAccMax = 2000;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	SURFACETRAIL_DESC Desc{};
	Desc.vColor = Colors::Goldenrod;
	Desc.iNumVertices = 10;

	m_pSwordTrail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	Desc.iPassIndex = 2;
	Desc.strMaskTextureTag = L"FX_B_CraterCrack002_Normal_Tex";
	m_pDistTrail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 2.2f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
	ControllerDesc.radius = 0.9f; // ���Ʒ� �ݱ��� ������
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
	ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
	ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	//m_pTransformCom->Set_Position(_vec3(100.f, 8.f, 108.f));

	m_MonsterHpBarPos = _vec3(0.f, 5.f, 0.f);

	//if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

	_vec3 vPos = m_pTransformCom->Get_State(State::Pos);
	vPos.y += 3.f;

	m_pTransformCom->Set_Position(vPos);

	return S_OK;
}

void CVoid23::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
	__super::Update_BodyCollider();

	Update_Trail(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);

}

void CVoid23::Late_Tick(_float fTimeDelta)
{
	m_pSwordTrail->Late_Tick(fTimeDelta);
	m_pDistTrail->Late_Tick(fTimeDelta);

	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pFloorCollider);
#endif
}

HRESULT CVoid23::Render()
{
	__super::Render();

	return S_OK;
}

void CVoid23::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (iDamage > 0)
	{
		m_iHP -= iDamage;
		m_bDamaged = true;
		m_bChangePass = true;
		m_fHittedTime = 6.f;
		m_fIdleTime = 0.f;

		CUI_Manager::Get_Instance()->Set_HitEffect(m_pTransformCom, iDamage, _vec2(0.f, 1.5f), (ATTACK_TYPE)iDamageType);

		_vec4 vPlayerPos = __super::Compute_PlayerPos();
		m_pTransformCom->LookAt(vPlayerPos);

		if (iDamage >= 500 && m_eCurState != STATE_KNOCKDOWN)
		{
			m_eCurState = STATE_HIT;
		}
		else
		{
			if (m_eCurState != STATE_KNOCKDOWN)
			{
				m_eCurState = STATE_CHASE;
			}
		}
	}	

	//if (m_bHit == false)
	//{
	//	m_iDamageAcc += iDamage;
	//}

	//if (iDamageType == AT_Sword_Common || iDamageType == AT_Sword_Skill1 || iDamageType == AT_Sword_Skill2 ||
	//	iDamageType == AT_Sword_Skill3 || iDamageType == AT_Sword_Skill4 || iDamageType == AT_Bow_Skill2 || iDamageType == AT_Bow_Skill4)
	//{
	//	// ����
	//	m_Animation.fAnimSpeedRatio = 1.5f;
	//}

	//if (iDamageType == AT_Bow_Common || iDamageType == AT_Bow_Skill1)
	//{
	//	// �з�����
	//	_vec4 vDir = m_pTransformCom->Get_State(State::Pos) - __super::Compute_PlayerPos();
	//	m_pTransformCom->Go_To_Dir(vDir, m_fBackPower);

	//	m_Animation.fAnimSpeedRatio = 2.5f;
	//}

	//if (iDamageType == AT_Bow_Skill3)
	//{
	//	// �̼� ��������
	//	m_bSlow = true;
	//	m_Animation.fAnimSpeedRatio = 1.f;
	//}
}

void CVoid23::Init_State(_float fTimeDelta)
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
		case Client::CVoid23::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fInterpolationTime = 0.2f;

			m_pTransformCom->Set_Speed(1.5f);

			m_fIdleTime = 0.f;

			m_bSelectAttackPattern = false;

			m_bAttacked = false;
			m_bAttacked2 = false;

			break;

		case Client::CVoid23::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 3.f;

			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CVoid23::STATE_CHASE:
			if (fDistance >= m_fAttackRange)
			{
				m_Animation.iAnimIndex = RUN;
			}

			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 3.f;
			m_Animation.fInterpolationTime = 0.2f;

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

		case Client::CVoid23::STATE_ATTACK:
			//m_bDamaged = false;
			m_Animation.fAnimSpeedRatio = 2.5f;
			m_Animation.isLoop = false;
			m_bAttacking = true;

			m_pTransformCom->LookAt_Dir(vDir);

			break;

		case Client::CVoid23::STATE_HIT:
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

		case Client::CVoid23::STATE_KNOCKDOWN:
			m_Animation.iAnimIndex = KNOCKDOWN;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			m_Animation.fInterpolationTime = 0.4f;
			break;

		case Client::CVoid23::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CVoid23::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
	vDir.y = 0.f;

	switch (m_eCurState)
	{
	case Client::CVoid23::STATE_IDLE:
	{
		if (m_bDamaged)
		{
			m_fIdleTime += fTimeDelta;

			if (m_fIdleTime >= 1.f)
			{
				m_eCurState = STATE_CHASE;
			}
		}

		//if (m_bAttacking == true)
		//{
		//	if (m_fIdleTime >= 1.f)
		//	{
		//		if (fDistance >= m_fAttackRange)
		//		{
		//			m_eCurState = STATE_CHASE;
		//		}
		//		else
		//		{
		//			m_eCurState = STATE_ATTACK;
		//		}

		//		m_fIdleTime = 0.f;
		//	}

		//}
		//else
		//{
		//	//if (m_fIdleTime >= 2.f)
		//	//{
		//	//	m_eCurState = STATE_WALK;
		//	//	m_fIdleTime = 0.f;
		//	//}

		//}

		//if (fDistance <= m_fChaseRange)
		//{
		//	m_eCurState = STATE_CHASE;
		//}
	}
	break;

	case Client::CVoid23::STATE_WALK:
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

	case Client::CVoid23::STATE_CHASE:
	{
		if (!m_bSelectAttackPattern)
		{
			m_iAttackPattern = { INT_MAX };

			_bool bReset = { true };
			for (auto& it : m_bAttack_Selected)
			{
				if (it == false)
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

			VOID23_ATTACK eAttackRandom = static_cast<VOID23_ATTACK>(rand() % ATTACK_END);

			while (m_bAttack_Selected[eAttackRandom] == true)
			{
				eAttackRandom = static_cast<VOID23_ATTACK>(rand() % ATTACK_END);
			}

			switch (eAttackRandom)
			{
			case Client::CVoid23::ATTACK1:
				m_iAttackPattern = 0;
				break;
			case Client::CVoid23::ATTACK2:
				m_iAttackPattern = 1;
				break;
			case Client::CVoid23::ATTACK3:
				m_iAttackPattern = 2;
				break;
			case Client::CVoid23::ATTACK4:
				m_iAttackPattern = 3;
				break;
			case Client::CVoid23::ATTACK5:
				m_iAttackPattern = 4;
				break;
			}

			m_bSelectAttackPattern = true;

		}

		m_pTransformCom->LookAt_Dir(vDir);

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
			m_Animation.isLoop = true;
			m_bSlow = false;
		}
		else
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}
	}
	break;

	case Client::CVoid23::STATE_ATTACK:

		switch (m_iAttackPattern)
		{
		case 0:
			m_Animation.iAnimIndex = B_ATTACK01;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 66.f && fAnimpos <= 68.f)
				{
					if (m_pGameInstance->CheckCollision_Parrying(m_pAttackColliderCom))
					{
						m_eCurState = STATE_KNOCKDOWN;
						break;
					}

					if (!m_bAttacked)
					{
						_uint iDamage = m_iDefaultDamage2 + rand() % 50;
						m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
						m_bAttacked = true;
					}
				}

				if (fAnimpos >= 117.f && fAnimpos <= 119.f)
				{
					if (!m_bAttacked2)
					{
						_uint iDamage = m_iDefaultDamage2 + rand() % 50;
						m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
						m_bAttacked2 = true;
					}
				}

				if (fAnimpos >= 60.f && fAnimpos <= 76.f)
				{
					m_pSwordTrail->On();
					m_pDistTrail->On();
				}

				if (fAnimpos >= 110.f && fAnimpos <= 124.f)
				{
					m_pSwordTrail->On();
					m_pDistTrail->On();
				}

				if (m_pModelCom->IsAnimationFinished(B_ATTACK01))
				{
					m_eCurState = STATE_IDLE;
					m_bAttack_Selected[ATTACK1] = true;
				}
			}
			break;

		case 1:
			m_Animation.iAnimIndex = B_ATTACK02;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 55.f && fAnimpos <= 57.f)
				{
					if (m_pGameInstance->CheckCollision_Parrying(m_pAttackColliderCom))
					{
						m_eCurState = STATE_KNOCKDOWN;
						break;
					}

					if (!m_bAttacked)
					{
						_uint iDamage = m_iDefaultDamage2 + rand() % 50;
						m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Stun);
						m_bAttacked = true;
					}
				}
				if (fAnimpos >= 98.f && fAnimpos <= 100.f)
				{
					if (!m_bAttacked2)
					{
						_uint iDamage = m_iDefaultDamage2 + rand() % 50;
						m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
						m_bAttacked2 = true;
					}
				}
				if (fAnimpos >= 49.f && fAnimpos <= 57.f)
				{
					m_pSwordTrail->On();
					m_pDistTrail->On();
				}

				if (m_pModelCom->IsAnimationFinished(B_ATTACK02))
				{
					m_eCurState = STATE_IDLE;
					m_bAttack_Selected[ATTACK2] = true;
				}

			}
			break;

		case 2:
			m_Animation.iAnimIndex = B_ATTACK03;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 108.f && fAnimpos <= 110.f)
				{
					if (m_pGameInstance->CheckCollision_Parrying(m_pAttackColliderCom))
					{
						m_eCurState = STATE_KNOCKDOWN;
						break;
					}

					if (!m_bAttacked)
					{
						_uint iDamage = m_iDefaultDamage2 + rand() % 50;
						//m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_KnockDown);
						m_pGameInstance->Attack_Player(m_pFloorCollider, iDamage, MonAtt_Stun);
						m_bAttacked = true;
					}
				}
				if (fAnimpos >= 101.f && fAnimpos <= 109.f)
				{
					m_pSwordTrail->On();
					m_pDistTrail->On();
				}

				if (m_pModelCom->IsAnimationFinished(B_ATTACK03))
				{
					m_eCurState = STATE_IDLE;
					m_bAttack_Selected[ATTACK3] = true;
				}

			}
			break;

		case 3:
			m_Animation.iAnimIndex = B_ATTACK04;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 70.f && fAnimpos <= 72.f)
				{
					if (m_pGameInstance->CheckCollision_Parrying(m_pAttackColliderCom))
					{
						m_eCurState = STATE_KNOCKDOWN;
						break;
					}

					if (!m_bAttacked)
					{
						_uint iDamage = m_iDefaultDamage2 + rand() % 50;
						m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
						m_bAttacked = true;
					}
				}
				if (fAnimpos >= 60.f && fAnimpos <= 80.f)
				{
					m_pSwordTrail->On();
					m_pDistTrail->On();
				}

				if (m_pModelCom->IsAnimationFinished(B_ATTACK04))
				{
					m_eCurState = STATE_IDLE;
					m_bAttack_Selected[ATTACK4] = true;
				}

			}
			break;

		case 4:
			m_Animation.iAnimIndex = B_ATTACK05;
			{
				_float fAnimpos = m_pModelCom->Get_CurrentAnimPos();
				if (fAnimpos >= 65.f && fAnimpos <= 67.f)
				{
					if (!m_bAttacked)
					{
						_uint iDamage = m_iDefaultDamage2 + rand() % 50;
						m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
						m_bAttacked = true;
					}
				}
				if (fAnimpos >= 118.f && fAnimpos <= 120.f)
				{
					if (!m_bAttacked2)
					{
						_uint iDamage = m_iDefaultDamage2 + rand() % 50;
						m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage, MonAtt_Hit);
						m_bAttacked2 = true;
					}
				}
				if (fAnimpos >= 58.f && fAnimpos <= 125.f)
				{
					m_pSwordTrail->On();
					m_pDistTrail->On();
				}

				if (m_pModelCom->IsAnimationFinished(B_ATTACK05))
				{
					m_eCurState = STATE_IDLE;
					m_bAttack_Selected[ATTACK5] = true;
				}

			}
			break;

		}

		break;

	case Client::CVoid23::STATE_HIT:

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

	case Client::CVoid23::STATE_KNOCKDOWN:

		if (m_pModelCom->IsAnimationFinished(KNOCKDOWN))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CVoid23::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(DIE))
		{
			m_fDeadTime += fTimeDelta;
			if (!m_isReward)
			{
				CTreasureBox::TREASURE_DESC Desc{};
				_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
				Desc.vPos = vPos;
				vector <pair<wstring, _uint>> vecItem;
				vecItem.push_back(make_pair(TEXT("[��ȭ]�ź��� ��"), 1));
				vecItem.push_back(make_pair(TEXT("�׷ξ� ������ ����"), 1));
				vecItem.push_back(make_pair(TEXT("���䷹������ �Ҹ� ����"), 1));
				vecItem.push_back(make_pair(TEXT("���䷹������ �Ҹ� ����"), 1));
				Desc.vecItem = vecItem;
				Desc.eDir = CTreasureBox::RIGHT;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Temp"), TEXT("Prototype_GameObject_TreasureBox"), &Desc)))
				{
					return;
				}
				vecItem.clear();
				m_isReward = true;
			}
		}

		break;
	}
}

void CVoid23::Update_Trail(_float fTimeDelta)
{
	//if (m_pGameInstance->Get_CameraModeIndex() != CM_MAIN)
	//{
	//	return;
	//}

	_mat Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Hand");
	_mat Offset = _mat::CreateTranslation(_vec3(6.01f, -0.67f, -0.53f));
	_mat Result1 = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	Matrix = *m_pModelCom->Get_BoneMatrix("Bip001-R-Hand");
	Offset = _mat::CreateTranslation(_vec3(2.21f, -0.41f, 0.02f));
	_mat Result2 = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();

	m_pSwordTrail->Tick(Result1.Position_vec3(), Result2.Position_vec3());

	Offset = _mat::CreateTranslation(_vec3(10.01f, -0.67f, -0.53f));
	Result1 = Offset * Matrix * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();
	m_pDistTrail->Tick(Result1.Position_vec3(), Result2.Position_vec3());
}

HRESULT CVoid23::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.7f, 2.2f, 0.5f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	Collider_Desc FloorCollDesc = {};
	FloorCollDesc.eType = ColliderType::AABB;
	FloorCollDesc.vExtents = _vec3(10.f, 0.1f, 10.f);
	FloorCollDesc.vCenter = _vec3(0.f, FloorCollDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pFloorCollider, &FloorCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	// 1���� : ����ü ����(����), 2���� : Aspect, 3���� : Near, 4���� : Far(����ü ����)
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 1.f, 0.01f, 7.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CVoid23::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 2.f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());

	//_mat Offset = _mat::CreateTranslation(0.f, 2.f, 0.f);
	m_pFloorCollider->Update(/*Offset **/ m_pTransformCom->Get_World_Matrix());
}

CVoid23* CVoid23::Create(_dev pDevice, _context pContext)
{
	CVoid23* pInstance = new CVoid23(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVoid23");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVoid23::Clone(void* pArg)
{
	CVoid23* pInstance = new CVoid23(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVoid23");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVoid23::Free()
{
	__super::Free();

	Safe_Release(m_pSwordTrail);
	Safe_Release(m_pDistTrail);
	Safe_Release(m_pFloorCollider);
}
