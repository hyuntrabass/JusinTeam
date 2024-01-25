#include "ItemInfo.h"
#include "GameInstance.h"
#include "TextButton.h"

CItemInfo::CItemInfo(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CItemInfo::CItemInfo(const CItemInfo& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CItemInfo::Init_Prototype()
{
	return S_OK;
}

HRESULT CItemInfo::Init(void* pArg)
{

	m_eItemDesc = ((ITEMINFO_DESC*)pArg)->eItemDesc;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 300.f;
	m_fSizeY = 300.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = (_float)g_iWinSizeY / 2.f;

	m_fDepth = ((ITEMINFO_DESC*)pArg)->fDepth;
	m_eItemDesc = ((ITEMINFO_DESC*)pArg)->eItemDesc;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CItemInfo::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);


	if (PtInRect(&m_pExitButton->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_isDead = true;
		return;
	}


	m_pExitButton->Tick(fTimeDelta);
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CItemInfo::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

	m_pExitButton->Late_Tick(fTimeDelta);

}

HRESULT CItemInfo::Render()
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
	if (m_iNum > 1)
	{
		_vec2 vStartPos = _vec2(m_fX + 14.f, m_fY + 20.f);
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x, +vStartPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
	return S_OK;
}


HRESULT CItemInfo::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Exit");
	Button.vPosition = _vec2(m_fX + 70.f, m_fY - 70.f);
	Button.vSize = _vec2(30.f, 30.f);

	m_pExitButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pExitButton)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItemInfo::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_ItemInfo"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CItemInfo::Bind_ShaderResources()
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

CItemInfo* CItemInfo::Create(_dev pDevice, _context pContext)
{
	CItemInfo* pInstance = new CItemInfo(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemInfo::Clone(void* pArg)
{
	CItemInfo* pInstance = new CItemInfo(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemInfo::Free()
{
	__super::Free();

	Safe_Release(m_pItemTex);
	Safe_Release(m_pExitButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
