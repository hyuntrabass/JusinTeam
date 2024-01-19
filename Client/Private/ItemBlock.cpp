#include "ItemBlock.h"
#include "GameInstance.h"
#include "TextButton.h"

CItemBlock::CItemBlock(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CItemBlock::CItemBlock(const CItemBlock& rhs)
	: CGameObject(rhs)
{
}

HRESULT CItemBlock::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CItemBlock::Init(void* pArg)
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

void CItemBlock::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < ITEMSLOT_END; i++)
	{
		m_pSlots[i]->Tick(fTimeDelta);
	}
}

void CItemBlock::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < ITEMSLOT_END; i++)
	{
		m_pSlots[i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CItemBlock::Render()
{

	return S_OK;
}

HRESULT CItemBlock::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItemBlock::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CItemBlock::Add_Slots()
{
	for (size_t i = 0; i < ITEMSLOT_END; i++)
	{
		CItemSlot::ITEMSLOT_DESC ItemSlotDesc = {};
		ItemSlotDesc.vSize = _float2(60.f, 60.f);
		ItemSlotDesc.vPosition = _float2((_float)g_iWinSizeX / 2.f + 55.f + (ItemSlotDesc.vSize.x / 2.f) + (ItemSlotDesc.vSize.x * i) + (-15.f * i), 675.f);

		m_pSlots[i] = (CItemSlot*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SkillSlot"), &ItemSlotDesc);
		if (m_pSlots[i] == nullptr)
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

CItemBlock* CItemBlock::Create(_dev pDevice, _context pContext)
{
	CItemBlock* pInstance = new CItemBlock(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemBlock::Clone(void* pArg)
{
	CItemBlock* pInstance = new CItemBlock(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemBlock::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < ITEMSLOT_END; i++)
		{
			Safe_Release(m_pSlots[i]);
		}
	}
	Safe_Release(m_pRendererCom);

}
