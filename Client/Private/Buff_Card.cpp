#include "Buff_Card.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "NineSlice.h"
#include "UI_Manager.h"
#include "SummonWindow.h"
#include "SummonWindowPet.h"
#include "Item.h"

CBuff_Card::CBuff_Card(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CBuff_Card::CBuff_Card(const CBuff_Card& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CBuff_Card::Init_Prototype()
{
	return S_OK;
}

HRESULT CBuff_Card::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 300.f;
	m_fSizeY = 300.f;

	m_fX = ((BUFFCARD_DESC*)pArg)->vPos.x;
	m_fY = ((BUFFCARD_DESC*)pArg)->vPos.y;

	m_eBuff = ((BUFFCARD_DESC*)pArg)->eBuff;

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

void CBuff_Card::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CBuff_Card::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CBuff_Card::Render()
{
	
	
	return S_OK;
}


HRESULT CBuff_Card::Add_Parts()
{
	//CTextButton::TEXTBUTTON_DESC Button = {};

	//Button.eLevelID = LEVEL_STATIC;
	//Button.fDepth = m_fDepth + 0.01f;
	//Button.strText = TEXT("");
	//Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Exit");
	//Button.vPosition = _vec2(_vec2(m_fX + 145.f, m_fY - 170.f));
	//Button.vSize = _vec2(30.f, 30.f);

	//m_pExitButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	//if (not m_pExitButton)
	//{
	//	return E_FAIL;
	//}

	//if (m_eItemDesc.eItemUsage == IT_VEHICLECARD || m_eItemDesc.eItemUsage == IT_PETCARD)
	//{
	//	Button.strText = TEXT("¼ÒÈ¯");
	//}
	//else
	//{
	//	Button.strText = TEXT("ÀåÂø");
	//}

	//Button.fFontSize = 0.4f;
	//Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	//Button.vTextPosition = _vec2(0.f, -2.f);
	//Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	//Button.vPosition = _vec2(m_fX + 80.f, m_fY + 60.f);
	//Button.vSize = _vec2(150.f, 100.f);
	//m_SelectButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	//if (not m_SelectButton)
	//{
	//	return E_FAIL;
	//}

	//CNineSlice::SLICE_DESC SliceDesc{};
	//SliceDesc.eLevelID = LEVEL_STATIC;
	//SliceDesc.fDepth = m_fDepth + 0.02f;
	//SliceDesc.fFontSize = 0.f;
	//SliceDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_Skill_Levelup_Icon_02");
	//SliceDesc.strText = TEXT("");
	//SliceDesc.vPosition = _vec2(m_fX, m_fY - 50.f);
	//SliceDesc.vSize = _vec2(350.f, 300.f);
	//m_pBackGround = (CNineSlice*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NineSlice"), &SliceDesc);
	//if (not m_pBackGround)
	//{
	//	return E_FAIL;
	//}

	//CItem::ITEM_DESC ItemDesc = {};
	//ItemDesc.bCanInteract = false;
	//ItemDesc.eItemDesc = m_eItemDesc;
	//ItemDesc.fDepth = m_fDepth + 0.01f;
	//ItemDesc.vPosition = _vec2(m_fX - 110.f, m_fY - 100.f);
	//ItemDesc.vSize = _vec2(80.f, 80.f);
	//ItemDesc.isScreen = false;
	//ItemDesc.haveBG = true;
	//m_pItemTex = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &ItemDesc);
	//if (not m_pItemTex)
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CBuff_Card::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBuff_Card::Bind_ShaderResources()
{
	return S_OK;
}

CBuff_Card* CBuff_Card::Create(_dev pDevice, _context pContext)
{
	CBuff_Card* pInstance = new CBuff_Card(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBuff_Card::Clone(void* pArg)
{
	CBuff_Card* pInstance = new CBuff_Card(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuff_Card::Free()
{
	__super::Free();

	Safe_Release(m_pItemTex);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pBackGround);
	Safe_Release(m_SelectButton);

	Safe_Release(m_pRendererCom);
}
