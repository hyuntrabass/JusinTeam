#include "SoundSetting.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "Camera_Manager.h"
#include "NineSlice.h"

CSoundSetting::CSoundSetting(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSoundSetting::CSoundSetting(const CSoundSetting& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSoundSetting::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CSoundSetting::Init(void* pArg)
{
	m_eSound = ((SOUNDSETTING*)pArg)->eList;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fSizeX = 1000.f;
	m_fSizeY = 50.f;

	m_fX = ((SOUNDSETTING*)pArg)->vPos.x;
	m_fY = ((SOUNDSETTING*)pArg)->vPos.y;

	m_fDepth = (_float)D_MENU / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_vStartPos = _vec2(1000.f, m_fY);
	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	if (FAILED(Init_State()))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CSoundSetting::Tick(_float fTimeDelta)
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
	

	_float fPercent = m_pNumBar->Get_Size().x / (_float)MAX_VOLUME;
	if (PtInRect(&m_pNumButton->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		m_isPickingButton = true;
	}
	if (!PtInRect(&rectUI, ptMouse) && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		m_isPickingButton = false;
	}


	if (m_isPickingButton)
	{
		CUI_Manager::Get_Instance()->Set_MouseState(CUI_Manager::M_GRAB);
		_float fMouseSensor = 0.1f;
		_long dwMouseMove;
		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			_vec2 vPos = _vec2(m_pNumButton->Get_TransPosition().x + dwMouseMove * fMouseSensor * 5.5f, m_pNumBar->Get_Position().y);
			m_pNumButton->Set_Position(vPos);
			if (m_pNumButton->Get_TransPosition().x <= m_pNumBar->Get_Position().x - m_pNumBar->Get_Size().x / 2.f)
			{
				if (dwMouseMove < 0.f)
				{
					m_pNumButton->Set_Position(_vec2(m_pNumBar->Get_Position().x - m_pNumBar->Get_Size().x / 2.f, m_pNumBar->Get_Position().y));
					return;
				}
			}
			if (m_pNumButton->Get_TransPosition().x > m_pNumBar->Get_Position().x + m_pNumBar->Get_Size().x / 2.f)
			{
				if (dwMouseMove > 0.f)
				{
					m_pNumButton->Set_Position(_vec2(m_pNumBar->Get_Position().x + m_pNumBar->Get_Size().x / 2.f + fPercent, m_pNumBar->Get_Position().y));
					return;
				}
			}

			m_iCurVolume = (_uint)((m_pNumButton->Get_TransPosition().x - (m_pNumBar->Get_Position().x - m_pNumBar->Get_Size().x / 2.f)) / fPercent);
			if (m_iCurVolume <= 0)
			{
				m_iCurVolume = 0;
			}
		}
	}

	if (m_isPickingButton && m_pGameInstance->Mouse_Up(DIM_LBUTTON))
	{
		m_isPickingButton = false;
	}


	_float fRatio = (_float)m_iCurVolume * fPercent / m_pNumBar->Get_Size().x;
	m_pNumBar->Set_Alpha(fRatio);
	
	Update_State();
	
	m_pNumBar->Tick(fTimeDelta);
	m_pNumBarBg->Tick(fTimeDelta);
	m_pNumButton->Tick(fTimeDelta);
}

void CSoundSetting::Late_Tick(_float fTimeDelta)
{
	m_pNumBar->Late_Tick(fTimeDelta);
	m_pNumBarBg->Late_Tick(fTimeDelta);
	m_pNumButton->Late_Tick(fTimeDelta);
	
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CSoundSetting::Render()
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


	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(240.f + 1.f, m_fY - 10.f + 1.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(240.f, m_fY - 10.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	
	_vec2 vNumPos = _vec2(m_vStartPos.x + m_pNumBar->Get_Size().x / 2.f + 20.f, m_fY);
	wstring strPercent = to_wstring(m_iCurVolume) + TEXT("%");
	m_pGameInstance->Render_Text(L"Font_Malang", strPercent, _vec2(vNumPos.x + 1.f, vNumPos.y - 10.f + 1.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", strPercent, _vec2(vNumPos.x, vNumPos.y - 10.f), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);


	return S_OK;
}



void CSoundSetting::Update_State()
{
	m_fSound = (_float)m_iCurVolume * 0.01f;
	switch (m_eSound)
	{
	case Client::CSoundSetting::ALL:
		m_pGameInstance->Set_SystemVolume(m_fSound);
		break;
	case Client::CSoundSetting::BACKGROUND:
		m_pGameInstance->Set_EnvironmentVolume(m_fSound);
		break;
	case Client::CSoundSetting::ENV:
		m_pGameInstance->Set_BackGroundVolume(m_fSound);
		break;
	case Client::CSoundSetting::EFFECT:
		m_pGameInstance->Set_EffectVolume(m_fSound);
		break;
	case Client::CSoundSetting::LIST_END:
		break;
	default:
		break;
	}
}

HRESULT CSoundSetting::Init_State()
{
	switch (m_eSound)
	{
	case Client::CSoundSetting::ALL:
		m_strText = TEXT("전체 음향");
		break;
	case Client::CSoundSetting::BACKGROUND:
		m_strText = TEXT("환경음");
		break;
	case Client::CSoundSetting::ENV:
		m_strText = TEXT("배경음");
		break;
	case Client::CSoundSetting::EFFECT:
		m_strText = TEXT("효과음");
		break;
	case Client::CSoundSetting::LIST_END:
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CSoundSetting::Add_Parts()
{

	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.strTexture = TEXT("");
	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 1.f;
	TextButton.fFontSize = 0.45f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strText = TEXT("");
	TextButton.vSize = _vec2(300.f, 3.f);
	TextButton.vPosition = _vec2(m_vStartPos.x, m_vStartPos.y);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_FadeBox");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pNumBar = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pNumBar)
	{
		return E_FAIL;
	}
	m_pNumBar->Set_Pass(VTPass_HPNoMask);
	m_pNumBar->Set_Alpha(1.f);

	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_FadeBox");
	m_pNumBarBg = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pNumBarBg)
	{
		return E_FAIL;
	}
	m_pNumBarBg->Set_Pass(VTPass_UI_Alpha);
	m_pNumBarBg->Set_Alpha(0.3f);

	_vec2 vPos = _vec2(m_vStartPos.x + m_pNumBar->Get_Size().x / 2.f, m_fY);

	_float fDefX = 1100.f;

	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.fFontSize = 0.4f;
	Button.strText = TEXT("");
	Button.fDepth = m_fDepth - 0.02f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_PickingRect");
	Button.vPosition = _vec2(vPos.x, vPos.y);
	Button.vSize = _vec2(40.f, 40.f);

	m_pNumButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pNumButton)
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CSoundSetting::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Grey"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSoundSetting::Bind_ShaderResources()
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

	_float fAlpha = MAX_ALPHA + 0.3f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSoundSetting* CSoundSetting::Create(_dev pDevice, _context pContext)
{
	CSoundSetting* pInstance = new CSoundSetting(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSoundSetting");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSoundSetting::Clone(void* pArg)
{
	CSoundSetting* pInstance = new CSoundSetting(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSoundSetting");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSoundSetting::Free()
{
	__super::Free();

	Safe_Release(m_pNumBar);
	Safe_Release(m_pNumBarBg);
	Safe_Release(m_pNumButton);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}