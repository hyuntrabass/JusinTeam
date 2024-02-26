#include "SickleTrap.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "Collision_Manager.h"

CSickleTrap::CSickleTrap(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CSickleTrap::CSickleTrap(const CSickleTrap& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CSickleTrap::Init_Prototype()
{
	return S_OK;
}

HRESULT CSickleTrap::Init(void* pArg)
{


	_vec3 StartPos = *reinterpret_cast<_vec3*>(pArg);

	m_pTransformCom->Set_Position(StartPos);

	m_pTransformCom->LookAt(dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_CenterPos());


	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	//m_pGameInstance->Play_Sound(TEXT("SE_5130_Meteor_SFX_01"));

	return S_OK;
}

void CSickleTrap::Tick(_float fTimeDelta)
{
	m_fAttDelay += fTimeDelta;
	m_fLifeTimer += fTimeDelta;
	if (m_fLifeTimer > 10.f)
	{
		Kill();
	}

	m_EffectMatrices[0] = _mat::CreateScale(4.f)/* * _mat::CreateRotationY(XMConvertToRadians(m_fLifeTimer *-2000.f))*/ * m_pTransformCom->Get_World_Matrix();
	m_EffectMatrices[1] = _mat::CreateScale(4.f) * m_pTransformCom->Get_World_Matrix();
	m_EffectMatrices[2] = _mat::CreateScale(4.f)* m_pTransformCom->Get_World_Matrix();
	
	for (size_t i = 0; i < 3; i++)
	{
		if (m_pEffects[i])
		{
			m_pEffects[i]->Tick(fTimeDelta);
		}
	}

	if (m_fAttDelay > 1.f)
	{
		if (m_pGameInstance->Attack_Player(m_pColliderCom, 10 + rand() % 30))
		{
			
			m_fAttDelay = 0.f;
		}

	}

	CCollider* pPlayerHitCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	if (m_pColliderCom->Intersect(pPlayerHitCollider))
	{
		CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
		_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
		_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
		vPos.y = vPlayerPos.y;
		_vec4 vDir = vPos - vPlayerPos;
		vDir.Normalize();
		vDir.w = 0.f;
		vPlayerPos += vDir;
		pPlayerTransform->Set_State(State::Pos, vPlayerPos);
	}


}

void CSickleTrap::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < 3; i++)
	{
		if (m_pEffects[i])
		{
			m_pEffects[i]->Late_Tick(fTimeDelta);
		}
	}

	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif // _DEBUG

}

HRESULT CSickleTrap::Add_Components()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 2.7f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_SafeZone_Collider"), (CComponent**)&m_pColliderCom, &CollDesc)))
	{
		return E_FAIL;
	}

	CEffect_Manager* pEffect_Manager = CEffect_Manager::Get_Instance();
	Safe_AddRef(pEffect_Manager);
	EffectInfo Info{};

	Info = pEffect_Manager->Get_EffectInformation(L"HumanBoss_Sickle");
	Info.pMatrix = &m_EffectMatrices[0];
	Info.isFollow = true;
	m_pEffects[0] = pEffect_Manager->Clone_Effect(Info);

	Info = pEffect_Manager->Get_EffectInformation(L"HumanBoss_Ring");
	Info.pMatrix = &m_EffectMatrices[1];
	Info.isFollow = true;
	m_pEffects[1] = pEffect_Manager->Clone_Effect(Info);

	Info = pEffect_Manager->Get_EffectInformation(L"HumanBoss_Ring2");
	Info.pMatrix = &m_EffectMatrices[2];
	Info.isFollow = true;
	m_pEffects[2] = pEffect_Manager->Clone_Effect(Info);


	Safe_Release(pEffect_Manager);

#ifdef _DEBUG
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

CSickleTrap* CSickleTrap::Create(_dev pDevice, _context pContext)
{
	CSickleTrap* pInstance = new CSickleTrap(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSickleTrap::Clone(void* pArg)
{
	CSickleTrap* pInstance = new CSickleTrap(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSickleTrap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSickleTrap::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	for (size_t i = 0; i < 3; i++)
	{
		Safe_Release(m_pEffects[i]);
	}

#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
#endif // _DEBUG

}
