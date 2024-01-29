#include "Shop.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "InvenFrame.h"
CShop::CShop(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CShop::CShop(const CShop& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CShop::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CShop::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fSizeX = 70.f;
	m_fSizeY = 70.f;
	m_fX = 1160.f;
	m_fY = 45.f;
	m_fDepth = (_float)D_INVEN / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CShop::Tick(_float fTimeDelta)
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

	if (TRUE == PtInRect(&dynamic_cast<CTextButton*>(m_pExitButton)->Get_Rect(), ptMouse)
		|| PtInRect(&dynamic_cast<CTextButton*>(m_pTitleButton)->Get_Rect(), ptMouse))
	{
		if (m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			m_isActive = false;
		}
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (m_isActive)
	{
		/* 돈 */
		_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
		dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

		_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
		dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));


		CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
		m_pExitButton->Tick(fTimeDelta);
		m_pInvenFrame->Tick(fTimeDelta);
		m_pBackGround->Tick(fTimeDelta);
		m_pTitleButton->Tick(fTimeDelta);
	}

}

void CShop::Late_Tick(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pMoney->Late_Tick(fTimeDelta);
		m_pDiamond->Late_Tick(fTimeDelta);
		m_pSeigeLine->Late_Tick(fTimeDelta);
		m_pInvenFrame->Late_Tick(fTimeDelta);
		m_pExitButton->Late_Tick(fTimeDelta);
		m_pBackGround->Late_Tick(fTimeDelta);
		m_pTitleButton->Late_Tick(fTimeDelta);

	}


	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}


	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CShop::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}


	return S_OK;
}


void CShop::Open_Shop()
{
	if (!m_isActive)
	{
		CFadeBox::STATE eState = CFadeBox::FADEOUT;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FadeBox"), &eState)))
		{
			return;
		}
		if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
		{
			return;
		}
		m_isActive = true;
		Init_ShopState();
	}
}

void CShop::Init_ShopState()
{
	dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Set_FrameMode(CInvenFrame::F_SHOP);
	dynamic_cast<CInvenFrame*>(m_pInvenFrame)->Init_State();

	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	dynamic_cast<CTextButton*>(m_pMoney)->Set_Text(to_wstring(iMoney));

	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	dynamic_cast<CTextButton*>(m_pDiamond)->Set_Text(to_wstring(iDiamond));
}


HRESULT CShop::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.fFontSize = 0.4f;
	Button.strText = TEXT("소모품 상점");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	Button.vPosition = _vec2(20.f, 20.f);
	Button.vSize = _vec2(50.f, 50.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(70.f, 0.f);

	m_pTitleButton =m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pTitleButton)
	{
		return E_FAIL;
	}
	
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Out");
	Button.vPosition = _vec2(1230.f, 30.f);
	Button.vSize = _vec2(70.f, 70.f);

	m_pExitButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pExitButton)
	{
		return E_FAIL;
	}
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SiegeLine");
	Button.vPosition = _vec2(102.f, 350.f);
	Button.vSize = _vec2(6.f, 360.f);

	m_pSeigeLine = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pSeigeLine)
	{
		return E_FAIL;
	}

	
	_uint iMoney = CUI_Manager::Get_Instance()->Get_Coin();;
	Button.strText = to_wstring(iMoney);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_coin");
	Button.vPosition = _vec2(1100.f, 30.f);
	Button.vSize = _vec2(25.f, 25.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(Button.vSize.x + 10.f, Button.vSize.y - 26.f);

	m_pMoney = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pMoney)
	{
		return E_FAIL;
	}
	_uint iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();;
	Button.strText = to_wstring(iDiamond);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Diamond");
	Button.vPosition = _vec2(1010.f, 30.f);
	Button.vSize = _vec2(25.f, 25.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(Button.vSize.x + 10.f, Button.vSize.y - 26.f);

	m_pDiamond = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pDiamond)
	{
		return E_FAIL;
	}

	UiInfo info{};
	info.strTexture = TEXT("Prototype_Component_Texture_BackGround_Mask");
	info.vPos = _vec2((_float)g_iWinSizeX/2.f, (_float)g_iWinSizeY / 2.f);
	info.vSize = _vec2((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	info.iLevel = (_uint)LEVEL_CUSTOM;

	m_pBackGround = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BackGround_Mask"), &info);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}



	m_pInvenFrame = CUI_Manager::Get_Instance()->Get_InvenFrame();
	if (not m_pInvenFrame)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CShop::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_InvenBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShop::Bind_ShaderResources()
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

	return S_OK;
}

CShop* CShop::Create(_dev pDevice, _context pContext)
{
	CShop* pInstance = new CShop(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShop::Clone(void* pArg)
{
	CShop* pInstance = new CShop(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CShop");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShop::Free()
{
	__super::Free();



	Safe_Release(m_pMoney);
	Safe_Release(m_pDiamond);
	Safe_Release(m_pSeigeLine);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pTitleButton);
	Safe_Release(m_pInvenFrame);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}