#include "Warning_Mark.h"
#include "TextButtonColor.h"

CWarning_Mark::CWarning_Mark(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CWarning_Mark::CWarning_Mark(const CWarning_Mark& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CWarning_Mark::Init_Prototype()
{
	return S_OK;
}

HRESULT CWarning_Mark::Init(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 100.f;
	m_fSizeY = 100.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = ((_float)g_iWinSizeY / 2.f) - 200.f;

	m_fDepth = (_float)D_ALERT / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	CTextButtonColor::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_TOWER;
	Button.fDepth = m_fDepth - 0.01f;
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(300.f, 300.f);
	Button.strTexture = L"Prototype_Component_Texture_Detected_Warning";
	Button.fAlpha = 0.45f;
	Button.vColor = _vec4(1.f, 0.f, 0.f, 1.f);

	m_pAlphaWarning = dynamic_cast<CTextButtonColor*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_TextButtonColor", &Button));
	m_pAlphaWarning->Set_Pass(VTPass_Mask_ColorAlpha);


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CWarning_Mark::Tick(_float fTimeDelta)
{
	if (true == m_isWarningEnd)
		m_fAlpha -= fTimeDelta * 0.5f;

	if (m_pAlphaWarning) {
		m_pAlphaWarning->Tick(fTimeDelta);
		if (m_pAlphaWarning->Get_Size().x > 100.f)
			m_pAlphaWarning->Set_Size(m_pAlphaWarning->Get_Size().x - fTimeDelta * 800.f, m_pAlphaWarning->Get_Size().y - fTimeDelta * 800.f);
		else
			Safe_Release(m_pAlphaWarning);
	}
}

void CWarning_Mark::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	if(m_pAlphaWarning)
		m_pAlphaWarning->Late_Tick(fTimeDelta);
}

HRESULT CWarning_Mark::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(VTPass_Mask_ColorAlpha)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWarning_Mark::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_TOWER, L"Prototype_Component_Texture_Detected_Warning", L"Com_Texture", reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWarning_Mark::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	_vec4 vColorRed = _vec4(1.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColorRed, sizeof(_vec4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}

CWarning_Mark* CWarning_Mark::Create(_dev pDevice, _context pContext)
{
	CWarning_Mark* pInstance = new CWarning_Mark(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CWarning_Mark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWarning_Mark::Clone(void* pArg)
{
	CWarning_Mark* pInstance = new CWarning_Mark(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CWarning_Mark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWarning_Mark::Free()
{
	__super::Free();

	Safe_Release(m_pAlphaWarning);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}
