#include "InvenFrame.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "ItemBlock.h"

CInvenFrame::CInvenFrame(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CInvenFrame::CInvenFrame(const CInvenFrame& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CInvenFrame::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CInvenFrame::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	m_pParent = ((INVENFRAME_DESC*)pArg)->pParent;
	m_fSizeX = ((INVENFRAME_DESC*)pArg)->vSize.x;
	m_fSizeY = ((INVENFRAME_DESC*)pArg)->vSize.y;

	m_fX = ((INVENFRAME_DESC*)pArg)->vPosition.x;
	m_fY = ((INVENFRAME_DESC*)pArg)->vPosition.y;

	m_fDepth = ((INVENFRAME_DESC*)pArg)->fDepth;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CInvenFrame::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT rectUI = {
			  (LONG)(m_fX - m_fSizeX * 0.5f),
			  (LONG)(m_fY - m_fSizeY * 0.5f),
			  (LONG)(m_fX + m_fSizeX * 0.5f),
			  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		/* 인벤토리 메뉴 피킹 */
		for (size_t i = 0; i < INVEN_TYPE::INVEN_END; i++)
		{
			if (PtInRect(&dynamic_cast<CTextButtonColor*>(m_pInvenType[i])->Get_Rect(), ptMouse))
			{
				m_ePrevInvenType = m_eCurInvenType;
				m_eCurInvenType = (INVEN_TYPE)i;
				if (m_ePrevInvenType != m_eCurInvenType)
				{
					dynamic_cast<CTextButtonColor*>(m_pInvenType[m_ePrevInvenType])->Set_Alpha(0.6f);
				}
				dynamic_cast<CTextButtonColor*>(m_pInvenType[i])->Set_Alpha(1.f);
				_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pInvenType[i])->Get_Position();
				dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
				_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
				dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));

				Set_ItemPosition(m_eCurInvenType);
				break;
			}
		}
		switch (m_eFrameMode)
		{
		case F_INVEN:
			Picking_InvenButton(ptMouse);
			break;
		case F_SHOP:
			Picking_ShopButton(ptMouse);
			break;
		}
		
		
	}



	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	for (size_t i = 0; i < m_vecItemsSlot[m_eCurInvenType].size(); i++)
	{
		m_vecItemsSlot[m_eCurInvenType][i]->Tick(fTimeDelta);
	}


	for (size_t i = 0; i < INVEN_TYPE::INVEN_END; i++)
	{
		m_pInvenType[i]->Tick(fTimeDelta);
	}
	m_pUnderBar->Tick(fTimeDelta);
	m_pBackGround->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);

	switch (m_eFrameMode)
	{
	case F_INVEN:
		Inven_Tick(fTimeDelta, ptMouse);
		break;
	case F_SHOP:
		Shop_Tick(fTimeDelta, ptMouse);
		break;
	}
	
}

void CInvenFrame::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	/*인벤토리 메뉴 렌더*/
	for (size_t i = 0; i < INVEN_TYPE::INVEN_END; i++)
	{
		m_pInvenType[i]->Late_Tick(fTimeDelta);
	}

	m_pUnderBar->Late_Tick(fTimeDelta);
	m_pBackGround->Late_Tick(fTimeDelta);
	m_pSelectButton->Late_Tick(fTimeDelta);


	switch (m_eFrameMode)
	{
	case F_INVEN:
	{
		if (!m_isActiveQuickSlot)
		{
			m_pSlotSettingButton->Late_Tick(fTimeDelta);
			m_pWearableClearButton->Late_Tick(fTimeDelta);

		}
		if (m_isActiveQuickSlot)
		{
			m_pResetSlot->Late_Tick(fTimeDelta);
			m_pResetSymbol->Late_Tick(fTimeDelta);
			m_pExitSlotSetting->Late_Tick(fTimeDelta);
			for (_uint i = 0; i < 4; i++)
			{
				m_pSelectSlot[i]->Late_Tick(fTimeDelta);
			}
		}

	}
	break;
	case F_SHOP:
	{
		m_pSellButton->Late_Tick(fTimeDelta);
	}
	break;
	}

	
	/*인벤 메뉴 사이 수직바 렌더*/
	for (auto& iter : m_pVerticalBar)
	{
		iter->Late_Tick(fTimeDelta);
	}

	/*아이템 렌더 */

	for (size_t i = 0; i < m_vecItemsSlot[m_eCurInvenType].size(); i++)
	{
		m_vecItemsSlot[m_eCurInvenType][i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CInvenFrame::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI_Alpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}


	return S_OK;
}


