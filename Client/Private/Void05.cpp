#include "Void05.h"

CVoid05::CVoid05(_dev pDevice, _context pContext)
    : CMonster(pDevice, pContext)
{
}

CVoid05::CVoid05(const CVoid05& rhs)
    : CMonster(rhs)
{
}

HRESULT CVoid05::Init_Prototype()
{
    return S_OK;
}

HRESULT CVoid05::Init(void* pArg)
{
    m_strModelTag = TEXT("Prototype_Model_Void05");

    if (FAILED(__super::Add_Components()))
    {
        return E_FAIL;
    }

    if (FAILED(Add_Collider()))
    {
        return E_FAIL;
    }

    m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 0.f, 0.f, 1.f));

    m_Animation.iAnimIndex = 0;
    m_Animation.isLoop = true;
    m_Animation.bSkipInterpolation = true;

    return S_OK;
}

void CVoid05::Tick(_float fTimeDelta)
{
    m_pModelCom->Set_Animation(m_Animation);

    Update_Collider();
}

void CVoid05::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    m_pRendererCom->Add_DebugComponent(m_pRHColliderCom);
    m_pRendererCom->Add_DebugComponent(m_pLHColliderCom);
}

HRESULT CVoid05::Render()
{
    __super::Render();

    return S_OK;
}

void CVoid05::Change_State(_float fTimeDelta)
{
}

void CVoid05::Control_State(_float fTimeDelta)
{
}

HRESULT CVoid05::Add_Collider()
{
    // Com_Collider
    Collider_Desc RCollDesc = {};
    RCollDesc.eType = ColliderType::Sphere;
    RCollDesc.fRadius = 0.2f;
    RCollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
        TEXT("Com_RCollider_Sphere"), (CComponent**)&m_pRHColliderCom, &RCollDesc)))
        return E_FAIL;

    // Com_Collider
    Collider_Desc LCollDesc = {};
    LCollDesc.eType = ColliderType::Sphere;
    LCollDesc.fRadius = 0.2f;
    LCollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
        TEXT("Com_LCollider_Sphere"), (CComponent**)&m_pLHColliderCom, &LCollDesc)))
        return E_FAIL;

    return S_OK;
}

void CVoid05::Update_Collider()
{
    _mat RightHandMatrix = *(m_pModelCom->Get_BoneMatrix("Bip001-R-Finger22"));
    RightHandMatrix *= m_pTransformCom->Get_World_Matrix();

    m_pRHColliderCom->Update(RightHandMatrix);

    _mat LeftHandMatrix = *(m_pModelCom->Get_BoneMatrix("Bip001-L-Finger22"));
    LeftHandMatrix *= m_pTransformCom->Get_World_Matrix();

    m_pLHColliderCom->Update(LeftHandMatrix);

}

CVoid05* CVoid05::Create(_dev pDevice, _context pContext)
{
    CVoid05* pInstance = new CVoid05(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CVoid05");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CVoid05::Clone(void* pArg)
{
    CVoid05* pInstance = new CVoid05(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CVoid05");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVoid05::Free()
{
    __super::Free();

    Safe_Release(m_pRHColliderCom);
    Safe_Release(m_pLHColliderCom);
}
