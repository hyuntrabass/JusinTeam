#include "SkillBlock.h"
#include "GameInstance.h"
#include "TextButton.h"

CSkillBlock::CSkillBlock(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkillBlock::CSkillBlock(const CSkillBlock& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkillBlock::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CSkillBlock::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Slots()))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CSkillBlock::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < SKILL_END; i++)
	{
		m_pSlots[i]->Tick(fTimeDelta);
	}
}

void CSkillBlock::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < SKILL_END; i++)
	{
		m_pSlots[i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CSkillBlock::Render()
{
	

	return S_OK;
}

HRESULT CSkillBlock::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CSkillBlock::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CSkillBlock::Add_Slots()
{
	for (size_t i = 0; i < SKILL_END; i++)
	{
		CSkillSlot::SKILLSLOT_DESC SkillSlotDesc = {};
		SkillSlotDesc.vSize = _float2(60.f, 60.f);
		SkillSlotDesc.vPosition = _float2((_float)g_iWinSizeX/2.f - 250.f + (SkillSlotDesc.vSize.x / 2.f) + (SkillSlotDesc.vSize.x * i) + (-15.f * i), 660.f);

		m_pSlots[i] = (CSkillSlot*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SkillSlot"), &SkillSlotDesc);
		if (m_pSlots[i] == nullptr)
		{
			return E_FAIL;
		}
	}
	//반대편 시작위치는  pos = (_float)g_iWinSizeX/2.f - 250.f + (SkillSlotDesc.vSize.x / 2.f) + (SkillSlotDesc.vSize.x * 3) + (-15.f * 3)
	//g_iWinSizeX/2 + (g_iWinSizeX/2 - pos) 


	return S_OK;
}

CSkillBlock* CSkillBlock::Create(_dev pDevice, _context pContext)
{
	CSkillBlock* pInstance = new CSkillBlock(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkillBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkillBlock::Clone(void* pArg)
{
	CSkillBlock* pInstance = new CSkillBlock(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkillBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillBlock::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < SKILL_END; i++)
		{
			Safe_Release(m_pSlots[i]);
		}
	}

	Safe_Release(m_pRendererCom);

}
