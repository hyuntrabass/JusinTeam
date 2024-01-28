#include "Arrow.h"

CArrow::CArrow(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CArrow::CArrow(const CArrow& rhs)
	: CGameObject(rhs)
{
}

HRESULT CArrow::Init_Prototype()
{
    return S_OK;
}

HRESULT CArrow::Init(void* pArg)
{
	
	Arrow_Type* type = (Arrow_Type*)pArg;
	
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	m_pTransformCom->Set_Matrix(type->world);
	m_pTransformCom->LookAt_Dir(type->vLook);
	m_pTransformCom->Set_Speed(35.f);

	TRAIL_DESC trail_desc{};
	trail_desc.vColor = _vec4(0.f, 0.6f, 1.f, 1.f);
	trail_desc.vPSize = _vec2(0.02f, 0.02f);
	trail_desc.iNumVertices = 10;
	m_pTrail = (CCommonTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonTrail"), &trail_desc);
    return S_OK;
}

void CArrow::Tick(_float fTimeDelta)
{
	
	m_pTransformCom->Go_Straight(fTimeDelta);
	_mat world{};
	world = m_pTransformCom->Get_World_Matrix();
	m_pTrail->Tick((world.Position_vec3()));

}

void CArrow::Late_Tick(_float fTimeDelta)
{
	m_pTrail->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CArrow::Render()
{


	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}


	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
	
		if (FAILED(m_pShaderCom->Begin(StaticPass_SingleColorFx)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}
}
HRESULT CArrow::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Arrow") , TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CArrow::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
	//	return E_FAIL;

	//if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	//{
	//	return E_FAIL;
	//}

	_vec4 Color = _vec4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &Color, sizeof _vec4)))
	{
		return E_FAIL;
	}

	return S_OK;
}



CArrow* CArrow::Create(_dev pDevice, _context pContext)
{
	CArrow* pInstance = new CArrow(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArrow::Clone(void* pArg)
{
	CArrow* pInstance = new CArrow(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArrow::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTrail);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);


}
