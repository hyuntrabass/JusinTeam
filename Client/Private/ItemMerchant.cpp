#include "ItemMerchant.h"

CItemMerchant::CItemMerchant(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CItemMerchant::CItemMerchant(const CItemMerchant& rhs)
	: CNPC(rhs)
{
}

HRESULT CItemMerchant::Init_Prototype()
{
    return S_OK;
}

HRESULT CItemMerchant::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_ItemMerchant");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(75.f, 0.f, 100.f, 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	return S_OK;
}

void CItemMerchant::Tick(_float fTimeDelta)
{
	if (false) // 나중에 조건 추가
	{
		m_bTalking = true;
	}

	if (m_bTalking == true)
	{
		m_Animation.iAnimIndex = TALK;
	}
	else
	{
		m_Animation.iAnimIndex = IDLE;
	}

	m_pModelCom->Set_Animation(m_Animation);
}

void CItemMerchant::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CItemMerchant::Render()
{
	__super::Render();

    return S_OK;
}

CItemMerchant* CItemMerchant::Create(_dev pDevice, _context pContext)
{
	CItemMerchant* pInstance = new CItemMerchant(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemMerchant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemMerchant::Clone(void* pArg)
{
	CItemMerchant* pInstance = new CItemMerchant(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemMerchant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemMerchant::Free()
{
	__super::Free();
}
