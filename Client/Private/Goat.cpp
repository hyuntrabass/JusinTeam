#include "Goat.h"

const _float CGoat::g_fChaseRange = 5.f;
const _float CGoat::g_fAttackRange = 2.f;

CGoat::CGoat(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CGoat::CGoat(const CGoat& rhs)
	: CMonster(rhs)
{
}

HRESULT CGoat::Init_Prototype()
{
	return S_OK;
}

HRESULT CGoat::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Goat");

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
	m_pTransformCom->Set_Speed(1.f);

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_eCurState = STATE_IDLE;

	m_iHP = 1;

	return S_OK;
}

void CGoat::Tick(_float fTimeDelta)
{
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
}

void CGoat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CGoat::Render()
{
	__super::Render();

	return S_OK;
}

void CGoat::Init_State(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
	{
		m_eCurState = STATE_IDLE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CGoat::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(1.f);
			break;

		case Client::CGoat::STATE_ROAM:
			m_iRoamingPattern = rand() % 3;

			if (m_iRoamingPattern == 1 && m_pModelCom->IsAnimationFinished(WALK) == false)
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;

		case Client::CGoat::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_pTransformCom->Set_Speed(3.f);
			break;

		case Client::CGoat::STATE_ATTACK:
			break;

		case Client::CGoat::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;

	}
}

void CGoat::Tick_State(_float fTimeDelta)
{
	Attack(fTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_G, InputChannel::GamePlay))
	{
		m_eCurState = STATE_DIE;
	}

	switch (m_eCurState)
	{
	case Client::CGoat::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 2.f)
		{
			m_eCurState = STATE_ROAM;
			m_fIdleTime = 0.f;
		}
		break;

	case Client::CGoat::STATE_ROAM:

		switch (m_iRoamingPattern)
		{
		case 0:
			m_Animation.iAnimIndex = ROAR;
			m_Animation.isLoop = false;
			break;
		case 1:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			m_pTransformCom->Go_Straight(fTimeDelta);
			break;
		case 2:
			m_Animation.iAnimIndex = STUN;
			m_Animation.isLoop = false;
			break;
		}
		break;

	case Client::CGoat::STATE_CHASE:
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

	case Client::CGoat::STATE_ATTACK:

		if (!m_bSelectAttackPattern)
		{
			if (m_pModelCom->IsAnimationFinished(ATTACK01) || m_pModelCom->IsAnimationFinished(ATTACK02) ||
				m_pModelCom->IsAnimationFinished(ATTACK03))
			{
				m_iAttackPattern = rand() % 3;
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
		}
		break;

	case Client::CGoat::STATE_DIE:
		break;
	}
}

void CGoat::Attack(_float fTimeDelta)
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

HRESULT CGoat::Add_Collider()
{
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 0.2f;
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CGoat::Update_Collider()
{
	_mat Matrix = *(m_pModelCom->Get_BoneMatrix("Bip001-HeadNub"));
	Matrix *= XMMatrixTranslation(0.f, 0.2f, 0.1f);
	Matrix *= m_pTransformCom->Get_World_Matrix();

	m_pColliderCom->Update(Matrix);
}

CGoat* CGoat::Create(_dev pDevice, _context pContext)
{
	CGoat* pInstance = new CGoat(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGoat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGoat::Clone(void* pArg)
{
	CGoat* pInstance = new CGoat(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGoat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoat::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
