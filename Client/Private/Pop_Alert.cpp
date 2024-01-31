#include "Pop_QuestIn.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "BlurTexture.h"
#include "FadeBox.h"
#include "Pop_Alert.h"

CPop_Alert::CPop_Alert(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CPop_Alert::CPop_Alert(const CPop_Alert& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CPop_Alert::Init_Prototype()
{
	return S_OK;
}

HRESULT CPop_Alert::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 300.f;
	m_fSizeY = 300.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 80.f;

	m_fDepth = (_float)D_ALERT / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CPop_Alert::Tick(_float fTimeDelta)
{
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


}

void CPop_Alert::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CPop_Alert::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	/*
	_float fFontSize = 0.4f;
	m_pGameInstance->Render_Text(L"Font_Malang", m_AlertQueue.front(), _vec2(m_fX - 1.f, m_fY), fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2(m_fX + 1.f, m_fY), fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2(m_fX, m_fY - 1.f), fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2(m_fX, m_fY + 1.f), fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strQuestTitle, _vec2(m_fX, m_fY), fFontSize, _vec4(1.f, 1.f, 1.f, 1.f));
	*/
	
	return S_OK;
}

HRESULT CPop_Alert::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CPop_Alert::Set_Alert(wstring strText)
{
}

HRESULT CPop_Alert::Bind_ShaderResources()
{
	return S_OK;
}

CPop_Alert* CPop_Alert::Create(_dev pDevice, _context pContext)
{
	CPop_Alert* pInstance = new CPop_Alert(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPop_Alert");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPop_Alert::Clone(void* pArg)
{
	CPop_Alert* pInstance = new CPop_Alert(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPop_Alert");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPop_Alert::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
}
