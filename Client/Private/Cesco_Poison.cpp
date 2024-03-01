#include "Cesco_Poison.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"

CCesco_Poison::CCesco_Poison(_dev pDevice, _context pContext)
    :CGameObject(pDevice, pContext)
{
}

CCesco_Poison::CCesco_Poison(const CCesco_Poison& rhs)
    :CGameObject(rhs)
{
}

HRESULT CCesco_Poison::Init_Prototype()
{
    return S_OK;
}

HRESULT CCesco_Poison::Init(void* pArg)
{
    if (pArg)
    {
        _mat WorldMat= *reinterpret_cast<_mat*>(pArg);
        m_pTransformCom->Set_Matrix(WorldMat);
        _vec4 vLook = _vec4(0.01f, -1.f, 0.f, 0.f);
          m_pTransformCom->LookAt_Dir(vLook);
    }


    m_pTransformCom->Set_Speed(15.f);
    if (FAILED(Add_Components()))
        return E_FAIL;

    return S_OK;
}

void CCesco_Poison::Tick(_float fTimeDelta)
{
    m_fLifeTime += fTimeDelta;

    if (m_fLifeTime >= 3.f)
    {
        Kill();
    }

    m_pTransformCom->Go_Straight(fTimeDelta);
    m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
 

    if (m_pEffect)
    {
        m_pEffect->Tick(fTimeDelta);
        m_EffectMatrices = _mat::CreateRotationX(XMConvertToRadians(-90.f)) * m_pTransformCom->Get_World_Matrix();
  
    }
}

void CCesco_Poison::Late_Tick(_float fTimeDelta)
{
    if (m_pEffect)
    {
        m_pEffect->Late_Tick(fTimeDelta);
    }
#ifdef _DEBUG
    m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif // DEBUG
}



HRESULT CCesco_Poison::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
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

    CEffect_Manager* pEffect_Manager = CEffect_Manager::Get_Instance();
    Safe_AddRef(pEffect_Manager);
    EffectInfo Info{};

    Info = pEffect_Manager->Get_EffectInformation(L"Cesco_Poison");
    Info.pMatrix = &m_EffectMatrices;
    Info.isFollow = true;
    m_pEffect = pEffect_Manager->Clone_Effect(Info);

    Safe_Release(pEffect_Manager);
    return S_OK;
}



CCesco_Poison* CCesco_Poison::Create(_dev pDevice, _context pContext)
{
	CCesco_Poison* pInstance = new CCesco_Poison(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCesco_Poison");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCesco_Poison::Clone(void* pArg)
{
	CCesco_Poison* pInstance = new CCesco_Poison(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCesco_Poison");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCesco_Poison::Free()
{
	__super::Free();

    Safe_Release(m_pEffect);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pBodyColliderCom);
    Safe_Release(m_pDissolveTextureCom);
}
