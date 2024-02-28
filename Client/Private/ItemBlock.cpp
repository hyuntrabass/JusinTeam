#include "ItemBlock.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "Event_Manager.h"

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

	if (m_pGameInstance->Key_Down(DIK_Q))
	{
		m_iSelect++;
		_uint iIndex = m_iSelect % 4;
		m_pSelect->Set_Position(m_pSlots[(ITEMSLOT)iIndex]->Get_Position());
	}
	if (m_pGameInstance->Key_Down(DIK_R))
	{
		_uint iIndex = m_iSelect % 4;
		if (!m_pSlots[(ITEMSLOT)iIndex]->Is_Full())
		{
			CEvent_Manager::Get_Instance()->Set_Alert(TEXT("장착된 아이템이 없습니다."));
		}
		else
		{
			m_isCoolTime = true;
			m_fTime = 0.f;
			m_pMask->Set_Position(m_pSlots[(ITEMSLOT)iIndex]->Get_Position());
			if (m_pSlots[(ITEMSLOT)iIndex]->Get_ItemUsage() == IT_HPPOTION)
			{
				ITEM eItem = m_pSlots[(ITEMSLOT)iIndex]->Get_ItemObject()->Get_ItemDesc();
				CUI_Manager::Get_Instance()->Use_Item_In_Slot((ITEMSLOT)iIndex);
				CUI_Manager::Get_Instance()->Set_Heal(true, eItem.iStatus);
				m_pGameInstance->Play_Sound(L"Potion");
			}
			else if (m_pSlots[(ITEMSLOT)iIndex]->Get_ItemUsage() == IT_MPPOTION)
			{
				ITEM eItem = m_pSlots[(ITEMSLOT)iIndex]->Get_ItemObject()->Get_ItemDesc();
				CUI_Manager::Get_Instance()->Use_Item_In_Slot((ITEMSLOT)iIndex);
				CUI_Manager::Get_Instance()->Set_MpState(true, eItem.iStatus);
				m_pGameInstance->Play_Sound(L"Potion");
			}
		}
	}
	if (m_isCoolTime)
	{
		if (m_fTime >= 1.f)
		{
			m_isCoolTime = false;
			return;
		}
		m_fTime += fTimeDelta;
		m_pMask->Set_Factor(m_fTime);
		m_pMask->Tick(fTimeDelta);
	}
	m_pSelect->Tick(fTimeDelta);
}

void CItemBlock::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}	

	if (m_isCoolTime)
	{
		m_pMask->Late_Tick(fTimeDelta);
	}
	for (size_t i = 0; i < ITEMSLOT_END; i++)
	{
		m_pSlots[i]->Late_Tick(fTimeDelta);
	}
	m_pSelect->Late_Tick(fTimeDelta);
}

HRESULT CItemBlock::Render()
{
	if (m_isCoolTime)
	{
		wstring fTime = to_wstring(1.f - m_fTime);
		size_t dotPos = fTime.find(L'.');
		if (dotPos != wstring::npos && fTime.length() > dotPos + 2) {
			fTime.erase(dotPos + 2);
		}
		m_pGameInstance->Render_Text(L"Font_Dialogue", fTime, _vec2(m_pMask->Get_Position().x, m_pMask->Get_Position().y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	}
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

		ItemSlotDesc.eSlotMode = CItemSlot::ITSLOT_SCREEN;
		ItemSlotDesc.vSize = _float2(60.f, 60.f);
		ItemSlotDesc.vPosition = _float2((_float)g_iWinSizeX / 2.f + 55.f + (ItemSlotDesc.vSize.x / 2.f) + (ItemSlotDesc.vSize.x * i) + (-15.f * i), 675.f);
		ItemSlotDesc.fDepth = (_float)D_SCREEN / (_float)D_END - 0.01f;
		m_pSlots[i] = (CItemSlot*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemSlot"), &ItemSlotDesc);
		if (m_pSlots[i] == nullptr)
		{
			return E_FAIL;
		}
		if (FAILED(CUI_Manager::Get_Instance()->Set_ItemSlots((ITEMSLOT)i, m_pSlots[i])))
		{
			return E_FAIL; //인벤보다 순서 먼저
		}
	}

	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_STATIC;
	ButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END - 0.06f;
	ButtonDesc.fFontSize = 0.45f;
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomRect");
	ButtonDesc.vPosition = m_pSlots[ITEM1]->Get_Position();
	ButtonDesc.vSize = _vec2(60.f, 60.f);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 12.f);
	m_pSelect = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pSelect)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END - 0.05f;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.strText = TEXT("");
	//ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_Effect_FX_B_Gradient030_Tex");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_FadeBox");
	ColButtonDesc.strTexture2 = TEXT("Prototype_Component_Texture_Effect_FX_A_Gradation006_Tex");
	ColButtonDesc.vSize = _vec2(40.f, 40.f);
	ColButtonDesc.vPosition = m_pSlots[ITEM1]->Get_Position();
	ColButtonDesc.vColor = _vec4(0.f, 0.f, 0.f, 0.5f);

	m_pMask = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pMask)
	{
		return E_FAIL;
	}
	m_pMask->Set_Pass(VTPass_Dissolve);
	m_pMask->Set_Factor(0.f);
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
	Safe_Release(m_pMask);
	Safe_Release(m_pSelect);
	Safe_Release(m_pRendererCom);

}
