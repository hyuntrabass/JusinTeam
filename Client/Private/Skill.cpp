#include "Skill.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "ItemInfo.h"

CSkill::CSkill(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSkill::CSkill(const CSkill& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSkill::Init_Prototype()
{
	return S_OK;
}

HRESULT CSkill::Init(void* pArg)
{

	m_tSkillInfo = ((SKILL_DESC*)pArg)->tSkillInfo;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = ((SKILL_DESC*)pArg)->vSize.x;
	m_fSizeY = ((SKILL_DESC*)pArg)->vSize.y;

	m_fX = ((SKILL_DESC*)pArg)->vPosition.x;
	m_fY = ((SKILL_DESC*)pArg)->vPosition.y;

	m_fDepth = ((SKILL_DESC*)pArg)->fDepth;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	return S_OK;
}

void CSkill::Tick(_float fTimeDelta)
{
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CSkill::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CSkill::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CSkill::Set_Position(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;	
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

}

HRESULT CSkill::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_tSkillInfo.strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CSkill::Bind_ShaderResources()
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

	return S_OK;
}

CSkill* CSkill::Create(_dev pDevice, _context pContext)
{
	CSkill* pInstance = new CSkill(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill::Clone(void* pArg)
{
	CSkill* pInstance = new CSkill(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
