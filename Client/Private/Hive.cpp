#include "Hive.h"

CHive::CHive(_dev pDevice, _context pContext)
    :CGameObject(pDevice, pContext)
{
}

CHive::CHive(const CHive& rhs)
    :CGameObject(rhs)
{
}

HRESULT CHive::Init_Prototype()
{
    return S_OK;
}

HRESULT CHive::Init(void* pArg)
{
    if (pArg)
    {
        HIVE_DESC* pHiveDesc = reinterpret_cast<HIVE_DESC*>(pArg);

        m_pTransformCom->Set_Position(pHiveDesc->vPosition);
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pGameInstance->Register_CollisionObject(this, m_pColliderCom);

    m_iPassIndex = StaticPass_Dissolve;
    m_fDissolveRatio = 1.f;

    m_iHP = 300;

    return S_OK;
}

void CHive::Tick(_float fTimeDelta)
{
    m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());

    if (not m_HasCreated)
    {
        m_fDissolveRatio -= fTimeDelta;
        if (m_fDissolveRatio <= 0.f)
        {
            m_fDissolveRatio = 0.f;
            m_iPassIndex = StaticPass_Default;
            m_HasCreated = true;
        }
    }

    if (m_iHP <= 0)
    {
        m_iPassIndex = StaticPass_Dissolve;
        if (m_fDissolveRatio <= 1.f)
        {
            m_fDissolveRatio += fTimeDelta;
            if (m_fDissolveRatio >= 1.f)
            {
                Kill();
            }
        }
    }
}

void CHive::Late_Tick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
#ifdef _DEBUG
    m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // DEBUG
}

HRESULT CHive::Render()
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

        if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
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

HRESULT CHive::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_TOWER, TEXT("Prototype_Model_Hive"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
    {
        return E_FAIL;
    }

	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::Sphere;
    ColliderDesc.fRadius = 1.5f;
    ColliderDesc.vCenter = _vec3(0.f, ColliderDesc.fRadius / -2.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

    return S_OK;
}

HRESULT CHive::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _vector)))
    {
        return E_FAIL;
    }

    if (true == m_pGameInstance->Get_TurnOnShadow()) {

        CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

        if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
            return E_FAIL;

    }

    if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
    {
        return E_FAIL;
    }

    return S_OK;
}

CHive* CHive::Create(_dev pDevice, _context pContext)
{
	CHive* pInstance = new CHive(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHive");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHive::Clone(void* pArg)
{
	CHive* pInstance = new CHive(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHive");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHive::Free()
{
	__super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pDissolveTextureCom);
}
