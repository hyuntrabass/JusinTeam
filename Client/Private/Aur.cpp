#include "Aur.h"

CAur::CAur(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CAur::CAur(const CAur& rhs)
	: CNPC(rhs)
{
}

HRESULT CAur::Init_Prototype()
{
    return S_OK;
}

HRESULT CAur::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Aur");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(71.f, 8.f, 97.f, 1.f));

	m_Animation.iAnimIndex = HANGING01;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	return S_OK;
}

void CAur::Tick(_float fTimeDelta)
{
	if (false) // 나중에 조건 추가
	{
		m_bTalking = true;
	}

	if (m_bTalking == true)
	{
		m_Animation.iAnimIndex = TALK01;
	}

	m_pModelCom->Set_Animation(m_Animation);
}

void CAur::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CAur::Render()
{
	__super::Render();

    return S_OK;
}

CAur* CAur::Create(_dev pDevice, _context pContext)
{
	CAur* pInstance = new CAur(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CAur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAur::Clone(void* pArg)
{
	CAur* pInstance = new CAur(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CAur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAur::Free()
{
	__super::Free();
}
