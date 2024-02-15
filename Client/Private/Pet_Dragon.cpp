#include "Pet_Dragon.h"
#include "Effect_Manager.h"
#include "UI_Manager.h"

CPet_Dragon::CPet_Dragon(_dev pDevice, _context pContext)
	: CPet(pDevice, pContext)
{
}

CPet_Dragon::CPet_Dragon(const CPet_Dragon& rhs)
	: CPet(rhs)
{
}

HRESULT CPet_Dragon::Init_Prototype()
{
    return S_OK;
}

HRESULT CPet_Dragon::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Pet_Dragon");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(70.f, 0.f, 110.f, 1.f));

	m_Animation.fAnimSpeedRatio = 2.f;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fInterpolationTime = 0.5f;

	m_pTransformCom->Set_Scale(_vec3(1.5f, 1.5f, 1.5f));

	m_eCurState = STATE_CHASE;

	m_fPosLerpRatio = 0.02f;
	m_fLookLerpRatio = 0.04f;

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Pet_Dragon_Parti");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info, true);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Pet_Dragon_Light");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info, true);

	return S_OK;
}

void CPet_Dragon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	m_EffectMatrix = *m_pModelCom->Get_BoneMatrix("B_Jaw") * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();
}

void CPet_Dragon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPet_Dragon::Render()
{
	CTransform* pCameraTransform = GET_TRANSFORM("Layer_Camera", LEVEL_STATIC);
	_vec4 vCameraPos = pCameraTransform->Get_State(State::Pos);

	_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);

	if ((vCameraPos - vMyPos).Length() > 1.f)
	{
		__super::Render();
	}

    return S_OK;
}

void CPet_Dragon::Init_State(_float fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CPet_Dragon::STATE_IDLE:
			break;
		case Client::CPet_Dragon::STATE_CHASE:
			m_fIdleTime = 0.f;
			m_bInvenOn = false;

			break;
		case Client::CPet_Dragon::STATE_EMOTION:
		{
			_uint iRandom = rand() % 3;
			switch (iRandom)
			{
			case 0:
				m_Animation.iAnimIndex = PET_04_EMOTION;
				break;

			case 1:
				m_Animation.iAnimIndex = PET_05_EMOTION;
				break;

			case 2:
				m_Animation.iAnimIndex = PET_06_EMOTION;
				break;
			}
		}

		m_Animation.isLoop = false;

		m_fIdleTime = 0.f;

		break;
		case Client::CPet_Dragon::STATE_INVEN:
			m_Animation.iAnimIndex = PET_04_IDLE;
			m_Animation.isLoop = true;

			m_fIdleTime = 0.f;
			m_bInvenOn = true;

			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CPet_Dragon::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);

	_vec4 vPlayerRight = pPlayerTransform->Get_State(State::Right).Get_Normalized();
	_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look).Get_Normalized();

	_vec3 vTargetPos = vPlayerPos - (1.f * vPlayerRight) - (1.f * vPlayerLook);
	vTargetPos.y += 1.5f;

	_vec3 vMyPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 vMyLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();

	switch (m_eCurState)
	{
	case Client::CPet_Dragon::STATE_IDLE:
		break;

	case Client::CPet_Dragon::STATE_CHASE:

	{
		if (fDistance <= 5.f && fDistance >= 2.f)
		{
			m_fPosLerpRatio = 0.03f;
		}
		else if (fDistance > 5.f)
		{
			m_fPosLerpRatio = 0.05f;
		}

		_vec3 vSetPos = XMVectorLerp(vMyPos, vTargetPos, m_fPosLerpRatio);
		_vec4 vSetLook = XMVectorLerp(vMyLook, vPlayerLook, m_fLookLerpRatio);

		m_pTransformCom->LookAt_Dir(vSetLook);

		if (fDistance >= 2.f)
		{
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;

			m_pTransformCom->Set_Position(vSetPos);
		}

		if (fDistance < 2.1f)
		{
			m_Animation.iAnimIndex = PET_04_IDLE;
			m_Animation.isLoop = true;

			m_fIdleTime += fTimeDelta;

			if (m_fIdleTime >= 5.f + static_cast<_float>(rand() % 3))
			{
				m_eCurState = STATE_EMOTION;
			}

			if (CUI_Manager::Get_Instance()->Is_InvenActive() == true)
			{
				m_eCurState = STATE_INVEN;
			}
		}

		if (fDistance > 10.f)
		{
			m_pTransformCom->Set_Position(vTargetPos);
			m_pTransformCom->LookAt_Dir(vPlayerLook);
		}

	}

	break;

	case Client::CPet_Dragon::STATE_EMOTION:

	{
		if (CUI_Manager::Get_Instance()->Is_InvenActive() == false)
		{
			if (fDistance <= 5.f && fDistance >= 2.f)
			{
				m_fPosLerpRatio = 0.03f;
			}
			else if (fDistance > 5.f)
			{
				m_fPosLerpRatio = 0.05f;
			}

			_vec3 vSetPos = XMVectorLerp(vMyPos, vTargetPos, m_fPosLerpRatio);
			_vec4 vSetLook = XMVectorLerp(vMyLook, vPlayerLook, m_fLookLerpRatio);

			m_pTransformCom->LookAt_Dir(vSetLook);

			if (fDistance >= 2.f)
			{
				m_pTransformCom->Set_Position(vSetPos);
			}
		}

		if (m_pModelCom->IsAnimationFinished(PET_04_EMOTION) || m_pModelCom->IsAnimationFinished(PET_05_EMOTION) || m_pModelCom->IsAnimationFinished(PET_06_EMOTION))
		{
			if (CUI_Manager::Get_Instance()->Is_InvenActive() == true)
			{
				m_eCurState = STATE_INVEN;
			}
			else
			{
				m_eCurState = STATE_CHASE;
			}
		}

		if (m_bInvenOn != CUI_Manager::Get_Instance()->Is_InvenActive())
		{
			m_eCurState = STATE_INVEN;
		}
	}

		break;

	case Client::CPet_Dragon::STATE_INVEN:

		m_pTransformCom->Set_Position(_vec3(vPlayerPos.x + 1.7f, vPlayerPos.y + 1.5f, vPlayerPos.z - 1.5f));
		m_pTransformCom->LookAt_Dir(_vec4(0.f, 0.f, 1.f, 0.f));

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 5.f + static_cast<_float>(rand() % 3))
		{
			m_eCurState = STATE_EMOTION;
		}

		if (CUI_Manager::Get_Instance()->Is_InvenActive() == false)
		{
			m_eCurState = STATE_CHASE;
		}

		break;
	}
}

CPet_Dragon* CPet_Dragon::Create(_dev pDevice, _context pContext)
{
	CPet_Dragon* pInstance = new CPet_Dragon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPet_Dragon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_Dragon::Clone(void* pArg)
{
	CPet_Dragon* pInstance = new CPet_Dragon(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPet_Dragon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_Dragon::Free()
{
	__super::Free();
}
