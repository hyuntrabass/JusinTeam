#include "Roskva.h"

CRoskva::CRoskva(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CRoskva::CRoskva(const CRoskva& rhs)
	: CNPC(rhs)
{
}

HRESULT CRoskva::Init_Prototype()
{
	return S_OK;
}

HRESULT CRoskva::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Roskva");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(65.f, 0.f, 100.f, 1.f));

	m_Animation.iAnimIndex = IDLE01;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	return S_OK;
}

void CRoskva::Tick(_float fTimeDelta)
{
	if (false) // 나중에 조건 추가
	{
		m_bTalking = true;
	}

	if (m_bTalking == true)
	{
		m_Animation.iAnimIndex = TALK01_OLD;
	}
	else
	{
		m_Animation.iAnimIndex = IDLE01;
	}

	m_pModelCom->Set_Animation(m_Animation);
}

void CRoskva::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CRoskva::Render()
{
	__super::Render();

	return S_OK;
}

CRoskva* CRoskva::Create(_dev pDevice, _context pContext)
{
	CRoskva* pInstance = new CRoskva(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRoskva");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRoskva::Clone(void* pArg)
{
	CRoskva* pInstance = new CRoskva(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CRoskva");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRoskva::Free()
{
	__super::Free();
}
