#include "InfinityStart.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "BlurTexture.h"
#include "FadeBox.h"
#include "Skill.h"

CInfinityStart::CInfinityStart(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CInfinityStart::CInfinityStart(const CInfinityStart& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CInfinityStart::Init_Prototype()
{
	return S_OK;
}

HRESULT CInfinityStart::Init(void* pArg)
{
	m_eTower = ((STARTGAME_DESC*)pArg)->eTower;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 1300.f;
	m_fSizeY = 1300.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = (_float)g_iWinSizeY / 2.f - 50.f;

	m_fDepth = (_float)D_WINDOW / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);




	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	m_fBright = 12.f;
	m_fSmokeSize = m_pSmoke->Get_Size().x;
	m_pGameInstance->Play_Sound(TEXT("Skill_Open"), 0.5f, false, 0.4f);
	return S_OK;
}

void CInfinityStart::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime >= 1.f && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_isDead = true;
		return;
	}

	if (m_fSizeX <= 500.f)
	{
		m_fSizeX = 500.f;
		m_bStartEffect = true;
	}
	if(!m_bStartEffect)
	{
		m_fSizeX -= 50.f; 
		m_fSizeY -= 50.f; 
	}
	if (m_bStartEffect)
	{
		if (m_fBright >= 4.f)
		{
			m_fBright -= fTimeDelta * 30.f;
		}

		m_fStop += fTimeDelta;
		if (m_fStop >= 4.f)
		{
			m_fStop = 0.f;
		}
		else if (m_fStop >= 2.f)
		{
			m_fSmokeSize += fTimeDelta * 50.f;
		}
		else
		{
			m_fSmokeSize -= fTimeDelta * 50.f;
		}
		m_pSmoke->Set_Size(m_fSmokeSize, m_fSmokeSize);

	}


	m_fTime += fTimeDelta * 0.2f;

	m_pSmoke->Tick(fTimeDelta);
	m_pPattern->Tick(fTimeDelta);
	m_pFade->Tick(fTimeDelta);

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CInfinityStart::Late_Tick(_float fTimeDelta)
{

	m_pSmoke->Late_Tick(fTimeDelta);
	m_pPattern->Late_Tick(fTimeDelta);
	m_pFade->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CInfinityStart::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Bright)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	_uint iTower = (_uint)m_eTower + 1;
	wstring strText = TEXT("무한의 탑 ") + to_wstring(iTower) + TEXT("층 관문");
	_float fPosY = m_fY - 390.f;
	m_pGameInstance->Render_Text(L"Font_Malang", strText, _vec2(m_fX + 1.f, m_fY + fPosY), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", strText, _vec2(m_fX, m_fY + fPosY + 1.f), 0.35f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", strText, _vec2(m_fX, m_fY + fPosY), 0.35f, _vec4(1.f, 1.f, 1.f, 1.f));
	
	fPosY += 180.f;
	m_pGameInstance->Render_Text(L"Font_Malang", m_strGameName, _vec2(m_fX + 1.f, m_fY + fPosY), 0.8f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strGameName, _vec2(m_fX, m_fY + fPosY + 1.f), 0.8f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strGameName, _vec2(m_fX, m_fY + fPosY), 0.8f, _vec4(1.f, 0.74f, 0.f, 1.f));

	return S_OK;
}

HRESULT CInfinityStart::Add_Components()
{
	wstring strTex = TEXT("Prototype_Component_Texture_UI_Gameplay_Gacha3");
	switch (m_eTower)
	{
	case Client::SURVIVAL:
		m_strGameName = TEXT("서바이벌");
		break;
	case Client::BOSS1:
		m_strGameName = TEXT("잠입보스");
		strTex = TEXT("Prototype_Component_Texture_UI_Gameplay_Gacha4");
		break;
	case Client::BRICK:
		m_strGameName = TEXT("묘신의 시험");
		break;
	case Client::CESCO:
		m_strGameName = TEXT("벌레잡기");
		break;
	case Client::BOSS2:
		m_strGameName = TEXT("최종보스");
		strTex = TEXT("Prototype_Component_Texture_UI_Gameplay_Gacha4");
		break;
	case Client::TOWER_END:
		break;
	default:
		break;
	}
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strTex, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Noise_04"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CInfinityStart::Add_Parts()
{

	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.fDepth = m_fDepth + 0.01f;
	TextButton.fAlpha = 0.8f;
	TextButton.fFontSize = 0.35f;
	TextButton.strText = TEXT("");
	TextButton.vPosition = _vec2(m_fX, m_fY);
	TextButton.vSize = _vec2(500.f, 500.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Gacha1");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pPattern = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pPattern)
	{
		return E_FAIL;
	}
	m_pPattern->Set_Pass(VTPass_UI_Alpha);



	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 0.2f;
	TextButton.fFontSize = 0.35f;
	TextButton.vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.vPosition = _vec2(m_fX, m_fY);
	TextButton.vSize = _vec2(600.f, 600.f);
	TextButton.strTexture = TEXT("Prototype_Component_Texture_Effect_FX_B_Glow003_Tex");
	//TextButton.strTexture2 = TEXT("Prototype_Component_Texture_Effect_FX_J_Dust001_Tex");
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	m_pSmoke = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pSmoke)
	{
		return E_FAIL;
	}
	m_pSmoke->Set_Pass(VTPass_Mask_ColorAlpha);

	CFadeBox::FADE_DESC Desc = {};
	Desc.fMaxAlpha = 0.9f;
	Desc.isInfiniteLoop = true;
	Desc.fDepth = (m_fDepth + 0.05f);
	Desc.fIn_Duration = 0.5f;
	m_pFade = CUI_Manager::Get_Instance()->Clone_FadeBox(Desc);
	if (not m_pFade)
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CInfinityStart::Bind_ShaderResources()
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

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}
	_float fTemp = 0.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fTemp, sizeof(_float))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightFactor", &m_fBright, sizeof(_float))))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

CInfinityStart* CInfinityStart::Create(_dev pDevice, _context pContext)
{
	CInfinityStart* pInstance = new CInfinityStart(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CInfinityStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInfinityStart::Clone(void* pArg)
{
	CInfinityStart* pInstance = new CInfinityStart(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CInfinityStart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInfinityStart::Free()
{
	__super::Free();

	Safe_Release(m_pSmoke);
	Safe_Release(m_pPattern);
	Safe_Release(m_pFade);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
