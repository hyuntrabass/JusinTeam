#include "Log.h"

CLog::CLog(_dev pDevice, _context pContext)
    :CGameObject(pDevice, pContext)
{
}

CLog::CLog(const CLog& rhs)
    :CGameObject(rhs)
{
}

HRESULT CLog::Init_Prototype()
{
    return S_OK;
}

HRESULT CLog::Init(void* pArg)
{
    if (pArg)
    {
        LOG_DESC* pLogDesc = reinterpret_cast<LOG_DESC*>(pArg);

        m_pTransformCom->Set_Matrix(pLogDesc->WorldMatrix);
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CLog::Tick(_float fTimeDelta)
{
    m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CLog::Late_Tick(_float fTimeDelta)
{
    m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
#ifdef _DEBUG
    m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif // DEBUG
}

HRESULT CLog::Render()
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

void CLog::Init_State(_float fTimeDelta)
{
}

void CLog::Tick_State(_float fTimeDelta)
{
}

HRESULT CLog::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_VILLAGE, TEXT("Prototype_Model_Log"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
    {
        return E_FAIL;
    }

	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::AABB;
	ColliderDesc.vExtents = _vec3(1.f, 1.2f, 1.f);
	ColliderDesc.vCenter = _vec3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pBodyColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

    return S_OK;
}

HRESULT CLog::Bind_ShaderResources()
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

CLog* CLog::Create(_dev pDevice, _context pContext)
{
	CLog* pInstance = new CLog(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLog::Clone(void* pArg)
{
	CLog* pInstance = new CLog(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLog::Free()
{
	__super::Free();
}
