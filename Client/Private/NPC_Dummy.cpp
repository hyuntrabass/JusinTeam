#include "NPC_Dummy.h"

CNPC_Dummy::CNPC_Dummy(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CNPC_Dummy::CNPC_Dummy(const CNPC_Dummy& rhs)
	: CNPC(rhs)
{
}

HRESULT CNPC_Dummy::Init_Prototype()
{
    return S_OK;
}

HRESULT CNPC_Dummy::Init(void* pArg)
{
	if (pArg)
	{
		m_eNPCType = *(NPC_TYPE*)pArg;

		switch (m_eNPCType)
		{
		case ITEM_MERCHANT:
			m_strModelTag = TEXT("Prototype_Model_Item_Merchant");
			break;
		case SKILL_MERCHANT:
			m_strModelTag = TEXT("Prototype_Model_Skill_Merchant");
			break;
		case GROAR:
			m_strModelTag = TEXT("Prototype_Model_Groar");
			break;
		}
	}

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = true;

	if (m_eNPCType == GROAR)
	{
		m_Animation.iAnimIndex = 1;
	}

    return S_OK;
}

void CNPC_Dummy::Tick(_float fTimeDelta)
{
	switch (m_eNPCType)
	{
	case Client::ITEM_MERCHANT:
		break;
	case Client::SKILL_MERCHANT:
		break;
	case Client::GROAR:
		break;
	}
	
	m_pModelCom->Set_Animation(m_Animation);
}

void CNPC_Dummy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CNPC_Dummy::Render()
{
	__super::Render();

    return S_OK;
}

CNPC_Dummy* CNPC_Dummy::Create(_dev pDevice, _context pContext)
{
	CNPC_Dummy* pInstance = new CNPC_Dummy(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNPC_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Dummy::Clone(void* pArg)
{
	CNPC_Dummy* pInstance = new CNPC_Dummy(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNPC_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Dummy::Free()
{
	__super::Free();
}
