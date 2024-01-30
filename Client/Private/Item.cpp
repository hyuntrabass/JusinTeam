#include "Item.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "ItemInfo.h"

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
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_bCanInteract)
	{
		if (PtInRect(&m_rcRect, ptMouse) && m_pGameInstance->Mouse_Down(DIM_RBUTTON, InputChannel::UI))
		{
			if (m_pGameInstance->Get_LayerSize(LEVEL_STATIC, TEXT("Layer_ItemInfo")) == 0)
			{
				CItemInfo::ITEMINFO_DESC InfoDesc = {};
				InfoDesc.eItemDesc = m_eItemDesc;
				InfoDesc.fDepth = m_fDepth - 0.05f;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_ItemInfo"), TEXT("Prototype_GameObject_ItemInfo"), &InfoDesc)))
				{
					return;
				}
			}

		}
	}


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CItem::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CItem::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Sprite)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	if (m_iNum > 1)
	{
		_vec2 vStartPos = _vec2(m_fX + 14.f, m_fY + 20.f);
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x, +vStartPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f));
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

	_int2 vNumSprite = { 8, 8 };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof(_int2))))
	{
		return E_FAIL;
	}
	_uint iIndex = m_eItemDesc.iIndex;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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
