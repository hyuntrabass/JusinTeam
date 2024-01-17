#include "Mouse.h"
#include "GameInstance.h"

CMouse::CMouse(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CMouse::CMouse(const CMouse& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CMouse::Init_Prototype()
{
	return S_OK;
}

HRESULT CMouse::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	m_fSizeX = 63.f;
	m_fSizeY = 63.f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fDepth = 0.01f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	ShowCursor(false);

	return S_OK;
}

void CMouse::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);
	m_fSizeX = 64.f;
	m_fSizeY = 64.f;

	m_fX = static_cast<_float>(ptMouse.x);
	m_fY = static_cast<_float>(ptMouse.y);

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CMouse::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CMouse::Render()
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

HRESULT CMouse::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMouse::Bind_ShaderResources()
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

CMouse* CMouse::Create(_dev pDevice, _context pContext)
{
	CMouse* pInstance = new CMouse(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMouse::Clone(void* pArg)
{
	CMouse* pInstance = new CMouse(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMouse::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
