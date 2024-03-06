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



	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BP_Skill_10081_SFX_01"));

	m_iHP = 1;

	return S_OK;
}

void CSickleTrap::Tick(_float fTimeDelta)
{
	m_fAttDelay += fTimeDelta;
	m_fLifeTimer += fTimeDelta;
	if (m_fLifeTimer > 10.f)
	{
		m_iHP = 0;
	}

	m_EffectMatrices = m_pTransformCom->Get_World_Matrix();
	
	for (size_t i = 0; i < m_iNumEffects; i++)
	{
		if (m_pEffects[i])
		{
			m_pEffects[i]->Tick(fTimeDelta);
			if (!m_iHP)
			{
				m_pEffects[0]->Kill();
				if (m_iSoundChannel != -1)
				{
					m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
				}
			}
			if (m_pEffects[i]->isDead())
			{
				Safe_Release(m_pEffects[i]);
			}
		}
	}

	if (not m_pEffects[0])
	{
		Kill();
	}

	if (m_fAttDelay > 1.f)
	{
		if (m_pGameInstance->Attack_Player(m_pColliderCom, 10 + rand() % 30))
		{
			m_fAttDelay = 0.f;
		}
	}

	if (m_iSoundChannel != -1)
	{
		if (m_pGameInstance->Get_ChannelCurPosRatio(m_iSoundChannel) >= 0.6f)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
			m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BP_Skill_10081_SFX_01"), 0.5f, false, 0.15f);
			m_pGameInstance->FadeinSound(m_iSoundChannel, fTimeDelta, 0.5f);
		}
	}
}

void CSickleTrap::Late_Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumEffects; i++)
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
	CollDesc.fRadius = 3.5f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &CollDesc)))
	{
		return E_FAIL;
	}

	CEffect_Manager* pEffect_Manager = CEffect_Manager::Get_Instance();
	Safe_AddRef(pEffect_Manager);
	EffectInfo Info{};

	Info = pEffect_Manager->Get_EffectInformation(L"Boss_Att7_Trap");
	Info.pMatrix = &m_EffectMatrices;
	Info.isFollow = true;
	m_pEffects[0] = pEffect_Manager->Clone_Effect(Info);

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
	for (size_t i = 0; i < m_iNumEffects; i++)
	{
		Safe_Release(m_pEffects[i]);
	}

#ifdef _DEBUG
	Safe_Release(m_pRendererCom);
#endif // _DEBUG

}
