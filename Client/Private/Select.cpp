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

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON))
	{
		int i = 0;
		
		for (i = 0; i < 4; ++i)
		{
			RECT rcRect = {
			  (LONG)((160.f + 320.f * i) - 320.f * 0.5f),
			  (LONG)(360.f - 360.f * 0.5f),
			  (LONG)((160.f + 320.f * i) + 320.f * 0.5f),
			  (LONG)(360.f + 360.f * 0.5f)
			};
			if (PtInRect(&rcRect, ptMouse))
			{
				m_pCharacterSelect->Set_Active_Alpha(CCharacterSelect::ALPHA);
				Set_SelectDesc(i);
				m_pGameInstance->Set_CameraState(CAMERA_STATE::CM_ZOOM);
				/* 일단 
				CTransform* pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_SELECT, TEXT("Layer_Void05"),TEXT("Com_Transform"));
				//이거 널인듯? 
				_vec3 vTargetPos = pTransform->Get_State(State::Pos);
				m_pGameInstance->Set_CameraTargetPos(vTargetPos);
				*/

				break;
			}
		}
		if (i >= 4)
		{
			if (m_pSelectDesc)
			{
				Safe_Release(m_pSelectDesc);
			}
			m_pCharacterSelect->Set_Active_Alpha(CCharacterSelect::NONALPHA);
		}

	}




	m_pCharacterSelect->Tick(fTimeDelta);

	if (m_pSelectDesc != nullptr)
	{
		m_pSelectButton->Tick(fTimeDelta);
	}
}

void CSelect::Late_Tick(_float fTimeDelta)
{
	m_pCharacterSelect->Late_Tick(fTimeDelta);
	m_pClassButton->Late_Tick(fTimeDelta);
	if (m_pSelectDesc != nullptr)
	{
		m_pSelectDesc->Late_Tick(fTimeDelta);
		m_pSelectButton->Late_Tick(fTimeDelta);
	}
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
	
	m_pClassButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pClassButton)
	{
		return E_FAIL;
	}


	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_SELECT;
	ColButtonDesc.fDepth = 0.5f;
	ColButtonDesc.fAlpha = 0.7f;
	ColButtonDesc.fFontSize = 0.5f;
	ColButtonDesc.vColor = _vec4(0.2f, 0.2f, 0.4f, 1.f);
	ColButtonDesc.strText = TEXT("선택");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Select_BG_BoxEfc_WhiteBlur");
	ColButtonDesc.vPosition = _vec2(1100.f, 600.f);
	ColButtonDesc.vSize = _vec2(160.f,40.f);
	ColButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vTextPosition = _vec2(0.f, 0.f);

	
	m_pSelectButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pSelectButton)
	{
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CSelect::Bind_ShaderResources()
{

	return S_OK;
}

void CSelect::Set_SelectDesc(_uint iSelect)
{
	if (m_pSelectDesc)
	{
		Safe_Release(m_pSelectDesc);
	}

	CSelectDesc::SELECT_DESC SelectDesc = {};
	SelectDesc.eCharacter = (CSelectDesc::CHARACTER)iSelect;

	m_pSelectDesc = (CSelectDesc*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SelectDesc"), &SelectDesc);
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

	if(m_pSelectDesc!=nullptr)
	Safe_Release(m_pSelectDesc);

}
