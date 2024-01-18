#include "JobMark.h"
#include "GameInstance.h"
#include "TextButton.h"

CJobMark::CJobMark(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CJobMark::CJobMark(const CJobMark& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CJobMark::Init_Prototype()
{
	return S_OK;
}

HRESULT CJobMark::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 150.f;
	m_fSizeY = 150.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 610.f;

	m_fDepth = 0.7f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = 0.5f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_basic_bow");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(80.f, 80.f);

	m_pJob = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pJob)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CJobMark::Tick(_float fTimeDelta)
{

	if (m_eCurState != m_ePrevState)
	{
		if (m_pJob)
		{
			Safe_Release(m_pJob);
		}

		CTextButton::TEXTBUTTON_DESC Button = {};
		Button.eLevelID = LEVEL_STATIC;
		Button.fDepth = 0.5f;
		Button.vPosition = _vec2(m_fX, m_fY);
		Button.vSize = _vec2(80.f, 80.f);

		switch (m_eCurState)
		{
		BOW:
			Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_basic_bow");
			break;
		ASSASSIN:
			Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_basic_sword");
			break;
		}
		m_pJob = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

		if (not m_pJob)
		{
			return;

		}
		m_ePrevState = m_eCurState;
	}
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CJobMark::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	if (m_pJob != nullptr)
	{
		m_pJob->Late_Tick(fTimeDelta);
	}
}

HRESULT CJobMark::Render()
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

HRESULT CJobMark::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Job"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CJobMark::Bind_ShaderResources()
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

CJobMark* CJobMark::Create(_dev pDevice, _context pContext)
{
	CJobMark* pInstance = new CJobMark(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CJobMark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJobMark::Clone(void* pArg)
{
	CJobMark* pInstance = new CJobMark(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CJobMark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJobMark::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pJob);
}
