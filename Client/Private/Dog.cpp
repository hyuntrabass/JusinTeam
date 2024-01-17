#include "Dog.h"

CDog::CDog(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CDog::CDog(const CDog& rhs)
	: CNPC(rhs)
{
}

HRESULT CDog::Init_Prototype()
{
    return S_OK;
}

HRESULT CDog::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Dog");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	//m_pTransformCom->Set_State(State::Pos, _vec4(5.f, 0.f, 5.f, 1.f));
	m_pTransformCom->Set_Speed(1.f);

	m_Animation.iAnimIndex = IDLE01;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fInterpolationTime = 0.4f;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_eCurState = STATE_IDLE;

    return S_OK;
}

void CDog::Tick(_float fTimeDelta)
{
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);
}

void CDog::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CDog::Render()
{
	__super::Render();

    return S_OK;
}

void CDog::Init_State(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
	{
		m_eCurState = STATE_IDLE;
	}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CDog::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE01;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.5f;
			m_Animation.fInterpolationTime = 0.4f;
			m_pTransformCom->Set_Speed(1.f);
			break;
		case Client::CDog::STATE_SIT:
			m_Animation.iAnimIndex = IDLE02;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 0.2f;
			m_Animation.fInterpolationTime = 0.1f;
			break;
		case Client::CDog::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 1.8f;
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;
		case Client::CDog::STATE_RUN:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = false;
			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CDog::Tick_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CDog::STATE_IDLE:
		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 6.f)
		{
			if (!m_bSelectPattern)
			{
				m_iPattern = rand() % 3;

				switch (m_iPattern)
				{
				case 0:
					m_eCurState = STATE_SIT;
					break;
				case 1:
					m_eCurState = STATE_WALK;
					break;
				case 2:
					m_eCurState = STATE_RUN;
					break;
				}
			}
			m_fIdleTime = 0.f;
		}

		break;
	case Client::CDog::STATE_SIT:
		break;
	case Client::CDog::STATE_WALK:
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_pTransformCom->Set_Speed(1.5f);
		break;
	case Client::CDog::STATE_RUN:
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_pTransformCom->Set_Speed(1.8f);
		break;
	}
}

CDog* CDog::Create(_dev pDevice, _context pContext)
{
	CDog* pInstance = new CDog(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDog::Clone(void* pArg)
{
	CDog* pInstance = new CDog(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDog::Free()
{
	__super::Free();
}
