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
	
	m_fSizeX = 800.f;
	m_fSizeY = 70.f;

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
		m_pOn->Set_Alpha(1.f);
		m_pOff->Set_Alpha(0.f);
	}
	else if (PtInRect(&m_pOff->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::Default))
	{
		m_pOn->Set_Alpha(0.f);
		m_pOff->Set_Alpha(1.f);
	}
}

void CGraphicSetting::Late_Tick(_float fTimeDelta)
{
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

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(200.f + 1.f, m_fY + 1.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(200.f, m_fY), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);


	return S_OK;
}



void CGraphicSetting::Update_State(_bool isOn)
{	/*
		SSAO ²ô±â / ÄÑ±â Renderer->Set_TurnOnSSAO
		ToneMapping(Åæ¸Ê) ²ô±â / ÄÑ±â Renderer->Set_TurnOnToneMap
		Bloom(ºí·ë) ²ô±â / ÄÑ±â Renderer->Set_TurnOnBloom
		±×¸²ÀÚ ²ô±â / ÄÑ±â m_pGameInstance->Set_TurnOnShadow
		¸ð¼Ç ºí·¯ ²ô±â / ÄÑ±â Renderer->Set_TurnOnMotionBlur
		FXAA ²ô±â / ÄÑ±â Renderer->Set_TurnOnFXAA
		DOF ²ô±â / ÄÑ±â Renderer->Set_TurnOnDOF
		GalMegi(God Ray) ²ô±â / ÄÑ±â Renderer->Set_TurnOnGalMegi
	*/
	switch (m_eGraphic)
	{
	case Client::CGraphicSetting::SSAO:
		//m_pRendererCom->
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

	_float fDefX = 800.f;

	CNineSlice::SLICE_DESC SliceDesc{};
	SliceDesc.eLevelID = LEVEL_STATIC;
	SliceDesc.fDepth = m_fDepth - 0.01f;
	SliceDesc.fFontSize = 0.f;
	SliceDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_BuffBg2");
	SliceDesc.strText = TEXT("");
	SliceDesc.vPosition = _vec2(fDefX, m_fY);
	SliceDesc.vSize = _vec2(260.f, m_fY - 5.f);
	m_pBackGround = (CNineSlice*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NineSlice"), &SliceDesc);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}
	SliceDesc.fDepth = m_fDepth - 0.02f;
	SliceDesc.fFontSize = 0.38f;
	SliceDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_BuffBg2");
	SliceDesc.strText = TEXT("Off");
	SliceDesc.vPosition = _vec2(fDefX - 70.f, m_fY);
	SliceDesc.vSize = _vec2(120.f, m_fSizeY - 8.f);
	m_pOff = (CNineSlice*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NineSlice"), &SliceDesc);
	if (not m_pOff)
	{
		return E_FAIL;
	}
	m_pOff->Set_Pass(VTPass_SpriteAlpha);
	m_pOff->Set_Alpha(1.f);

	SliceDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_BuffBg2");
	SliceDesc.strText = TEXT("On");
	SliceDesc.vPosition = _vec2(fDefX + 70.f, m_fY);
	SliceDesc.vSize = _vec2(120.f, m_fSizeY - 8.f);
	m_pOn = (CNineSlice*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NineSlice"), &SliceDesc);
	if (not m_pOn)
	{
		return E_FAIL;
	}

	m_pOn->Set_Pass(VTPass_SpriteAlpha);
	m_pOn->Set_Alpha(1.f);

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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Setting"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
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