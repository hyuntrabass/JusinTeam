#include "BrickCat.h"

#include "Effect_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"

CBrickCat::CBrickCat(_dev pDevice, _context pContext)
	: CPet(pDevice, pContext)
{
}

CBrickCat::CBrickCat(const CBrickCat& rhs)
	: CPet(rhs)
{
}

HRESULT CBrickCat::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickCat::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Pet_Cat");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(75.f, 0.f, 110.f, 1.f));
	m_Animation.fAnimSpeedRatio = 2.f;
	m_Animation.bSkipInterpolation = false;

	m_pTransformCom->Set_Scale(_vec3(3.f, 3.f, 3.f));

	m_eCurState = STATE_CHASE;

	m_fPosLerpRatio = 0.02f;
	m_fLookLerpRatio = 0.04f;

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Pet_Cat_Parti");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info, true);

	/*
	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Pet_Cat_Light");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	*/

	return S_OK;
}

void CBrickCat::Tick(_float fTimeDelta)
{
	if (CCamera_Manager::Get_Instance()->Get_CameraState() != CS_BRICKGAME)
	{
		m_isDead = true;
		return;
	}


	__super::Tick(fTimeDelta);
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	m_EffectMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-Spine") * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();
}

void CBrickCat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBrickCat::Render()
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

void CBrickCat::Init_State(_float fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CBrickCat::STATE_IDLE:
			break;
		case Client::CBrickCat::STATE_CHASE:
			m_fIdleTime = 0.f;
			m_bInvenOn = false;

			break;
		case Client::CBrickCat::STATE_EMOTION:
		{
			_uint iRandom = rand() % 3;
			switch (iRandom)
			{
			case 0:
				m_Animation.iAnimIndex = EMOTION;
				break;

			case 1:
				m_Animation.iAnimIndex = TELEPORT_END;
				break;

			case 2:
				m_Animation.iAnimIndex = TELEPORT_START;
				break;
			}
		}

			m_Animation.isLoop = false;

			m_fIdleTime = 0.f;

		break;
		case Client::CBrickCat::STATE_INVEN:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;

			m_fIdleTime = 0.f;
			m_bInvenOn = true;

			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CBrickCat::Tick_State(_float fTimeDelta)
{

	CTransform * pBarTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_BrickGame"), TEXT("BrickBarTransform"));
	_vec4 vBarPos = pBarTransform->Get_State(State::Pos);

	_float fDistance = __super::Compute_PlayerDistance();

	_vec4 vBarRight = pBarTransform->Get_State(State::Right).Get_Normalized();
	_vec4 vBarLook = pBarTransform->Get_State(State::Look).Get_Normalized();

	_vec3 vTargetPos = vBarPos;// +(1.f * vBarRight) + (1.f * vBarLook);
	vTargetPos.z += 1.f;
	_vec3 vMyPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 vMyLook = _vec4(0.f, 0.f, -1.f, 0.f);

	switch (m_eCurState)
	{
	case Client::CBrickCat::STATE_IDLE:
		break;

	case Client::CBrickCat::STATE_CHASE:

	{
		m_pTransformCom->Set_Position(vTargetPos);

		m_pTransformCom->LookAt_Dir(vMyLook);
	}
	break;

	case Client::CBrickCat::STATE_EMOTION:

	{
		if (CUI_Manager::Get_Instance()->Is_InvenActive() == false)
		{
			if (fDistance <= 5.f && fDistance >= 2.f)
			{
				m_fPosLerpRatio = 0.1f;
			}
			else if (fDistance > 5.f)
			{
				m_fPosLerpRatio = 0.1f;
			}

			_vec3 vSetPos = XMVectorLerp(vMyPos, vTargetPos, m_fPosLerpRatio);
			_vec4 vSetLook = XMVectorLerp(vMyLook, vBarLook, m_fLookLerpRatio);

			m_pTransformCom->LookAt_Dir(vSetLook);

			if (CCamera_Manager::Get_Instance()->Get_AimMode() == true)
			{
				m_pTransformCom->Set_Position(vTargetPos);

				break;
			}

			if (fDistance >= 2.f)
			{
				m_pTransformCom->Set_Position(vSetPos);
			}
		}

		if (m_pModelCom->IsAnimationFinished(EMOTION) || m_pModelCom->IsAnimationFinished(TELEPORT_END) || m_pModelCom->IsAnimationFinished(TELEPORT_START))
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
	}

		break;


	case Client::CBrickCat::STATE_INVEN:

		m_pTransformCom->Set_Position(_vec3(vBarPos.x + 1.7f, vBarPos.y + 1.f, vBarPos.z - 1.5f));
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

CBrickCat* CBrickCat::Create(_dev pDevice, _context pContext)
{
	CBrickCat* pInstance = new CBrickCat(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickCat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickCat::Clone(void* pArg)
{
	CBrickCat* pInstance = new CBrickCat(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickCat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickCat::Free()
{
	__super::Free();
}
