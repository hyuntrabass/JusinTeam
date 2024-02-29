#include "NumEffect.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "3DUITex.h"

CNumEffect::CNumEffect(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CNumEffect::CNumEffect(const CNumEffect& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CNumEffect::Init_Prototype()
{
	return S_OK;
}

HRESULT CNumEffect::Init(void* pArg)
{

	m_bOrth = ((NUMEFFECT_DESC*)pArg)->bOrth;
	m_fDepth = (_float)D_WINDOW / (_float)D_END;

	m_iCurNum = ((NUMEFFECT_DESC*)pArg)->iDamage;
	m_vTextPosition = ((NUMEFFECT_DESC*)pArg)->vTextPosition;
	//m_vColor = ((NAMETAG_DESC*)pArg)->vColor;HITEFFECT_DESC
	m_pParentTransform = ((NUMEFFECT_DESC*)pArg)->pParentTransform;
	if (!m_bOrth)
	{
		Safe_AddRef(m_pParentTransform);
	}

	m_fX = m_vTextPosition.x;
	m_fY = m_vTextPosition.y;

	m_fSizeX = 40.f;
	m_fSizeY = 40.f;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}	

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = m_fDepth - 0.01f;
	ColButtonDesc.fAlpha = 1.;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_Effect_FX_A_Shine003_Tex");
	ColButtonDesc.strTexture2 = TEXT("Prototype_Component_Texture_Effect_FX_A_Shine003_Tex");
	ColButtonDesc.vSize = _vec2(20.f, 10.f);
	ColButtonDesc.vPosition = _vec2(m_fX, m_fY);

	m_pShineEffect = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pShineEffect)
	{
		return E_FAIL;
	}
	m_pShineEffect->Set_Pass(VTPass_Mask_Texture);

	m_vColor = _vec4(0.94f, 0.34f, 0.1f, 1.f);
	
	return S_OK;
}

void CNumEffect::Tick(_float fTimeDelta)
{
	if (m_isChanging)
	{
		if (m_fSizeX <= 40.f)
		{
			m_isChanging = false;
			m_fSizeX = 40.f;
			m_fSizeY = 40.f;
			m_fAlpha = 1.f;
			return;
		}		
		m_fAlpha = Lerp(0.f, 1.f, (300.f - m_fSizeX) / 300.f);
		m_fSizeX -= 300.f * fTimeDelta;
		m_fSizeY -= 300.f * fTimeDelta;
	}

	/*
	
	if (m_iCurNum >= 5.f)
	{
		m_vColor = _vec4(0.94f, 0.34f, 0.1f, 1.f);
	}
	else if (m_iCurNum >= 10.f)
	{
		m_vColor = _vec4(1.f, 0.8f, 0.f, 1.f);
	}
	else
	{
		m_vColor.w = 0.f;
	}
	*/
	m_vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	if (m_isEffect)
	{
		if (m_pShineEffect->Get_Size().y >= 120.f)
		{
			m_isEffect = false;
		}
		m_pShineEffect->Set_Size(m_pShineEffect->Get_Size().x + 40.f, m_pShineEffect->Get_Size().y + 30.f);
		m_pShineEffect->Tick(fTimeDelta);
	}

}

void CNumEffect::Late_Tick(_float fTimeDelta)
{
	if (m_isEffect)
	{

			m_pShineEffect->Late_Tick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CNumEffect::Render()
{
	if (!m_bOrth)
	{
		m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
		_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
		m_fX = v2DPos.x;
		m_fY = v2DPos.y;
	}
	else
	{
		m_fX = m_vTextPosition.x;
		m_fY = m_vTextPosition.y;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCurNum % 10)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_LerpColorNAlpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	// 10ÀÚ¸® ¼ö.
	if (m_iCurNum > 9)
	{
		if (!m_bOrth)
		{
			m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
			_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
			m_fX = v2DPos.x - 15.f;
			m_fY = v2DPos.y;
		}
		else
		{
			m_fX = m_vTextPosition.x - 20.f;
			m_fY = m_vTextPosition.y;
		}

		__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (m_iCurNum % 100) / 10)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(VTPass_LerpColorNAlpha)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pVIBufferCom->Render()))
		{
			return E_FAIL;
		}

		// 100ÀÚ¸® ¼ö.
		if (m_iCurNum > 99)
		{
			if (!m_bOrth)
			{
				m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
				_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
				m_fX = v2DPos.x - 30.f;
				m_fY = v2DPos.y;
			}
			else
			{
				m_fX = m_vTextPosition.x - 40.f;
				m_fY = m_vTextPosition.y;
			}
			__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

			if (FAILED(Bind_ShaderResources()))
			{
				return E_FAIL;
			}
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iCurNum / 100)))
			{
				return E_FAIL;
			}
			if (FAILED(m_pShaderCom->Begin(VTPass_LerpColorNAlpha)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pVIBufferCom->Render()))
			{
				return E_FAIL;
			}

		}
	}
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("ÄÞº¸"), _vec2(m_fX + 50.f + 1.f, m_fY + 1.f), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", TEXT("ÄÞº¸"), _vec2(m_fX + 50.f, m_fY), 0.5f, _vec4(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

HRESULT CNumEffect::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NumDamage"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNumEffect::Bind_ShaderResources()
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


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
	{
		return E_FAIL;
	}	
	
	m_fAlpha = 1.f;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

void CNumEffect::Set_TargetNum(_uint iNum)
{
	if (m_iCurNum != iNum)
	{
		m_iCurNum = iNum;
		m_iTargetNum = m_iCurNum;
		if (m_iCurNum >= 10)
		{
			m_fSizeX = 100.f;
			m_fSizeY = 100.f;
			m_isChanging = true;
		}
		m_pShineEffect->Set_Size(20.f, 10.f);
		m_isEffect = true;
	}
}

CNumEffect* CNumEffect::Create(_dev pDevice, _context pContext)
{
	CNumEffect* pInstance = new CNumEffect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNumEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNumEffect::Clone(void* pArg)
{
	CNumEffect* pInstance = new CNumEffect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNumEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNumEffect::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
	Safe_Release(m_pParentTransform);
	Safe_Release(m_pShineEffect);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
