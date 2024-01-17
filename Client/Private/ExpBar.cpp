#include "ExpBar.h"
#include "GameInstance.h"
#include "TextButton.h"

CExpBar::CExpBar(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CExpBar::CExpBar(const CExpBar& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CExpBar::Init_Prototype()
{
	return S_OK;
}

HRESULT CExpBar::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}





	m_fSizeX = g_iWinSizeX;
	m_fSizeY = 12.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 720.f;

	m_fDepth = 0.8f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_vRatio = _float2(70.f, 100.f);

	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = 1.f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(m_fSizeX, m_fSizeY);

	m_pBackground = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pBackground)
	{
		return E_FAIL;
	}


	return S_OK;
}

void CExpBar::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_1))
		m_vRatio.x -= 2.f;;
	if (m_pGameInstance->Key_Pressing(DIK_2))
	{
		if(m_vRatio.x <= m_vRatio.y)
			m_vRatio.x += 0.1f;;
	}
	
	m_fTime += fTimeDelta;
}

void CExpBar::Late_Tick(_float fTimeDelta)
{
	m_pBackground->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CExpBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_HP)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	_float iExp = 100.f * (m_vRatio.x / m_vRatio.y);
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("경험치 ") + to_wstring(static_cast<_uint>(iExp)) + TEXT("% "), _vec2(50.f - 0.1f, m_fY), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("경험치 ") + to_wstring(static_cast<_uint>(iExp)) + TEXT("% "), _vec2(50.f + 0.1f, m_fY), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("경험치 ") + to_wstring(static_cast<_uint>(iExp)) + TEXT("% "), _vec2(50.f, m_fY - 0.1f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("경험치 ") + to_wstring(static_cast<_uint>(iExp)) + TEXT("% "), _vec2(50.f, m_fY + 0.1f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Dialogue",  TEXT("경험치 ") + to_wstring(static_cast<_uint>(iExp)) + TEXT("% "), _vec2(50.f, m_fY), 0.3f, _vec4(0.5f, 1.0f, 0.5f, 1.f));


	return S_OK;
}

HRESULT CExpBar::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_EXPBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_maskno"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CExpBar::Bind_ShaderResources()
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
	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	_vec4 vColor = _vec4(0.5f, 1.f, 0.5f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof(_vec4))))
	{
		return E_FAIL;
	}

	_float fRatio = m_vRatio.x / m_vRatio.y;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpRatio", &fRatio, sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

CExpBar* CExpBar::Create(_dev pDevice, _context pContext)
{
	CExpBar* pInstance = new CExpBar(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CExpBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExpBar::Clone(void* pArg)
{
	CExpBar* pInstance = new CExpBar(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CExpBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExpBar::Free()
{
	__super::Free();

	Safe_Release(m_pBackground);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
