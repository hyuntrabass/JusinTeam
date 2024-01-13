#include "Rabbit.h"

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

	m_pTransformCom->Set_State(State::Pos, _vec4(5.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Speed(10.f);

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;

	m_eCurState = STATE_IDLE;

	m_iHP = 1;

	srand((unsigned)time(NULL));

	return S_OK;
}

void CRabbit::Tick(_float fTimeDelta)
{
	Change_State(fTimeDelta);
	Control_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
}

void CRabbit::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	for (size_t i = 0; i < COLL_END; i++)
	{
		m_pRendererCom->Add_DebugComponent(m_pColliderCom[i]);
	}
#endif
}

HRESULT CRabbit::Render()
{
	__super::Render();

	return S_OK;
}

void CRabbit::Change_State(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex) == false && m_Animation.isLoop == false)
	{
		return;
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

			if (m_iRoamingPattern != 0)
			{
				random_device rd;
				_randNum RandNum;
				_randFloat Random = _randFloat(0.f, 1.f);
				m_pTransformCom->LookAt(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}

			break;
		case Client::CRabbit::STATE_ATTACK:
			break;
		case Client::CRabbit::STATE_DIE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = false;
			break;
		}
	}
}

void CRabbit::Control_State(_float fTimeDelta)
{

	switch (m_eCurState)
	{
	case Client::CRabbit::STATE_IDLE:

		if (m_fIdleTime >= 2.f)
		{
			m_eCurState = STATE_ROAM;
			m_fIdleTime = 0.f;
		}

		m_fIdleTime += fTimeDelta;

		break;
	case Client::CRabbit::STATE_ROAM:

	{
		_uint iRandom = rand() % 3;

		switch (m_iRoamingPattern)
		{
		case 0:
			m_Animation.iAnimIndex = ROAR;
			m_Animation.isLoop = false;
			break;
		case 1:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = false;
			break;
		case 2:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			break;
		}

		if (m_pModelCom->Get_CurrentAnimationIndex() == RUN || m_pModelCom->Get_CurrentAnimationIndex() == WALK)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

	}

		break;
	case Client::CRabbit::STATE_ATTACK:
		break;
	case Client::CRabbit::STATE_DIE:
		break;
	}

	if (m_pModelCom->IsAnimationFinished(ROAR) ||
		m_pModelCom->IsAnimationFinished(RUN) || m_pModelCom->IsAnimationFinished(WALK))
	{
		m_eCurState = STATE_IDLE;
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
