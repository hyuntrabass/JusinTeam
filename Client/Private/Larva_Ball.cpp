#include "Larva_Ball.h"
#include "Effect_Dummy.h"

CLarva_Ball::CLarva_Ball(_dev pDevice, _context pContext)
	:CGameObject(pDevice, pContext)
{
}

CLarva_Ball::CLarva_Ball(const CLarva_Ball& rhs)
	:CGameObject(rhs)
{
}

HRESULT CLarva_Ball::Init_Prototype()
{
	return S_OK;
}

HRESULT CLarva_Ball::Init(void* pArg)
{
	if (pArg)
	{
		LARVABALL_DESC* pLarvaBallDesc = reinterpret_cast<LARVABALL_DESC*>(pArg);

		m_pTransformCom->Set_Position(pLarvaBallDesc->vPosition);
		m_pTransformCom->LookAt_Dir(pLarvaBallDesc->vDirection);
	}

	m_EffectMatrix = m_pTransformCom->Get_World_Matrix();

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CLarva_Ball::Tick(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;
	m_fGasSpawnTime += fTimeDelta;

	m_pTransformCom->Go_Straight(fTimeDelta);
	m_EffectMatrix = m_pTransformCom->Get_World_Matrix();
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pBallEffect->Tick(fTimeDelta);

	if (m_pGameInstance->Attack_Player(m_pColliderCom, 10, MonAtt_Poison) || m_fLifeTime >= 7.f)
	{
		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(TEXT("Groar_Ball_Smoke"));
		Info.pMatrix = &m_EffectMatrix;
		Info.fLifeTime = 0.3f;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		Kill();
	}
}

void CLarva_Ball::Late_Tick(_float fTimeDelta)
{
	if (m_pBallEffect)
	{
		m_pBallEffect->Late_Tick(fTimeDelta);
	}
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // DEBUG
}

HRESULT CLarva_Ball::Add_Components()
{
#ifdef _DEBUG

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

#endif // _DEBUG

	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::Sphere;
	ColliderDesc.fRadius = 0.2f;
	ColliderDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(TEXT("Groar_Ball"));
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	m_pBallEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	return S_OK;
}

CLarva_Ball* CLarva_Ball::Create(_dev pDevice, _context pContext)
{
	CLarva_Ball* pInstance = new CLarva_Ball(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLarva_Ball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLarva_Ball::Clone(void* pArg)
{
	CLarva_Ball* pInstance = new CLarva_Ball(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLarva_Ball");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLarva_Ball::Free()
{
	__super::Free();

	Safe_Release(m_pBallEffect);
	Safe_Release(m_pColliderCom);
#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
#endif // _DEBUG
}
