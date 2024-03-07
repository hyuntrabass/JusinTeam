#include "GraphicSetting.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "Camera_Manager.h"
#include "NineSlice.h"

CGraphicSetting::CGraphicSetting(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CGraphicSetting::CGraphicSetting(const CGraphicSetting& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CGraphicSetting::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CGraphicSetting::Init(void* pArg)
{
	m_eGraphic = ((GRAPHICSETTING*)pArg)->eList;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fSizeX = 1000.f;
	m_fSizeY = 50.f;

	m_fX = ((GRAPHICSETTING*)pArg)->vPos.x;
	m_fY = ((GRAPHICSETTING*)pArg)->vPos.y;

	m_fDepth = (_float)D_MENU / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	if (FAILED(Init_State()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Set_TurnOnShadow(true);
	m_pRendererCom->Set_TurnOnMotionBlur(true);
	return S_OK;
}

void CGraphicSetting::Tick(_float fTimeDelta)
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
	
	if (PtInRect(&m_pOn->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Default))
	{
		Update_State(true);
		m_pOn->Set_Alpha(MAX_ALPHA);
		m_pOff->Set_Alpha(0.f);
		m_pOn->Set_TextColor(_vec4(1.f, 1.f, 1.f, 1.f));
		m_pOff->Set_TextColor(_vec4(0.6f, 0.6f, 0.6f, 1.f));
	}
	else if (PtInRect(&m_pOff->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Default))
	{
		Update_State(false);
		m_pOn->Set_Alpha(0.f);
		m_pOff->Set_Alpha(MAX_ALPHA);
		m_pOff->Set_TextColor(_vec4(1.f, 1.f, 1.f, 1.f));
		m_pOn->Set_TextColor(_vec4(0.6f, 0.6f, 0.6f, 1.f));
	}
	m_pBackGround->Tick(fTimeDelta);
	m_pOn->Tick(fTimeDelta);
	m_pOff->Tick(fTimeDelta);
}

void CGraphicSetting::Late_Tick(_float fTimeDelta)
{
	m_pBackGround->Late_Tick(fTimeDelta);
	m_pOn->Late_Tick(fTimeDelta);
	m_pOff->Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CGraphicSetting::Render()
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


	return S_OK;
}



void CGraphicSetting::Update_State(_bool isOn)
{
	switch (m_eGraphic)
	{
	case Client::CGraphicSetting::SSAO:
		m_pRendererCom->Set_TurnOnSSAO(isOn);
		break;
	case Client::CGraphicSetting::TONE:
		m_pRendererCom->Set_TurnOnToneMap(isOn);
		break;
	case Client::CGraphicSetting::BLOOM:
		m_pRendererCom->Set_TurnOnBloom(isOn);
		break;
	case Client::CGraphicSetting::SHADOW:
		m_pGameInstance->Set_TurnOnShadow(isOn); //false
		break;
	case Client::CGraphicSetting::MOTIONBLUR:
		m_pRendererCom->Set_TurnOnMotionBlur(isOn); //false
		break;
	case Client::CGraphicSetting::FXAA:
		m_pRendererCom->Set_TurnOnFXAA(isOn);
		break;
	case Client::CGraphicSetting::DOF:
		m_pRendererCom->Set_TurnOnDOF(isOn);
		break;
	case Client::CGraphicSetting::GODRAY:
		m_pRendererCom->Set_TurnOnGalMegi(isOn);
		break;
	case Client::CGraphicSetting::LIST_END:
		break;
	default:
		break;
	}
}

HRESULT CGraphicSetting::Init_State()
{
	switch (m_eGraphic)
	{
	case Client::CGraphicSetting::SSAO:
		m_strText = TEXT("SSAO");
		break;
	case Client::CGraphicSetting::TONE:
		m_strText = TEXT("Åæ¸Ê");
		break;
	case Client::CGraphicSetting::BLOOM:
		m_strText = TEXT("ºí·ë");
		break;
	case Client::CGraphicSetting::SHADOW:
		m_strText = TEXT("±×¸²ÀÚ");
		break;
	case Client::CGraphicSetting::MOTIONBLUR:
		m_strText = TEXT("¸ð¼Ç ºí·¯");
		break;
	case Client::CGraphicSetting::FXAA:
		m_strText = TEXT("FFAA");
		break;
	case Client::CGraphicSetting::DOF:
		m_strText = TEXT("DOF");
		break;
	case Client::CGraphicSetting::GODRAY:
		m_strText = TEXT("GOD-RAY");
		break;
	case Client::CGraphicSetting::LIST_END:
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CGraphicSetting::Add_Parts()
{

	_float fDefX = 1100.f;

	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 1.f;
	TextButton.fFontSize = 0.35f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_RoundBox");
	TextButton.strText = TEXT("");
	TextButton.vPosition = _vec2(fDefX, m_fY);
	TextButton.vSize = _vec2(212.f, m_fSizeY - 10.f);
	m_pBackGround = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}
	m_pBackGround->Set_Pass(VTPass_UI);

	TextButton.fDepth = m_fDepth - 0.02f;
	TextButton.fAlpha = 1.f;
	TextButton.fFontSize = 0.35f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_RoundBox2");
	TextButton.strText = TEXT("Off");
	TextButton.vPosition = _vec2(fDefX - 52.f, m_fY);
	TextButton.vSize = _vec2(100.f, m_fSizeY - 18.f);
	m_pOff = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pOff)
	{
		return E_FAIL;
	}
	m_pOff->Set_Pass(VTPass_UI_Alpha);
	/*if (m_eGraphic == MOTIONBLUR || m_eGraphic == SHADOW)
	{
		m_pOff->Set_TextColor(_vec4(1.f, 1.f, 1.f, 1.f));
		m_pOff->Set_Alpha(MAX_ALPHA);
	}
	else*/
	{
		m_pOff->Set_TextColor(_vec4(0.6f, 0.6f, 0.6f, 1.f));
		m_pOff->Set_Alpha(0.f);
	}

	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_RoundBox2");
	TextButton.strText = TEXT("On");
	TextButton.vPosition = _vec2(fDefX + 52.f, m_fY);
	TextButton.vSize = _vec2(100.f, m_fSizeY - 18.f);
	m_pOn = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
	if (not m_pOn)
	{
		return E_FAIL;
	}
	m_pOn->Set_Pass(VTPass_UI_Alpha);
	/*if (m_eGraphic == MOTIONBLUR || m_eGraphic == SHADOW)
	{
		m_pOn->Set_Alpha(0.f);
		m_pOn->Set_TextColor(_vec4(0.6f, 0.6f, 0.6f, 1.f));
	}
	else*/
	{
		m_pOn->Set_TextColor(_vec4(1.f, 1.f, 1.f, 1.f));
		m_pOn->Set_Alpha(MAX_ALPHA);
	}


	return S_OK;
}

HRESULT CGraphicSetting::Add_Components()
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

HRESULT CGraphicSetting::Bind_ShaderResources()
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

CGraphicSetting* CGraphicSetting::Create(_dev pDevice, _context pContext)
{
	CGraphicSetting* pInstance = new CGraphicSetting(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGraphicSetting");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGraphicSetting::Clone(void* pArg)
{
	CGraphicSetting* pInstance = new CGraphicSetting(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGraphicSetting");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphicSetting::Free()
{
	__super::Free();

	Safe_Release(m_pBackGround);
	Safe_Release(m_pOn);
	Safe_Release(m_pOff);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}