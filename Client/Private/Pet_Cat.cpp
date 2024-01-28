#include "Pet_Cat.h"

CPet_Cat::CPet_Cat(_dev pDevice, _context pContext)
	: CPet(pDevice, pContext)
{
}

CPet_Cat::CPet_Cat(const CPet_Cat& rhs)
	: CPet(rhs)
{
}

HRESULT CPet_Cat::Init_Prototype()
{
    return S_OK;
}

HRESULT CPet_Cat::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Pet_Cat");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(75.f, 0.f, 110.f, 1.f));
	m_pTransformCom->Set_Scale(_vec3(1.5f, 1.5f, 1.5f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;

	m_eCurState = STATE_IDLE;

	m_fPosLerpRatio = 0.02f;
	m_fLookLerpRatio = 0.04f;

    return S_OK;
}

void CPet_Cat::Tick(_float fTimeDelta)
{
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);
}

void CPet_Cat::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPet_Cat::Render()
{
	__super::Render();

    return S_OK;
}

void CPet_Cat::Init_State(_float fTimeDelta)
{
}

void CPet_Cat::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);

	_vec4 vPlayerRight = pPlayerTransform->Get_State(State::Right).Get_Normalized();
	_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look).Get_Normalized();

	_vec3 vTargetPos = vPlayerPos - (1.f * vPlayerRight) - (1.f * vPlayerLook);
	_vec3 vMyPos = m_pTransformCom->Get_State(State::Pos);

	if (fDistance <= 5.f && fDistance >= 2.f)
	{
		m_fPosLerpRatio = 0.03f;
	}
	else if (fDistance > 5.f)
	{
		m_fPosLerpRatio = 0.05f;
	}

	_vec3 vSetPos = XMVectorLerp(vMyPos, vTargetPos, m_fPosLerpRatio);
	vSetPos.y = vPlayerPos.y + 1.5f;

	//_vec4 vMyLook = vMyPos + m_pTransformCom->Get_State(State::Look).Get_Normalized();
	//_vec4 vTargetLook = vMyPos + vPlayerLook;

	//_vec4 vSetLook = XMVectorLerp(vMyLook, vTargetLook, m_fLookLerpRatio);

	//m_pTransformCom->LookAt(vSetLook);

	_vec4 vMyLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_vec4 vTargetLook = vPlayerLook;

	_vec4 vSetLook = XMVectorLerp(vMyLook, vTargetLook, m_fLookLerpRatio);
	//vSetLook.y = 0.f;

	m_pTransformCom->LookAt_Dir(vSetLook);

	if (fDistance >= 2.f)
	{
		m_Animation.iAnimIndex = RUN;
		m_pTransformCom->Set_Position(vSetPos);
	}	
	else
	{
		m_Animation.iAnimIndex = IDLE;
	}

}

CPet_Cat* CPet_Cat::Create(_dev pDevice, _context pContext)
{
	CPet_Cat* pInstance = new CPet_Cat(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPet_Cat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPet_Cat::Clone(void* pArg)
{
	CPet_Cat* pInstance = new CPet_Cat(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPet_Cat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPet_Cat::Free()
{
	__super::Free();
}