void CInvenFrame::Init_State()
{
	dynamic_cast<CTextButtonColor*>(m_pInvenType[INVEN_ALL])->Set_Alpha(1.f);
	m_eCurInvenType = INVEN_ALL;
	Set_ItemPosition(m_eCurInvenType);

	_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pInvenType[INVEN_ALL])->Get_Position();
	dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
	_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
	dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));
}

HRESULT CInvenFrame::Add_Parts()
{
	_float fTerm = m_fSizeX / (_uint)INVEN_END;
	_float fStartX = m_fX - (m_fSizeX / 2.f) + (m_fSizeX / (_uint)INVEN_END) / 2.f;
	_float fY = 85.f;

	CTextButtonColor::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenmenu1");
	Button.vPosition = _vec2(fStartX + fTerm * (_uint)INVEN_ALL, fY);
	Button.vSize = _vec2(30.f, 30.f);
	Button.fAlpha = 0.6f;

	m_pInvenType[INVEN_ALL] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pInvenType[INVEN_ALL])
	{
		return E_FAIL;
	}

	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenmenu2");
	Button.vPosition = _vec2(fStartX + fTerm * (_uint)INVEN_WEARABLE, fY);

	m_pInvenType[INVEN_WEARABLE] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pInvenType[INVEN_WEARABLE])
	{
		return E_FAIL;
	}

	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenmenu5");
	Button.vPosition = _vec2(fStartX + fTerm * (_uint)INVEN_EQUIP, fY);

	m_pInvenType[INVEN_EQUIP] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pInvenType[INVEN_EQUIP])
	{
		return E_FAIL;
	}

	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenmenu4");
	Button.vPosition = _vec2(fStartX + fTerm * (_uint)INVEN_EXPENDABLES, fY);

	m_pInvenType[INVEN_EXPENDABLES] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pInvenType[INVEN_EXPENDABLES])
	{
		return E_FAIL;
	}


	for (size_t i = 0; i < INVEN_TYPE::INVEN_END; i++)
	{
		dynamic_cast<CTextButtonColor*>(m_pInvenType[(_uint)i])->Set_Pass(VTPass_UI_Alpha);
	}


	/* 여기까지 인벤 메뉴*/


	Button.fDepth = m_fDepth - 0.02f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Select_BG_BoxEfc_WhiteBlur");
	Button.vSize = _vec2(m_fSizeX, 50.f);
	Button.vColor = _vec4(0.2f, 0.2f, 0.2f, 0.6f);
	Button.fAlpha = 0.6f;
	_float fBgY = m_fY + m_fSizeY / 2.f - Button.vSize.y / 2.f;
	Button.vPosition = _vec2(m_fX, fBgY);
	m_pBackGround = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}

	_float fButtonStartX = m_fX - (m_fSizeX / 2.f) + (m_fSizeX / 2.f) / 2.f;
	Button.fDepth = m_fDepth - 0.03f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	Button.fFontSize = 0.35f;
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(0.f, 0.f);
	Button.vSize = _vec2(150.f, 100.f);
	Button.fAlpha = 0.8f;
	Button.strText = TEXT("퀵슬롯 설정");
	Button.vPosition = _vec2(fButtonStartX, fBgY);
	m_pSlotSettingButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pSlotSettingButton)
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pSlotSettingButton)->Set_Pass(VTPass_UI_Alpha);

	Button.strText = TEXT("장비 모두 해제");
	Button.vPosition = _vec2(fButtonStartX + m_fSizeX / 2.f, fBgY);
	m_pWearableClearButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pWearableClearButton)
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pWearableClearButton)->Set_Pass(VTPass_UI_Alpha);

	Button.strText = TEXT("아이템 판매");
	Button.vPosition = _vec2(fButtonStartX + m_fSizeX / 2.f, fBgY);
	m_pSellButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pSellButton)
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pSellButton)->Set_Pass(VTPass_UI_Alpha);


	/* 슬롯 열었을때 */
	Button.fDepth = m_fDepth - 0.03f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	Button.fFontSize = 0.35f;
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(0.f, 0.f);
	Button.vColor = _vec4(0.2f, 0.2f, 0.2f, 0.8f);
	Button.vSize = _vec2(150.f, 100.f);
	Button.fAlpha = 0.8f;
	Button.strText = TEXT("설정 닫기");
	Button.vPosition = _vec2(fButtonStartX + m_fSizeX / 2.f, fBgY);
	m_pExitSlotSetting = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pExitSlotSetting)
	{
		return E_FAIL;
	}
	m_pExitSlotSetting->Set_Pass(VTPass_UI_Alpha);

	Button.fDepth = m_fDepth - 0.03f;
	Button.vColor = _vec4(0.6f, 0.f, 0.f, 0.8f);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_buttonRed");
	Button.strText = TEXT("  초기화");
	Button.vPosition = _vec2(fButtonStartX, fBgY);
	m_pResetSlot = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &Button);
	if (not m_pResetSlot)
	{
		return E_FAIL;
	}
	m_pResetSlot->Set_Pass(VTPass_UI_Alpha);


	for (_uint i = 0; i < 4; i++)
	{
		CItemSlot::ITEMSLOT_DESC ItemSlotDesc = {};
		ItemSlotDesc.eSlotMode = CItemSlot::ITSLOT_INVEN;
		ItemSlotDesc.vSize = _vec2(60.f, 60.f);
		ItemSlotDesc.fDepth = m_fDepth - 0.03f;
		ItemSlotDesc.vPosition = _vec2(fStartX + fTerm * i, fBgY - 62.5f);
		m_pSelectSlot[i] = (CItemSlot*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemSlot"), &ItemSlotDesc);
		if (m_pSelectSlot[i] == nullptr)
		{
			return E_FAIL;
		}
		if (FAILED(CUI_Manager::Get_Instance()->Set_InvenItemSlots((CItemBlock::ITEMSLOT)i, m_pSelectSlot[i])))
		{
			return E_FAIL;
		}
	}

	CTextButton::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_InvenRe");
	TextButton.fDepth = m_fDepth - 0.03f;
	TextButton.strText = TEXT("");
	TextButton.vSize = _vec2(25.f, 25.f);
	TextButton.vPosition = _vec2(m_fX - m_fSizeX / 2.f + 40.f, fBgY);
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.fFontSize = 0.1f;
	m_pResetSymbol = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &TextButton);
	if (not m_pResetSymbol)
	{
		return E_FAIL;
	}



	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.strText = TEXT("");
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_InvenUnderBar");
	TextButton.vSize = _vec2(90.f, 90.f);
	TextButton.vPosition = _vec2(fStartX + fTerm * INVEN_ALL, fY + 12.f);
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.fFontSize = 0.1f;
	m_pUnderBar = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &TextButton);
	if (not m_pUnderBar)
	{
		return E_FAIL;
	}

	TextButton.fDepth = m_fDepth - 0.02f;
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SelecInvenMenu");
	TextButton.vSize = _vec2(fTerm, 34.f);
	TextButton.vPosition = _vec2(fStartX + fTerm * INVEN_ALL, fY);
	m_pSelectButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &TextButton);
	if (not m_pSelectButton)
	{
		return E_FAIL;
	}


	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_VerticalBar");
	TextButton.vSize = _vec2(16.f, 16.f);
	fStartX += (m_fSizeX / (_uint)INVEN_END) / 2.f;

	for (_uint i = 0; i < 3; i++)
	{
		TextButton.vPosition = _vec2(fStartX + fTerm * i, fY);
		CGameObject* pGameObject = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &TextButton);
		if (not pGameObject)
		{
			return E_FAIL;
		}
		m_pVerticalBar.push_back(pGameObject);
	}
	
	//CItemSlot* pItemSlot = (CItemSlot*)CUI_Manager::Get_Instance()->Get_ItemSlots((CItemBlock::ITEMSLOT)0);
	return S_OK;
}

