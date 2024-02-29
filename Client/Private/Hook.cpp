#include "Hook.h"

CHook::CHook(_dev pDevice, _context pContext)
    :CGameObject(pDevice, pContext)
{
}

CHook::CHook(const CHook& rhs)
    :CGameObject(rhs)
{
}

HRESULT CHook::Init_Prototype()
{
    return S_OK;
}

HRESULT CHook::Init(void* pArg)
{
    if (pArg)
    {
        HOOK_DESC* pLogDesc = reinterpret_cast<HOOK_DESC*>(pArg);

        m_pTransformCom->Set_Matrix(pLogDesc->WorldMatrix);
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CHook::Tick(_float fTimeDelta)
{
    m_fLifeTime += fTimeDelta;

    if (m_fLifeTime >= 10.f)
    {
        Kill();
    }

    m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CHook::Late_Tick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
#ifdef _DEBUG
    m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif // DEBUG
}

HRESULT CHook::Render()
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

        _bool HasMaskTex{};
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
        {
            HasMaskTex = false;
        }
        else
        {
            HasMaskTex = true;
        }

        _bool HasGlowTex{};
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_GlowTexture", i, TextureType::Specular)))
        {
            HasGlowTex = false;
        }
        else
        {
            HasGlowTex = true;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasGlowTex", &HasGlowTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(StaticPass_Default)))
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

HRESULT CHook::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_VILLAGE, TEXT("Prototype_Model_Hook"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
    {
        return E_FAIL;
    }

	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::Sphere;
    ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Hook_Collider"), reinterpret_cast<CComponent**>(&m_pBodyColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

    return S_OK;
}

HRESULT CHook::Bind_ShaderResources()
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

    return S_OK;
}

CHook* CHook::Create(_dev pDevice, _context pContext)
{
	CHook* pInstance = new CHook(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHook::Clone(void* pArg)
{
	CHook* pInstance = new CHook(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHook::Free()
{
	__super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pBodyColliderCom);
    Safe_Release(m_pDissolveTextureCom);
}
