#include "Menu.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "Camera_Manager.h"

CMenu::CMenu(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CMenu::CMenu(const CMenu& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CMenu::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CMenu::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fSizeX = 70.f;
	m_fSizeY = 70.f;

	m_fX = 1220.f;
	m_fY = 44.f;


	m_fDepth = (_float)D_MENU / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CMenu::Tick(_float fTimeDelta)
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
	if (!m_isActive && CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	if ((TRUE == PtInRect(&rectUI, ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI)) || m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		if (!m_isActive)
		{
			CFadeBox::FADE_DESC Desc = {};
			Desc.fIn_Duration = 0.5f;
			Desc.fOut_Duration = 1.f;
			Desc.phasFadeCompleted = &m_isReadytoActivate;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);
			return;
		}
	}
	if (m_isReadytoActivate)
	{
		Init_State();
	}

	if (!m_isActive)
	{
		return;
	}

	if (TRUE == PtInRect(&dynamic_cast<CTextButton*>(m_pExitButton)->Get_Rect(), ptMouse)
		|| PtInRect(&dynamic_cast<CTextButton*>(m_pTitleButton)->Get_Rect(), ptMouse) )
	{
		if (m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			CFadeBox::FADE_DESC Desc = {};
			Desc.fIn_Duration = 0.5f;
			Desc.fOut_Duration = 1.f;
			Desc.phasFadeCompleted = &m_isReadytoDeactivate;
			CUI_Manager::Get_Instance()->Add_FadeBox(Desc);
			/*
			for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
			{
				if (m_pGameInstance->Get_IsLoopingSound(i))
				{
					m_pGameInstance->FadeinSound(i, fTimeDelta);
				}
			}
			*/

		}
	}
	if (m_isActive && m_pGameInstance->Key_Down(DIK_ESCAPE, InputChannel::GamePlay))
	{
		CFadeBox::FADE_DESC Desc = {};
		Desc.fIn_Duration = 0.5f;
		Desc.fOut_Duration = 1.f;
		Desc.phasFadeCompleted = &m_isReadytoDeactivate;
		CUI_Manager::Get_Instance()->Add_FadeBox(Desc);
	}

	if (m_isReadytoDeactivate)
	{	
		m_isActive = false;
		m_isReadytoDeactivate = false;		
		CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
		return;
	}

	if (PtInRect(&m_pGameEndButton->Get_InitialRect(), ptMouse))
	{
		m_pGameEndButton->Set_Size(140.f, 80.f, 0.3f);
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			DestroyWindow(g_hWnd);
		}
	}
	else
	{
		m_pGameEndButton->Set_Size(150.f, 100.f, 0.35f);
	}


	//메뉴
	for (size_t i = 0; i < ENV_END; i++)
	{
		RECT rect = m_pSlots[i]->Get_Rect();
		rect.bottom -= 30;
		if (TRUE == PtInRect(&rect, ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			m_eCurSlot = (ENV_SLOT)i;
			m_pSlots[m_eCurSlot]->Set_ChangeTex(true);
			if (m_eCurSlot != m_ePrevSlot)
			{
				m_pSlots[m_ePrevSlot]->Set_ChangeTex(false);
				m_ePrevSlot = m_eCurSlot;
			}
			break;
		}
	}

	switch (m_eCurSlot)
	{
	case Client::CMenu::GRAPHIC:
		Tick_GraphicSlot(fTimeDelta);
		break;
	case Client::CMenu::SOUND:
		Tick_SoundSlot(fTimeDelta);
		break;
	case Client::CMenu::ENV_END:
		break;
	default:
		break;
	}


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_pExitButton->Tick(fTimeDelta);
	m_pBackGround->Tick(fTimeDelta);
	m_pTitleButton->Tick(fTimeDelta);
	m_pUnderBar->Tick(fTimeDelta);
	m_pSelectButton->Tick(fTimeDelta);
	m_pGameEndButton->Tick(fTimeDelta);

	for (size_t i = 0; i < ENV_END; i++)
	{
		m_pSlots[i]->Tick(fTimeDelta);
	}
	for (size_t i = 0; i < MENU_END; i++)
	{
		m_pMenu[i]->Tick(fTimeDelta);
	}

}

void CMenu::Late_Tick(_float fTimeDelta)
{
	
	if (m_isActive)
	{
		switch (m_eCurSlot)
		{
		case Client::CMenu::GRAPHIC:
			for (size_t i = 0; i < CGraphicSetting::LIST_END; i++)
			{
				m_pGraphicSettings[i]->Late_Tick(fTimeDelta);
			}
			break;
		case Client::CMenu::SOUND:		
			for (size_t i = 0; i < CSoundSetting::LIST_END; i++)
			{
				m_pSoundSettings[i]->Late_Tick(fTimeDelta);
			}
			break;
		case Client::CMenu::ENV_END:
			break;
		default:
			break;
		}


		for (size_t i = 0; i < ENV_END; i++)
		{
			m_pSlots[i]->Late_Tick(fTimeDelta);
		}
		for (size_t i = 0; i < MENU_END; i++)
		{
			m_pMenu[i]->Late_Tick(fTimeDelta);
		}
		m_pExitButton->Late_Tick(fTimeDelta);
		m_pBackGround->Late_Tick(fTimeDelta);
		m_pTitleButton->Late_Tick(fTimeDelta);
		m_pUnderBar->Late_Tick(fTimeDelta);
		m_pSelectButton->Late_Tick(fTimeDelta);
		m_pGameEndButton->Late_Tick(fTimeDelta);
	}

	
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CMenu::Render()
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


void CMenu::Init_State()
{
	m_eCurSlot = GRAPHIC;
	m_ePrevSlot = GRAPHIC;
	m_pSlots[GRAPHIC]->Set_ChangeTex(true);
	m_pSlots[SOUND]->Set_ChangeTex(false);

	m_isActive = true;
	m_isReadytoActivate = false;
	CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
	CCamera_Manager::Get_Instance()->Set_CameraState(CS_SKILLBOOK);
}

void CMenu::Tick_GraphicSlot(_float fTimeDelta)
{
	for (size_t i = 0; i < CGraphicSetting::LIST_END; i++)
	{
		m_pGraphicSettings[i]->Tick(fTimeDelta);
	}


}

void CMenu::Tick_SoundSlot(_float fTimeDelta)
{
	for (size_t i = 0; i < CSoundSetting::LIST_END; i++)
	{
		m_pSoundSettings[i]->Tick(fTimeDelta);
	}
}

HRESULT CMenu::Add_Parts()
{


	_float fY = 85.f;
	_float fTerm = 300.f / (_uint)MENU_END;
	_float fStartX = fTerm;


	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.fFontSize = 0.5f;
	Button.strText = TEXT("설정");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	Button.vPosition = _vec2(30.f, 30.f);
	Button.vSize = _vec2(50.f, 50.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(40.f, 0.f);

	m_pTitleButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pTitleButton)
	{
		return E_FAIL;
	}

	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Out");
	Button.vPosition = _vec2(1230.f, 40.f);
	Button.vSize = _vec2(70.f, 70.f);
	m_pExitButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pExitButton)
	{
		return E_FAIL;
	}

	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_InvenUnderBar");
	Button.vSize = _vec2(fTerm, 150.f);
	Button.vPosition = _vec2(fStartX - 95.f, fY + 5.f);
	Button.vTextPosition = _vec2(0.f, 0.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);

	m_pUnderBar = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pUnderBar)
	{
		return E_FAIL;
	}

	Button.fDepth = m_fDepth - 0.02f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SelecInvenMenu");
	Button.vSize = _vec2(fTerm, 34.f);
	Button.vPosition = _vec2(fStartX - 95.f, fY - 5.f);
	m_pSelectButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pSelectButton)
	{
		return E_FAIL;
	}
	
	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 1.f;
	TextButton.fFontSize = 0.45f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strText = TEXT("환경");
	TextButton.vPosition = _vec2(fStartX - 95.f, fY - 5.f);
	TextButton.vSize = _vec2(fTerm, 70.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_NoTex");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pMenu[ENV] = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pMenu[ENV])
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pMenu[ENV])->Set_Pass(VTPass_UI_Alpha);
	


	TextButton.fFontSize = 0.4f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strText = TEXT("그래픽");
	TextButton.vPosition = _vec2(120.f, fY + 50.f);
	TextButton.vSize = _vec2(150.f, 120.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	TextButton.strTexture2 = TEXT("Prototype_Component_Texture_UI_Button_Blue2");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	TextButton.isChangePass = true;
	m_pSlots[GRAPHIC] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pSlots[GRAPHIC])
	{
		return E_FAIL;
	}
	m_pSlots[GRAPHIC]->Set_ChangeTex(true);

	TextButton.strText = TEXT("사운드");
	TextButton.vPosition = _vec2(120.f, fY + 100.f);
	TextButton.vSize = _vec2(150.f, 120.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	TextButton.strTexture2 = TEXT("Prototype_Component_Texture_UI_Button_Blue2");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	TextButton.isChangePass = true;
	m_pSlots[SOUND] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pSlots[SOUND])
	{
		return E_FAIL;
	}


	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_buttonRed");
	TextButton.strText = TEXT("게임 종료");
	TextButton.vPosition = _vec2(1150.f, 660.f);
	m_pGameEndButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pGameEndButton)

	{
		return E_FAIL;

	}
	UiInfo info{};
	info.strTexture = TEXT("Prototype_Component_Texture_Skill_Background2");
	info.vPos = _vec2((_float)g_iWinSizeX / 2.f, (_float)g_iWinSizeY / 2.f);
	info.vSize = _vec2((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	info.iLevel = (_uint)LEVEL_STATIC;

	m_pBackGround = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BackGround_Mask"), &info);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}

	for (size_t i = 0; i < CGraphicSetting::LIST_END; i++)
	{
		CGraphicSetting::GRAPHICSETTING GDesc{};
		GDesc.eList = (CGraphicSetting::GRAPHICLIST)i;
		GDesc.vPos = _vec2(720.f, fY + 45.f + (50.f * i) + (3.f * i));

		m_pGraphicSettings[i] = (CGraphicSetting*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_GraphicSetting"), &GDesc);
		if (not m_pGraphicSettings[i])
		{
			return E_FAIL;
		}
	}
	
	for (size_t i = 0; i < CSoundSetting::LIST_END; i++)
	{
		CSoundSetting::SOUNDSETTING SDesc{};
		SDesc.eList = (CSoundSetting::SOUNDLIST)i;
		SDesc.vPos = _vec2(720.f, fY + 45.f + (50.f * i) + (3.f * i));

		m_pSoundSettings[i] = (CSoundSetting*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SoundSetting"), &SDesc);
		if (not m_pSoundSettings[i])
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CMenu::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Setting"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMenu::Bind_ShaderResources()
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

CMenu* CMenu::Create(_dev pDevice, _context pContext)
{
	CMenu* pInstance = new CMenu(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMenu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMenu::Clone(void* pArg)
{
	CMenu* pInstance = new CMenu(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMenu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMenu::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < MENU_END; i++)
		{
			Safe_Release(m_pMenu[i]);
		}
		for (size_t i = 0; i < ENV_END; i++)
		{
			Safe_Release(m_pSlots[i]);
		}
		for (size_t i = 0; i < CGraphicSetting::LIST_END; i++)
		{
			Safe_Release(m_pGraphicSettings[i]);
		}
		for (size_t i = 0; i < CSoundSetting::LIST_END; i++)
		{
			Safe_Release(m_pSoundSettings[i]);
		}

	}

	Safe_Release(m_pSetting);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pTitleButton);

	Safe_Release(m_pUnderBar);
	Safe_Release(m_pSelectButton);
	Safe_Release(m_pGameEndButton);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}