HRESULT CInvenFrame::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_invenframe"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInvenFrame::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}
	_float fAlpha = 0.8f;
	if(FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof _float)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CInvenFrame* CInvenFrame::Create(_dev pDevice, _context pContext)
{
	CInvenFrame* pInstance = new CInvenFrame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CInvenFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInvenFrame::Clone(void* pArg)
{
	CInvenFrame* pInstance = new CInvenFrame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CInvenFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInvenFrame::Set_Item(ITEM eItem, _uint iNum)
{
	_bool isItemExist = false;
	for (auto& iter : m_vecItems)
	{
		if (eItem.strName == iter->Get_ItemDesc().strName)
		{
			iter->Set_ItemNum(iNum);
			isItemExist = true;
			return;
		}
	}

	if (!isItemExist)
	{
		CItem::ITEM_DESC ItemDesc{};
		ItemDesc.bCanInteract = true;
		ItemDesc.eItemDesc = eItem;
		ItemDesc.iNum = iNum;
		ItemDesc.fDepth = m_fDepth - 0.05f;
		ItemDesc.vSize = _vec2(50.f, 50.f);
		ItemDesc.vPosition = _vec2(-50.f, -50.f);
		CItem* pItem = (CItem*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &ItemDesc);
		if (pItem == nullptr)
		{
			return;
		}
		m_vecItems.push_back(pItem);
	}

	m_vecItemsSlot[INVEN_ALL].push_back(m_vecItems.back());
	m_vecItemsSlot[(INVEN_TYPE)eItem.iInvenType].push_back(m_vecItems.back());
}

void CInvenFrame::ItemSlot_Logic(_uint iSlotIdx, _uint iIndex)
{
	if (!m_pSelectSlot[iSlotIdx]->Is_Full())
	{
		_int iNum = 0;
		HRESULT hr = CUI_Manager::Get_Instance()->Set_Item_In_EmptySlot((CItemBlock::ITEMSLOT)iSlotIdx, m_vecItemsSlot[m_eCurInvenType][iIndex], &iNum);
		if (FAILED(hr))
		{
			return;
		}
		if (iNum > 0)
		{
			m_vecItemsSlot[m_eCurInvenType][iIndex]->Set_ItemNum(-iNum);
		}
		else
		{
			Delete_Item(m_eCurInvenType, iIndex);
		}
	}
	else
	{	//m_iItemNum이 0보다 클경우 그 수만큼 빼주면되고, 0이면 다없애면되고 -1이면 없애면안됨
		_int iNum = 0;
		_int ChangeItemNum = 0;
		ITEM eItem = CUI_Manager::Get_Instance()->Set_Item_In_FullSlot((CItemBlock::ITEMSLOT)iSlotIdx, m_vecItemsSlot[m_eCurInvenType][iIndex], &iNum, &ChangeItemNum);
		if (eItem.iInvenType != -1)
		{
			if (iNum > 0)
			{
				m_vecItemsSlot[m_eCurInvenType][iIndex]->Set_ItemNum(-iNum);
			}
			else
			{
				Delete_Item(m_eCurInvenType, iIndex);
			}
			Set_Item(eItem, ChangeItemNum);
		}
		else
		{
			if (iNum > 0)
			{
				m_vecItemsSlot[m_eCurInvenType][iIndex]->Set_ItemNum(-iNum);
			}
			else if (iNum == -1)
			{
				return;
			}
			else
			{
				Delete_Item(m_eCurInvenType, iIndex);
			}
			Set_ItemPosition(m_eCurInvenType);
		}
	}

}

void CInvenFrame::ItemSlot_Delete_Logic(_uint iSlotIdx)
{
	_bool isItemExist = false;
	_uint iItemNum = m_pSelectSlot[iSlotIdx]->Get_ItemObject()->Get_ItemNum();

	for (auto& iter : m_vecItems)
	{
		if (m_pSelectSlot[iSlotIdx]->Get_ItemName() == iter->Get_ItemDesc().strName)
		{
			iter->Set_ItemNum(iItemNum);
			CUI_Manager::Get_Instance()->Delete_Item_In_Slot((CItemBlock::ITEMSLOT)iSlotIdx);
			isItemExist = true;
			return;
		}
	}
	if (!isItemExist)
	{
		Set_Item(m_pSelectSlot[iSlotIdx]->Get_ItemObject()->Get_ItemDesc(), iItemNum);

		CUI_Manager::Get_Instance()->Delete_Item_In_Slot((CItemBlock::ITEMSLOT)iSlotIdx);
	}
	Set_ItemPosition(m_eCurInvenType);
	return;
}

void CInvenFrame::Delete_Item(INVEN_TYPE eInvenType, _uint iIndex)
{
	INVEN_TYPE eOriginInvenType = (INVEN_TYPE)m_vecItemsSlot[eInvenType][iIndex]->Get_ItemDesc().iInvenType;
	wstring strItemName = m_vecItemsSlot[eInvenType][iIndex]->Get_ItemDesc().strName;

	for (size_t i = 0; i < m_vecItemsSlot[eOriginInvenType].size(); i++)
	{
		if (m_vecItemsSlot[eOriginInvenType][i]->Get_ItemDesc().strName == strItemName)
		{
			m_vecItemsSlot[eOriginInvenType].erase(m_vecItemsSlot[eOriginInvenType].begin() + i);
			break;
		}
	}
	for (size_t i = 0; i < m_vecItemsSlot[INVEN_ALL].size(); i++)
	{
		if (m_vecItemsSlot[INVEN_ALL][i]->Get_ItemDesc().strName == strItemName)
		{
			m_vecItemsSlot[INVEN_ALL].erase(m_vecItemsSlot[INVEN_ALL].begin() + i);
			break;
		}
	}

	for (size_t i = 0; i < m_vecItems.size(); i++)
	{
		if (m_vecItems[i]->Get_ItemDesc().strName == strItemName)
		{
			Safe_Release(m_vecItems[i]);
			m_vecItems.erase(m_vecItems.begin() + i);
			break;
		}
	}
	Set_ItemPosition(m_eCurInvenType);
}

void CInvenFrame::Picking_InvenButton(POINT ptMouse)
{
	if (!m_isPicking)
	{
		for (_uint j = 0; j < 4; j++)
		{
			if (PtInRect(&m_pSelectSlot[j]->Get_Rect(), ptMouse))
			{
				if (m_pSelectSlot[j]->Is_Full())
				{
					ItemSlot_Delete_Logic(j);
					break;
				}
			}
		}
	}

	if (m_isPicking)
	{
		_bool isPicking = false;
		for (_uint j = 0; j < 4; j++)
		{
			if (PtInRect(&m_pSelectSlot[j]->Get_Rect(), ptMouse))
			{
				isPicking = true;
				ItemSlot_Logic(j, m_iCurIndex);
				break;
			}
		}
		m_isPicking = false;
	}

	if (m_isActiveQuickSlot)
	{
		for (size_t i = 0; i < m_vecItemsSlot[m_eCurInvenType].size(); i++)
		{
			if (PtInRect(&m_vecItemsSlot[m_eCurInvenType][i]->Get_Rect(), ptMouse))
			{
				if (m_vecItemsSlot[m_eCurInvenType][i]->Get_ItemDesc().iItemType == (_uint)ITEM_POTION)
				{
					m_isPicking = true;
					m_iCurIndex = i;
					break;
				}
			}
			//이건 아이템 타입에 따라 달라질듯
		}
	}
}

void CInvenFrame::Picking_ShopButton(POINT ptMouse)
{
	
}

void CInvenFrame::Inven_Tick(_float fTimeDelta, POINT ptMouse)
{


	if (m_isQuickAnim)
	{
		if (m_pSlotSettingButton->Get_TransPosition().y + 10.f < m_fY + m_fSizeY / 2.f)
		{
			m_pSlotSettingButton->Set_Position(_vec2(m_pSlotSettingButton->Get_Position().x, m_pSlotSettingButton->Get_TransPosition().y + fTimeDelta * 50.f));
			m_pWearableClearButton->Set_Position(_vec2(m_pWearableClearButton->Get_Position().x, m_pWearableClearButton->Get_TransPosition().y + fTimeDelta * 50.f));

		}
		m_pBackGround->Set_Position(_vec2(m_pBackGround->Get_Position().x, m_pBackGround->Get_TransPosition().y - fTimeDelta * 200.f));
		m_pBackGround->Set_Size(m_fSizeX, m_pBackGround->Get_Size().y + (fTimeDelta * 200.f) * 2.f);
		if (m_pBackGround->Get_Position().y - m_pBackGround->Get_TransPosition().y >= 40.f)
		{
			m_isQuickAnim = false;
			m_isActiveQuickSlot = true;
		}

	}

	if (PtInRect(&m_pSlotSettingButton->Get_InitialRect(), ptMouse))
	{
		m_pSlotSettingButton->Set_Size(140.f, 80.f, 0.3f);
		if (!m_isActiveQuickSlot && !m_isQuickAnim && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Engine))
		{
			m_isQuickAnim = true;
		}
	}
	else
	{
		m_pSlotSettingButton->Set_Size(150.f, 100.f, 0.35f);
	}



	if (!m_isActiveQuickSlot)
	{
		m_pSlotSettingButton->Tick(fTimeDelta);
		m_pWearableClearButton->Tick(fTimeDelta);

	}
	if (m_isActiveQuickSlot)
	{
		m_pResetSlot->Tick(fTimeDelta);
		m_pResetSymbol->Tick(fTimeDelta);
		m_pExitSlotSetting->Tick(fTimeDelta);
		if (PtInRect(&m_pExitSlotSetting->Get_InitialRect(), ptMouse))
		{
			m_pExitSlotSetting->Set_Size(140.f, 80.f, 0.3f);
			if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Engine))
			{
				m_isActiveQuickSlot = false;
				m_pBackGround->Set_Size(m_fSizeX, 50.f);
				m_pBackGround->Set_Position(_vec2(m_pBackGround->Get_Position().x, m_pBackGround->Get_Position().y));
				m_pSlotSettingButton->Set_Position(_vec2(m_pSlotSettingButton->Get_Position().x, m_pSlotSettingButton->Get_Position().y));
				m_pWearableClearButton->Set_Position(_vec2(m_pWearableClearButton->Get_Position().x, m_pWearableClearButton->Get_Position().y));

			}
		}
		else
		{
			m_pExitSlotSetting->Set_Size(150.f, 100.f, 0.35f);
		}
		if (PtInRect(&m_pResetSlot->Get_InitialRect(), ptMouse))
		{
			m_pResetSlot->Set_Size(140.f, 80.f, 0.3f);
			dynamic_cast<CTextButton*>(m_pResetSymbol)->Set_Size(20.f, 20.f);
			if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Engine) && !m_isPicking)
			{
				for (_uint i = 0; i < 4; i++)
				{
					if (m_pSelectSlot[i]->Is_Full())
					{
						ItemSlot_Delete_Logic(i);
					}
				}
			}
		}
		else
		{
			m_pResetSlot->Set_Size(150.f, 100.f, 0.35f);
			dynamic_cast<CTextButton*>(m_pResetSymbol)->Set_Size(25.f, 25.f);
		}

		for (_uint i = 0; i < 4; i++)
		{
			m_pSelectSlot[i]->Tick(fTimeDelta);
		}
	}
}

