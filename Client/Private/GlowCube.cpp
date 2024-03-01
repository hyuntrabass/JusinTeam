#include "GlowCube.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "GameInstance.h"
#include "Collider.h"
CGlowCube::CGlowCube(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CGlowCube::CGlowCube(const CGlowCube& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGlowCube::Init_Prototype()
{
	return S_OK;
}

HRESULT CGlowCube::Init(void* pArg)
{
	m_isDefault = ((GLOWCUBE_DESC*)pArg)->isDefault;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_vPos = ((GLOWCUBE_DESC*)pArg)->vPos;
	m_vColor = ((GLOWCUBE_DESC*)pArg)->vColor;
	m_pParentTransform = ((GLOWCUBE_DESC*)pArg)->pParentTransform;
	Safe_AddRef(m_pParentTransform);

	m_pTransformCom->Set_Scale(_vec3(0.02f, 0.02f, 0.02f));
	//m_pTransformCom->Set_Scale(_vec3(1.2f, 1.2f, 1.2f));
	m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vPos);

	m_shouldRenderBlur = true;
	return S_OK;
}

void CGlowCube::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scale(_vec3(0.015f, 0.015f, 0.015f));

	m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vPos);
}

void CGlowCube::Late_Tick(_float fTimeDelta)
{

	if (m_isDefault)
	{
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
	}
	else
	{
		m_shouldRenderBlur = true;
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Blend, this);
	}

}

HRESULT CGlowCube::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_bool bFailed = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}
		_bool isBlur = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}

		_bool isFalse = { false };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &isFalse, sizeof _bool)))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &isFalse, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelected", &isFalse, sizeof _bool)))
		{
			return E_FAIL;
		}

		_float fAlpha = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof _float)))
		{
			return E_FAIL;
		}

		_vec2 vUV = _vec2(m_fX, 0.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUV, sizeof _vec2)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CGlowCube::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	if (m_isDefault)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_BrickCube"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_cubeone"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGlowCube::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CGlowCube* CGlowCube::Create(_dev pDevice, _context pContext)
{
	CGlowCube* pInstance = new CGlowCube(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGlowCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGlowCube::Clone(void* pArg)
{
	CGlowCube* pInstance = new CGlowCube(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGlowCube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGlowCube::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pShaderCom);
}
