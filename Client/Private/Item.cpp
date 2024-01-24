#include "Item.h"
#include "GameInstance.h"
#include "TextButton.h"

CItem::CItem(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CItem::Init_Prototype()
{
	return S_OK;
}

HRESULT CItem::Init(void* pArg)
{

	m_eItemDesc = ((ITEM_DESC*)pArg)->eItemDesc;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_bCanInteract = ((ITEM_DESC*)pArg)->bCanInteract;

	m_iNum = ((ITEM_DESC*)pArg)->iNum;

	m_fSizeX = ((ITEM_DESC*)pArg)->vSize.x;
	m_fSizeY = ((ITEM_DESC*)pArg)->vSize.y;

	m_fX = ((ITEM_DESC*)pArg)->vPosition.x;
	m_fY = ((ITEM_DESC*)pArg)->vPosition.y;

	m_fDepth = ((ITEM_DESC*)pArg)->fDepth;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	return S_OK;
}

void CItem::Tick(_float fTimeDelta)
{
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CItem::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

	if (m_iNum > 1)
	{
		_vec2 vStartPos = _vec2(m_fX, m_fY);
		wstring strText = TEXT("°³");
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum) + strText, _vec2(vStartPos.x + 1.f, vStartPos.y), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum) + strText, _vec2(vStartPos.x, + vStartPos.y + 1.f), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum) + strText, _vec2(vStartPos.x, + vStartPos.y), 0.5f, _vec4(1.f, 1.f, 1.f, 1.f));
													
	}

}

HRESULT CItem::Render()
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

void CItem::Set_Position(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;	
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

}

HRESULT CItem::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_eItemDesc.strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
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

CItem* CItem::Create(_dev pDevice, _context pContext)
{
	CItem* pInstance = new CItem(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
