#include "HPMonster.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "3DUITex.h"

CHPMonster::CHPMonster(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CHPMonster::CHPMonster(const CHPMonster& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CHPMonster::Init_Prototype()
{
	return S_OK;
}

HRESULT CHPMonster::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_iMaxHp = ((HP_DESC*)pArg)->iMaxHp;
	m_eLevel = ((HP_DESC*)pArg)->eLevelID;
	m_vPosition = ((HP_DESC*)pArg)->vPosition;
	m_pParentTransform = ((HP_DESC*)pArg)->pParentTransform;
	Safe_AddRef(m_pParentTransform);

	m_fSizeX = 60.f;
	m_fSizeY = 7.f;
	m_fDepth = (_float)D_NAMETAG / (_float)D_END;


	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	/*
	
	


	C3DUITex::UITEX_DESC Desc = {};
	Desc.eLevelID = m_eLevel;
	Desc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG");
	Desc.vPosition = _vec3(m_vPosition.x, m_vPosition.y, + 0.01f);
	Desc.pParentTransform = m_pParentTransform;
	Desc.vSize = _vec2(m_fSizeX + 2.f, m_fSizeY + 2.f);
	m_pBackground = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &Desc);

	if (not m_pBackground)
	{
		return E_FAIL;
	}
	*/

	return S_OK;
}

void CHPMonster::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_5))
		m_iCurHp -= 0.1f;;
	if (m_pGameInstance->Key_Pressing(DIK_6))
	{
		if (m_iCurHp <= m_iMaxHp)
			m_iCurHp += 0.1f;;
	}

	if (m_iCurHp <= 0)
	{
		m_iCurHp = 0;
	}


	m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vPosition);	
	_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
	m_fX = v2DPos.x;
	m_fY = v2DPos.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	//m_pBackground->Tick(fTimeDelta);
}

void CHPMonster::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	//m_pBackground->Late_Tick(fTimeDelta);
}

HRESULT CHPMonster::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_HPNoMask)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHPMonster::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_HPBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHPMonster::Bind_ShaderResources()
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

	_float fRatio = (_float)m_iCurHp / (_float)m_iMaxHp;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpRatio", &fRatio, sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

CHPMonster* CHPMonster::Create(_dev pDevice, _context pContext)
{
	CHPMonster* pInstance = new CHPMonster(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHPMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHPMonster::Clone(void* pArg)
{
	CHPMonster* pInstance = new CHPMonster(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHPMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHPMonster::Free()
{
	__super::Free();

	//Safe_Release(m_pBackground);
	//Safe_Release(m_pParentTransform);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
