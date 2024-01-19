#include "FadeBox.h"
#include "GameInstance.h"

CFadeBox::CFadeBox(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CFadeBox::CFadeBox(const CFadeBox& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CFadeBox::Init_Prototype()
{
	return S_OK;
}

HRESULT CFadeBox::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fDepth = 0.05f;



	m_eState = *((STATE*)pArg);

	switch (m_eState)
	{
	case FADEIN:
		m_fAlpha = 0.f;
		break;
	case FADELOOP:
		m_fDepth = 0.5f;
		m_fAlpha = 0.f;
		break;
	case FADEOUT:
		m_fAlpha = 1.f;
		break;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	return S_OK;
}

void CFadeBox::Tick(_float fTimeDelta)
{
	switch (m_eState)
	{
	case FADEIN:
		if (m_fAlpha >= 1.f)
			m_isDead = true;
		m_fAlpha += fTimeDelta * m_fDir * 2.f;
		break;
	case FADELOOP:
		if(m_fAlpha < 0.7f)
			m_fAlpha += fTimeDelta;
		break;
	case FADEOUT:
		if (m_fAlpha <= 0.f)
			m_isDead = true;
		m_fAlpha -= fTimeDelta * m_fDir * 2.f;

		break;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CFadeBox::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CFadeBox::Render()
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

HRESULT CFadeBox::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_FadeBox"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFadeBox::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CFadeBox* CFadeBox::Create(_dev pDevice, _context pContext)
{
	CFadeBox* pInstance = new CFadeBox(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CFadeBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFadeBox::Clone(void* pArg)
{
	CFadeBox* pInstance = new CFadeBox(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CFadeBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFadeBox::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
