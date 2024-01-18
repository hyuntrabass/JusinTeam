#include "TestVTFModel.h"

CTestVTFModel::CTestVTFModel(_dev pDevice, _context pContext)
    : CGameObject(pDevice, pContext)
{
}

CTestVTFModel::CTestVTFModel(const CTestVTFModel& rhs)
    : CGameObject(rhs)
{
}

HRESULT CTestVTFModel::Init_Prototype()
{
    return S_OK;
}

HRESULT CTestVTFModel::Init(void* pArg)
{
    Add_Components();

    m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 0.f, 0.f, 1.f));

    m_iHP = 10;

    return S_OK;
}

void CTestVTFModel::Tick(_float fTimeDelta)
{
    m_OldWorldMatrix = m_pTransformCom->Get_World_Matrix();

    if (m_pGameInstance->Key_Pressing(DIK_K)) {
        _vec4 vPos = m_pTransformCom->Get_State(State::Pos);
        vPos += _vec4(1.f, 0.f, 0.f, 0.f) * fTimeDelta;
        m_pTransformCom->Set_State(State::Pos, vPos);
    }

    if (m_pGameInstance->Key_Pressing(DIK_L)) {
        m_pModelCom->Set_NextAnimationIndex(++m_iAnimIndex);
    }
}

void CTestVTFModel::Late_Tick(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta * 2.f);
    m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

}

HRESULT CTestVTFModel::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
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

        _bool HasSpecTex{};
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_SpecTexture", i, TextureType::Shininess)))
        {
            HasSpecTex = false;
        }
        else
        {
            HasSpecTex = true;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasSpecTex", &HasSpecTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(1)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CTestVTFModel::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VTF"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_VTFRabbit"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CTestVTFModel::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_Matrix("g_OldWorldMatrix", m_OldWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix_vec4x4())))
        return E_FAIL;

    if (FAILED(m_pModelCom->Bind_OldAnimation(m_pShaderCom)))
        return E_FAIL;

    if (FAILED(m_pModelCom->Bind_Animation(m_pShaderCom)))
        return E_FAIL;

    return S_OK;
}

CTestVTFModel* CTestVTFModel::Create(_dev pDevice, _context pContext)
{
    CTestVTFModel* pInstance = new CTestVTFModel(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CTestVTFModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestVTFModel::Clone(void* pArg)
{
    CTestVTFModel* pInstance = new CTestVTFModel(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CTestVTFModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestVTFModel::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
}
