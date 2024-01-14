#include "NPCvsMon.h"

CNPCvsMon::CNPCvsMon(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CNPCvsMon::CNPCvsMon(const CNPCvsMon& rhs)
	: CMonster(rhs)
{
}

HRESULT CNPCvsMon::Init_Prototype()
{
    return S_OK;
}

HRESULT CNPCvsMon::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_NPCvsMon");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(5.f, 0.f, 5.f, 1.f));

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 1.5f;

    return S_OK;
}

void CNPCvsMon::Tick(_float fTimeDelta)
{
	m_pModelCom->Set_Animation(m_Animation);
}

void CNPCvsMon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CNPCvsMon::Render()
{
	__super::Render();

    return S_OK;
}

HRESULT CNPCvsMon::Add_Collider()
{
    return S_OK;
}

void CNPCvsMon::Update_Collider()
{
}

CNPCvsMon* CNPCvsMon::Create(_dev pDevice, _context pContext)
{
	CNPCvsMon* pInstance = new CNPCvsMon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNPCvsMon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPCvsMon::Clone(void* pArg)
{
	CNPCvsMon* pInstance = new CNPCvsMon(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNPCvsMon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPCvsMon::Free()
{
	__super::Free();
}
