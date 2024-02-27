#include "Effect_Sphere.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"

static _int iID = 0;

CEffect_Sphere::CEffect_Sphere(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffect_Sphere::CEffect_Sphere(const CEffect_Sphere& rhs)
	: CGameObject(rhs)
{
	m_iID = iID++;
}

void CEffect_Sphere::Select(const _bool& isSelected)
{
	m_isSelected = isSelected;
}


void CEffect_Sphere::Get_State(_float4& vPos, _float4& vLook)
{
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(State::Pos));
	XMStoreFloat4(&vLook, m_pTransformCom->Get_State(State::Look));
}


HRESULT CEffect_Sphere::Init_Prototype()
{

	return S_OK;
}

HRESULT CEffect_Sphere::Init(void* pArg)
{
	m_Info = *(EffectDummyInfo*)pArg;	

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (m_Info.ppDummy)
	{
		*m_Info.ppDummy = this;
		m_Info.ppDummy = nullptr;
	}
	//m_Info.mMatrix = _mat::CreateScale(m_Info.fSize);

	mMatrix = &m_Info.mMatrix;

	m_pTransformCom->Set_Matrix(m_Info.mMatrix);

	CEffect_Manager::Get_Instance()->Create_Effect(m_Info.EffectName, mMatrix, true);

	return S_OK;
}

void CEffect_Sphere::Tick(_float fTimeDelta)
{
	*mMatrix = m_pTransformCom->Get_World_Matrix();

}

void CEffect_Sphere::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
}

HRESULT CEffect_Sphere::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			continue;
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelected", &m_isSelected, sizeof _bool)))
		{
			return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Begin(m_iOutLineShaderPass)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
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

HRESULT CEffect_Sphere::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	m_iShaderPass = StaticPass_COLMesh;
	m_iOutLineShaderPass = StaticPass_OutLine;
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Collider"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffect_Sphere::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iID", &m_iID, sizeof _int)))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
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
	/*_float4 vColor{ 0.3f, 0.8f, 0.3f, 0.5f };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _float4)))
	{
		return E_FAIL;
	}

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_EDITOR, TEXT("Light_Main"));
	if (!pLightDesc)
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof _float4)))
	{
		return E_FAIL;
	}*/
	return S_OK;
}

CEffect_Sphere* CEffect_Sphere::Create(_dev pDevice, _context pContext)
{
	CEffect_Sphere* pInstance = new CEffect_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Sphere::Clone(void* pArg)
{
	CEffect_Sphere* pInstance = new CEffect_Sphere(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CEffect_Sphere::Free()
{
	__super::Free();
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}