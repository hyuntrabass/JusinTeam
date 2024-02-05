#include "Pop_Skill.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "BlurTexture.h"
#include "FadeBox.h"

CPop_Skill::CPop_Skill(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CPop_Skill::CPop_Skill(const CPop_Skill& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CPop_Skill::Init_Prototype()
{
	return S_OK;
}

HRESULT CPop_Skill::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 300.f;
	m_fSizeY = 300.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = 100.f;

	m_fDepth = (_float)D_QUEST / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	_uint iSkillNum{};
	iSkillNum = ((SKILLIN_DESC*)pArg)->iSkillLevel;

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}


	m_fStartButtonPos = dynamic_cast<CTextButton*>(m_pButton)->Get_Position();
	m_fButtonTime = m_fStartButtonPos.y;
	return S_OK;
}

void CPop_Skill::Tick(_float fTimeDelta)
{
	
	if (m_fDeadTime >= 0.8f && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_isDead = true;
	}
	m_fIndex += 36.f * fTimeDelta * 2.f;
	if (m_fIndex >= 36.f)
		m_fIndex = 0.f;

	m_fTime += fTimeDelta * 0.2f;
	m_fDeadTime += fTimeDelta;

	if (dynamic_cast<CTextButton*>(m_pButton)->Get_TransPosition().y <= dynamic_cast<CTextButton*>(m_pButton)->Get_Position().y - 5.f)
	{
		m_fDir = 0.6f;
		dynamic_cast<CTextButton*>(m_pButton)->Set_Position(_float2(dynamic_cast<CTextButton*>(m_pButton)->Get_Position().x, dynamic_cast<CTextButton*>(m_pButton)->Get_Position().y - 5.f));
	}
	if (dynamic_cast<CTextButton*>(m_pButton)->Get_TransPosition().y >= dynamic_cast<CTextButton*>(m_pButton)->Get_Position().y)
	{
		m_fDir = -1.f;
		dynamic_cast<CTextButton*>(m_pButton)->Set_Position(_float2(dynamic_cast<CTextButton*>(m_pButton)->Get_Position().x, dynamic_cast<CTextButton*>(m_pButton)->Get_Position().y));
	}
	m_fButtonTime += fTimeDelta * m_fDir * 10.f;
	dynamic_cast<CTextButton*>(m_pButton)->Set_Position(_float2(dynamic_cast<CTextButton*>(m_pButton)->Get_Position().x, m_fButtonTime));




	m_pBackground->Tick(fTimeDelta);
	m_pButton->Tick(fTimeDelta);


}

void CPop_Skill::Late_Tick(_float fTimeDelta)
{

	m_pButton->Late_Tick(fTimeDelta);
	m_pBorder->Late_Tick(fTimeDelta);
	m_pBackground->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CPop_Skill::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_SpriteMaskTexture)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("��ų ����!"), _vec2((_float)g_iWinSizeX / 2.f - 0.2f, 230.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("��ų ����!"), _vec2((_float)g_iWinSizeX / 2.f + 0.2f, 230.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("��ų ����!"), _vec2((_float)g_iWinSizeX / 2.f, 230.f - 0.2f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("��ų ����!"), _vec2((_float)g_iWinSizeX / 2.f, 230.f + 0.2f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("��ų ����!"), _vec2((_float)g_iWinSizeX / 2.f, 230.f), 0.7f, _vec4(1.f, 1.f, 0.f, 1.f));

	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("ȭ���� ���� �����ϼ���."), _vec2((_float)g_iWinSizeX / 2.f - 0.2f, 580.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("ȭ���� ���� �����ϼ���."), _vec2((_float)g_iWinSizeX / 2.f + 0.2f, 580.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("ȭ���� ���� �����ϼ���."), _vec2((_float)g_iWinSizeX / 2.f, 580.f - 0.2f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("ȭ���� ���� �����ϼ���."), _vec2((_float)g_iWinSizeX / 2.f, 580.f + 0.2f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("ȭ���� ���� �����ϼ���."), _vec2((_float)g_iWinSizeX / 2.f, 580.f), 0.4f, _vec4(0.6f, 0.6f, 0.6f, 1.f));

	return S_OK;
}

HRESULT CPop_Skill::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FX_C_Smoke001_Tex"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FX_C_Smoke001_Tex"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPop_Skill::Add_Parts()
{
	CBlurTexture::BLURTEX_DESC BLURTEXDesc = {};
	BLURTEXDesc.eLevelID = LEVEL_STATIC;
	BLURTEXDesc.fDepth = m_fDepth - 0.01f;
	BLURTEXDesc.fFontSize = 0.f;
	BLURTEXDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ExclamationMark");
	BLURTEXDesc.vPosition = _vec2(m_fX, m_fY);
	BLURTEXDesc.vSize = _vec2(55.f, 60.f);
	BLURTEXDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	BLURTEXDesc.vColor = _vec4(1.0f, 0.5f, 0.1f, 1.f);
	BLURTEXDesc.vTextPosition = _vec2(0.f, 0.f);



	//FX_A_Shine014_Tex
	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_STATIC;
	ButtonDesc.fDepth = m_fDepth;
	ButtonDesc.fFontSize = 0.45f;
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SiegeQuest");
	ButtonDesc.vPosition = _vec2((_float)g_iWinSizeX / 2.f, 360.f);
	ButtonDesc.vSize = _vec2(360.f, 10.f);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 12.f);
	m_pBorder = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pBorder)
	{
		return E_FAIL;
	}

	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ClickArrow");
	ButtonDesc.vPosition = _vec2(780.f, 585.f);
	ButtonDesc.vSize = _vec2(30.f, 30.f);

	m_pButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pButton)
	{
		return E_FAIL;
	}


	CFadeBox::FADE_DESC Desc = {};
	Desc.eState = CFadeBox::FADELOOP;
	Desc.fDuration = 0.f;
	m_pBackground = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_FadeBox"), &Desc);
	if (not m_pBackground)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPop_Skill::Bind_ShaderResources()
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
	/*
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fTime, sizeof(_float))))
	{
		return E_FAIL;
	}	
	*/
	_int2 vNumSprite = { 6, 6 };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof(_int2))))
	{
		return E_FAIL;
	}
	int iIndex = static_cast<_int>(m_fIndex);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	return S_OK;
}

CPop_Skill* CPop_Skill::Create(_dev pDevice, _context pContext)
{
	CPop_Skill* pInstance = new CPop_Skill(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPop_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPop_Skill::Clone(void* pArg)
{
	CPop_Skill* pInstance = new CPop_Skill(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPop_Skill");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPop_Skill::Free()
{
	__super::Free();


	Safe_Release(m_pBorder);

	Safe_Release(m_pBackground);
	Safe_Release(m_pButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
