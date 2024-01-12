#include "Select.h"
#include "GameInstance.h"

CSelect::CSelect(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSelect::CSelect(const CSelect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSelect::Init_Prototype()
{
	return S_OK;
}

HRESULT CSelect::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}



	return S_OK;
}

void CSelect::Tick(_float fTimeDelta)
{
	m_pCharacterSelect->Tick(fTimeDelta);
}

void CSelect::Late_Tick(_float fTimeDelta)
{
	m_pCharacterSelect->Late_Tick(fTimeDelta);
	m_pClassButton->Late_Tick(fTimeDelta);
}

HRESULT CSelect::Render()
{

	return S_OK;
}

HRESULT CSelect::Add_Components()
{
	
	return S_OK;
}

HRESULT CSelect::Add_Parts()
{
	m_pCharacterSelect = (CCharacterSelect*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CharacterSelect"));
	if (not m_pCharacterSelect)
	{
		return E_FAIL;
	}

	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_SELECT;
	ButtonDesc.fDepth = 0.5f;
	ButtonDesc.fFontSize = 0.4f;
	ButtonDesc.strText = TEXT("클래스 선택");
	ButtonDesc.strTexture = TEXT("");
	ButtonDesc.vPosition = _vec2(60.f, 60.f);
	ButtonDesc.vSize = _vec2(20.f, 20.f);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 0.f);
	
	m_pClassButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CharacterSelect"), &ButtonDesc);
	if (not m_pClassButton)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSelect::Bind_ShaderResources()
{

	return S_OK;
}

CSelect* CSelect::Create(_dev pDevice, _context pContext)
{
	CSelect* pInstance = new CSelect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSelect::Clone(void* pArg)
{
	CSelect* pInstance = new CSelect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelect::Free()
{
	__super::Free();

	Safe_Release(m_pSelectButton);
	Safe_Release(m_pClassButton);
	Safe_Release(m_pCharacterSelect);

}
