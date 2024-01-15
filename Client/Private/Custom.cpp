#include "Custom.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CCustom::CCustom(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CCustom::CCustom(const CCustom& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCustom::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CCustom::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Models()))
	{
		return E_FAIL;
	}
	
	if (FAILED(Init_Menu()))
	{
		return E_FAIL;
	}



	return S_OK;
}

void CCustom::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	//싱글톤으로 매니저 만들면 현재 선택한거 이때 넘길때 ㅇ

	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && PtInRect(&m_pCustomMenu[C_FACE]->Get_Rect(), ptMouse))
	{
		m_isMenuClick[C_FACE] = true;
		m_isMenuClick[C_HAIR] = false;
		m_pSelectCustomEffect->Set_Position(_vec2(-200, 0));
		m_pSelectMenuEffect->Set_Position(_vec2(m_pCustomMenu[C_HAIR]->Get_Position().x, m_pCustomMenu[C_FACE]->Get_Position().y + 8.f));
	}
	else if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI) && PtInRect(&m_pCustomMenu[C_HAIR]->Get_Rect(), ptMouse))
	{
		m_isMenuClick[C_HAIR] = true;
		m_isMenuClick[C_FACE] = false;
		m_pSelectCustomEffect->Set_Position(_vec2(-200, 0));
		m_pSelectMenuEffect->Set_Position(_vec2(m_pCustomMenu[C_HAIR]->Get_Position().x, m_pCustomMenu[C_HAIR]->Get_Position().y + 8.f));
	}

	if (m_isMenuClick[C_FACE])
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::GamePlay))
		{
			for (_int i = 0; i < 6; i++)
			{
				RECT rcRect = {};

				rcRect = {
					  (LONG)(m_FacePos[i].x - 79.f * 0.5f),
					  (LONG)(m_FacePos[i].y - 87.f * 0.5f),
					  (LONG)(m_FacePos[i].x + 79.f * 0.5f),
					  (LONG)(m_FacePos[i].y + 87.f * 0.5f)
				};
				if (PtInRect(&rcRect, ptMouse))
				{
					m_pSelectCustomEffect->Set_Position(m_FacePos[i]);
					break;
				}

			}
		}
		
	}
	else if (m_isMenuClick[C_HAIR])
	{
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::GamePlay))
		{
			for (_int i = 0; i < 9; i++)
			{
				RECT rcRect = {};

				rcRect = {
					  (LONG)(m_HairPos[i].x - 79.f * 0.5f),
					  (LONG)(m_HairPos[i].y - 87.f * 0.5f),
					  (LONG)(m_HairPos[i].x + 79.f * 0.5f),
					  (LONG)(m_HairPos[i].y + 87.f * 0.5f)
				};
				if (PtInRect(&rcRect, ptMouse))
				{
					m_pSelectCustomEffect->Set_Position(m_HairPos[i]);
					break;
				}

			}
		}
	}
	
	if (m_pGameInstance->Mouse_Down(DIM_RBUTTON, InputChannel::Editor) && PtInRect(&m_pSelectButton->Get_Rect(), ptMouse))
	{
		m_pGameInstance->Level_ShutDown(LEVEL_CUSTOM);
	}
	
	
	m_pSelectCustomEffect->Tick(fTimeDelta);
	if (m_pSelectButton != nullptr)
	{
		m_pSelectButton->Tick(fTimeDelta);
	}
}


void CCustom::Late_Tick(_float fTimeDelta)
{
	m_pSelectMenuEffect->Late_Tick(fTimeDelta);
	if (m_isMenuClick[C_FACE])
	{

		
		m_pFaceGroup->Late_Tick(fTimeDelta);
	}
	else if (m_isMenuClick[C_HAIR])
	{
		m_pHairGroup->Late_Tick(fTimeDelta);
	}

	m_pSelectCustomEffect->Late_Tick(fTimeDelta);

	for (size_t i = 0; i < C_END; i++)
	{
		m_pCustomMenu[i]->Late_Tick(fTimeDelta);
	}

	m_pClassButton->Late_Tick(fTimeDelta);
	if (m_pSelectButton != nullptr)
	{
		m_pSelectButton->Late_Tick(fTimeDelta);
	}
}

HRESULT CCustom::Render()
{

	return S_OK;
}

HRESULT CCustom::Add_Components()
{
	
	return S_OK;
}

