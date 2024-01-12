#include "SelectDesc.h"
#include "GameInstance.h"

CSelectDesc::CSelectDesc(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSelectDesc::CSelectDesc(const CSelectDesc& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSelectDesc::Init_Prototype()
{
	return S_OK;
}

HRESULT CSelectDesc::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_eCharacter = ((SELECT_DESC*)pArg)->eCharacter;

	/*
	
	m_fSizeX = 280.f;
	m_fSizeY = 20.f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = 80.f;

	m_fDepth = 0.5f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	*/	
	m_fDepth = 0.5f;
	return S_OK;
}

void CSelectDesc::Tick(_float fTimeDelta)
{
	
}

void CSelectDesc::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CSelectDesc::Render()
{
	_float fStartX = 20.f;

	switch (m_eCharacter)
	{
	case WARRIOR:
	{
		m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("워리어"), _vec2(fStartX, 100.f), 0.5f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("| 정의의 선봉장"), _vec2(fStartX + 40.f, 100.f), 0.3f, _vec4(0.2f, 0.2f, 0.8f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("높은 방어력과 체력을 바탕으로"), _vec2(fStartX, 180.f), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("아군을 보호하며 적을 제어하는 클래스"), _vec2(fStartX, 200.f), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}
	break;
	case SORCERESS:
	{

	}
	break;
	case ROGUE:
	{

	}
	break;
	case PRIEST:
	{

	}
	break;

	}

	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("난이도"), _vec2(fStartX, 260.f), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("공격력"), _vec2(fStartX, 300.f), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("방어력"), _vec2(fStartX, 320.f), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("아군 보호"), _vec2(fStartX, 340.f), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Dialogue", TEXT("적군 제어"), _vec2(fStartX, 360.f), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);

	return S_OK;
}

HRESULT CSelectDesc::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSelectDesc::Bind_ShaderResources()
{

	return S_OK;
}

CSelectDesc* CSelectDesc::Create(_dev pDevice, _context pContext)
{
	CSelectDesc* pInstance = new CSelectDesc(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSelectDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSelectDesc::Clone(void* pArg)
{
	CSelectDesc* pInstance = new CSelectDesc(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSelectDesc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelectDesc::Free()
{
	__super::Free();


	Safe_Release(m_pRendererCom);

}
