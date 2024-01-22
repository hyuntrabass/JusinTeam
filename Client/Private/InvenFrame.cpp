#include "InvenFrame.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"

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
				break;
			}
		}


		for (size_t i = 0; i < m_vecItemSlot[m_eCurInvenType].size(); i++)
		{
			if (m_vecItems.empty())
			{
				return;
			}
			if (PtInRect(&dynamic_cast<CTextButtonColor*>(m_vecItems[m_vecItemSlot[m_eCurInvenType][i]])->Get_Rect(), ptMouse))
			{
				//아이템 선택시 로직  desc표시, 무기면 장착, 옷이어도 장착, 포션이면 아이템슬롯에 끼우기 등? 
				//왼클릭 설명 우클릭 장착? 
			}
		}
	}



	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	for (size_t i = 0; i < INVEN_TYPE::INVEN_END; i++)
	{
		m_pInvenType[i]->Tick(fTimeDelta);
	}
	m_pUnderBar->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);
}

void CInvenFrame::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	/*인벤토리 메뉴 렌더*/
	for (size_t i = 0; i < INVEN_TYPE::INVEN_END; i++)
	{
		m_pInvenType[i]->Late_Tick(fTimeDelta);
	}

	/*수직바 렌더*/
	for (auto& iter : m_pVerticalBar)
	{
		iter->Late_Tick(fTimeDelta);
	}

	/*아이템 렌더*/
	for (size_t i = 0; i < m_vecItemSlot[m_eCurInvenType].size(); i++)
	{
		if (m_vecItems.empty())
		{
			return;
		}
		m_vecItems[m_vecItemSlot[m_eCurInvenType][i]]->Late_Tick(fTimeDelta);
	}

	m_pUnderBar->Late_Tick(fTimeDelta);
	m_pSelectButton->Late_Tick(fTimeDelta);
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

	_vec2 vPos = dynamic_cast<CTextButtonColor*>(m_pInvenType[INVEN_ALL])->Get_Position();
	dynamic_cast<CTextButton*>(m_pSelectButton)->Set_Position(vPos);
	_vec2 fUnderBarPos = dynamic_cast<CTextButton*>(m_pUnderBar)->Get_Position();
	dynamic_cast<CTextButton*>(m_pUnderBar)->Set_Position(_vec2(vPos.x, fUnderBarPos.y));
}

HRESULT CInvenFrame::Add_Parts()
{
	_float fTerm = m_fSizeX / (_uint)INVEN_END;
	_float fStartX = m_fX - (m_fSizeX / 2.f) + (m_fSizeX / (_uint)INVEN_END) / 2.f ;
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
	CTextButton::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
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

void CInvenFrame::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < INVEN_TYPE::INVEN_END; i++)
		{
			Safe_Release(m_pInvenType[i]);
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
	Safe_Release(m_pSelectButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);


}