#include "NPC_Dummy.h"

_float CNPC_Dummy::m_fOffsetX = 75.f;

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
		m_NPCInfo = *(NPC_INFO*)pArg;

		m_strModelTag = m_NPCInfo.strNPCPrototype;
	}

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(/*75.f - */m_fOffsetX, 0.f, 90.f, 1.f));

#pragma region IDLE NPC

	if (m_strModelTag == TEXT("Prototype_Model_Dwarf_Male_002"))
	{
		m_Animation.iAnimIndex = 0;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_003"))
	{
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_004"))
	{
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_006"))
	{
		m_Animation.iAnimIndex = 4;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_013"))
	{
		m_Animation.iAnimIndex = 1;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_015"))
	{
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_018"))
	{
		m_Animation.iAnimIndex = 0;
	}

#pragma endregion IDLE NPC

#pragma region DANCE NPC

	if (m_strModelTag == TEXT("Prototype_Model_Female_013"))
	{
		m_Animation.iAnimIndex = 1;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_027"))
	{
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_Chi"))
	{
		m_Animation.iAnimIndex = 3;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_016"))
	{
		m_Animation.iAnimIndex = 0;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_020"))
	{
		m_Animation.iAnimIndex = 0;
	}

#pragma endregion DANCE NPC

#pragma region 기타 치는애

	if (m_strModelTag == TEXT("Prototype_Model_Male_027"))
	{
		m_Animation.iAnimIndex = 1;
	}

#pragma endregion 기타 치는애

	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 2.f;

	m_fOffsetX -= 2.f;

    return S_OK;
}

void CNPC_Dummy::Tick(_float fTimeDelta)
{	
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
