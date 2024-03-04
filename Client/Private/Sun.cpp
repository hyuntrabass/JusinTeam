#include "Sun.h"

CSun::CSun(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSun::CSun(const CSun& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSun::Init_Prototype()
{
	return S_OK;
}

HRESULT CSun::Init(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	_vec3 vScale = _vec3(0.1f, 0.1f, 1.f);
	m_pTransformCom->Set_Scale(vScale);

	return S_OK;
}

void CSun::Tick(_float fTimeDelta)
{
}

void CSun::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_Sun, this);
}

HRESULT CSun::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	_vec4 vCamPos = m_pGameInstance->Get_CameraPos();

	LIGHT_DESC* Light_Desc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");

	if (nullptr == Light_Desc)
		return S_OK;

	_vec4 vDir = Light_Desc->vDirection.Get_Normalized();

	_vec4 vPos = vCamPos + (-vDir);

	m_pTransformCom->Set_State(State::Pos, vPos);

	m_pTransformCom->LookAway(vCamPos);

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (m_pTexturesCom->Bind_ShaderResource(m_pShaderCom, "g_Texture"))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(37)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSun::Render_Reflection(_float4 vClipPlane)
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	LIGHT_DESC* Light_Desc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");

	if (nullptr == Light_Desc)
		return S_OK;

	_mat ReflectCamWorldMat = m_pGameInstance->Get_Transform_Inversed(TransformType::View);

	_vec3 vRefelctCamPos = ReflectCamWorldMat.Position_vec3();

	m_pTransformCom->LookAt(vRefelctCamPos);

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vClipPlane", &vClipPlane, sizeof(_float4))))
		return E_FAIL;

	if (m_pTexturesCom->Bind_ShaderResource(m_pShaderCom, "g_Texture"))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSun::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	// For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	// 태양 텍스처를 차자라
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Texture_Sun", L"Com_Texture", (CComponent**)&m_pTexturesCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSun::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSun* CSun::Create(_dev pDevice, _context pContext)
{
	CSun* pInstance = new CSun(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSun::Clone(void* pArg)
{
	CSun* pInstance = new CSun(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSun");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSun::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTexturesCom);
	Safe_Release(m_pVIBufferCom);
}