void CInvenFrame::Shop_Tick(_float fTimeDelta, POINT ptMouse)
{

	if (PtInRect(&m_pSellButton->Get_InitialRect(), ptMouse))
	{
		m_pSellButton->Set_Size(140.f, 80.f, 0.3f);
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Engine))
		{
		
		}
	}
	else
	{
		m_pSellButton->Set_Size(150.f, 100.f, 0.35f);
	}

	m_pSellButton->Tick(fTimeDelta);
}

void CInvenFrame::Set_ItemPosition(INVEN_TYPE eInvenType)
{

	_float fSize = 50.f;
	_float fTerm = m_fSizeX / 5.f;
	_float fStartX = m_fX - (m_fSizeX / 2.f) + (m_fSizeX / 5.f) / 2.f;
	_float fStartY = m_fY - m_fSizeY / 2.f + fSize / 2.f + 5.f;
	for (_uint i = 0; i < m_vecItemsSlot[eInvenType].size(); i++)
	{
		_uint iTermY = i / 5;
		_vec2 vPos = _vec2(fStartX + fTerm * i, fStartY + fSize * iTermY + 5.f * iTermY);
		m_vecItemsSlot[eInvenType][i]->Set_Position(vPos);
	}
}

void CInvenFrame::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < INVEN_TYPE::INVEN_END; i++)
		{
			Safe_Release(m_pInvenType[i]);
		}

		for (_uint i = 0; i < 4; i++)
		{
			Safe_Release(m_pSelectSlot[i]);
		}
	}

	for (auto& iter : m_pVerticalBar)
	{
		Safe_Release(iter);
	}
	for (auto& iter : m_vecItems)
	{
		Safe_Release(iter);
	}

	Safe_Release(m_pUnderBar);
	Safe_Release(m_pResetSlot);
	Safe_Release(m_pResetSymbol);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pSelectButton);
	Safe_Release(m_pExitSlotSetting);
	Safe_Release(m_pSlotSettingButton);
	Safe_Release(m_pWearableClearButton);


	/* shop 모드 */
	Safe_Release(m_pSellButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);


}