HRESULT CCustom::Add_Parts()
{

	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_CUSTOM;
	ButtonDesc.fDepth = 0.5f;
	ButtonDesc.fFontSize = 0.4f;
	ButtonDesc.strText = TEXT("커스터마이징");
	ButtonDesc.strTexture = TEXT("");
	ButtonDesc.vPosition = _vec2(50.f, 15.f);
	ButtonDesc.vSize = _vec2(20.f, 20.f);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 0.f);
	
	m_pClassButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pClassButton)
	{
		return E_FAIL;
	}

	ButtonDesc.strText = TEXT("얼굴");
	ButtonDesc.vTextPosition = _vec2(0.f, 40.f);
	ButtonDesc.fFontSize = 0.3f;
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_Face");
	ButtonDesc.vPosition = _vec2(50.f, 120.f);
	ButtonDesc.vSize = _vec2(60.f, 60.f);

	m_pCustomMenu[C_FACE] = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pCustomMenu[C_FACE])
	{
		return E_FAIL;
	}	

	ButtonDesc.strText = TEXT("헤어");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_Hair");
	ButtonDesc.vPosition = _vec2(50.f, 220.f);

	m_pCustomMenu[C_HAIR] = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pCustomMenu[C_HAIR])
	{
		return E_FAIL;
	}

	//그룹사진
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_CustomFace2");
	ButtonDesc.vPosition = _vec2(1130.f, 180.f);
	ButtonDesc.vSize = _vec2(240.f, 266.f);
	m_pFaceGroup = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pFaceGroup)
	{
		return E_FAIL;
	}

	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_CustomHair2");
	m_pHairGroup = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pHairGroup)
	{
		return E_FAIL;
	}
	
	//이펙트
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_SelectEffect");
	ButtonDesc.vPosition = _vec2(-200.f, 15.f);
	ButtonDesc.vSize = _vec2(79.f, 87.f); 
 	m_pSelectCustomEffect = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	
	
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Custom_FadeBox");
	ButtonDesc.vPosition = _vec2(-200.f, 15.f);
	ButtonDesc.vSize = _vec2(100.f, 80.f);
	m_pSelectMenuEffect = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pSelectMenuEffect)
	{
		return E_FAIL;
	}


	//캐릭터 생성
	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = 0.5f;
	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.fFontSize = 0.35f;
	ColButtonDesc.vColor = _vec4(0.2f, 0.2f, 0.2f, 0.5f);
	ColButtonDesc.strText = TEXT("캐릭터 생성");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Select_BG_BoxEfc_WhiteBlur");
	ColButtonDesc.vPosition = _vec2(1125.f, 670.f);
	ColButtonDesc.vSize = _vec2(150.f,30.f);
	ColButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vTextPosition = _vec2(0.f, 0.f);

	
	m_pSelectButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pSelectButton)
	{
		return E_FAIL;  
	}

	return S_OK;
}

HRESULT CCustom::Add_Models()
{

	return S_OK;
}

HRESULT CCustom::Init_Menu()
{
	for (size_t i = 0; i < C_END; i++)
		m_isMenuClick[i] = false;

	m_HairPos[0] = _vec2(1050, 104);
	m_HairPos[1] = _vec2(1130, 104);
	m_HairPos[2] = _vec2(1210, 104);
	m_HairPos[3] = _vec2(1050, 188);
	m_HairPos[4] = _vec2(1130, 188);
	m_HairPos[5] = _vec2(1210, 188);
	m_HairPos[6] = _vec2(1050, 272);
	m_HairPos[7] = _vec2(1130, 272);
	m_HairPos[8] = _vec2(1210, 272);

		

	m_FacePos[0] = _vec2(1050, 104);
	m_FacePos[1] = _vec2(1130, 104);
	m_FacePos[2] = _vec2(1210, 104);
	m_FacePos[3] = _vec2(1050, 188);
	m_FacePos[4] = _vec2(1130, 188);
	m_FacePos[5] = _vec2(1210, 188);

	return S_OK;
}

HRESULT CCustom::Bind_ShaderResources()
{

	return S_OK;
}


void CCustom::Set_CameraState(_uint iSelect)
{
	m_pGameInstance->Set_CameraState(CM_ZOOM);
	m_pGameInstance->Set_ZoomFactor(3.5f);
}

CCustom* CCustom::Create(_dev pDevice, _context pContext)
{
	CCustom* pInstance = new CCustom(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCustom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCustom::Clone(void* pArg)
{
	CCustom* pInstance = new CCustom(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCustom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustom::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < C_END; i++)
		{
			Safe_Release(m_pCustomMenu[i]);
		}
	}

	Safe_Release(m_pClassButton);
	Safe_Release(m_pSelectButton);

	Safe_Release(m_pFaceGroup);
	Safe_Release(m_pHairGroup);
	Safe_Release(m_pSelectMenuEffect);
	Safe_Release(m_pSelectCustomEffect);
}
