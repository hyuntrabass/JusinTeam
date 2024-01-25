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

	m_pTransformCom->Set_State(State::Pos, _vec4(75.f, 0.f, 110.f, 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;

	m_eCurState = STATE_IDLE;

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
