#include "Pet_Dragon.h"

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

	m_pTransformCom->Set_State(State::Pos, _vec4(70.f, 0.f, 110.f, 1.f));

	m_Animation.iAnimIndex = PET_04_IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;

	m_eCurState = STATE_IDLE;

	return S_OK;
}

void CPet_Dragon::Tick(_float fTimeDelta)
{
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);
}

void CPet_Dragon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPet_Dragon::Render()
{
	__super::Render();

    return S_OK;
}

void CPet_Dragon::Init_State(_float fTimeDelta)
{
}

void CPet_Dragon::Tick_State(_float fTimeDelta)
{
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
