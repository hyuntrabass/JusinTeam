#include "3DUITex.h"
#include "GameInstance.h"
#include "UI_Manager.h"

C3DUITex::C3DUITex(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

C3DUITex::C3DUITex(const C3DUITex& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT C3DUITex::Init_Prototype()
{
	return S_OK;
}

HRESULT C3DUITex::Init(void* pArg)
{



	m_eLevel = ((UITEX_DESC*)pArg)->eLevelID;
	m_strTexture = ((UITEX_DESC*)pArg)->strTexture;
	m_vPosition = ((UITEX_DESC*)pArg)->vPosition;
	m_pParentTransform = ((UITEX_DESC*)pArg)->pParentTransform;
	Safe_AddRef(m_pParentTransform);

	m_fSizeX = ((UITEX_DESC*)pArg)->vSize.x;
	m_fSizeY = ((UITEX_DESC*)pArg)->vSize.y;
	m_fDepth = (_float)D_NAMETAG / (_float)D_END + 0.1f;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	return S_OK;
}

void C3DUITex::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vPosition);	
	_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
	m_fX = v2DPos.x;
	m_fY = v2DPos.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void C3DUITex::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT C3DUITex::Render()
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

HRESULT C3DUITex::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT C3DUITex::Bind_ShaderResources()
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

C3DUITex* C3DUITex::Create(_dev pDevice, _context pContext)
{
	C3DUITex* pInstance = new C3DUITex(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : C3DUITex");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C3DUITex::Clone(void* pArg)
{
	C3DUITex* pInstance = new C3DUITex(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : C3DUITex");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C3DUITex::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
