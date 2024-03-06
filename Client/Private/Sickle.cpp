#include "Sickle.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "Collision_Manager.h"

CSickle::CSickle(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CSickle::CSickle(const CSickle& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CSickle::Init_Prototype()
{
	return S_OK;
}

HRESULT CSickle::Init(void* pArg)
{


	_vec3 StartPos = *reinterpret_cast<_vec3*>(pArg);

	m_pTransformCom->Set_Position(StartPos);
	m_pTransformCom->LookAt(dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")))->Get_CenterPos());


	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BP_Skill_10017_SFX_01"));

	return S_OK;
}

void CSickle::Tick(_float fTimeDelta)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	vPos = XMVectorLerp(vPos, vPlayerPos,0.02f);
	m_pTransformCom->Set_State(State::Pos, vPos);
	 
	m_fLifeTimer += fTimeDelta;
	if (m_fLifeTimer > 8.f)
	{
		m_hasAttacked = true;
		_vec3 vPos = m_pTransformCom->Get_State(State::Pos);
		m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SickleTrap"), TEXT("Prototype_GameObject_SickleTrap"), &vPos);
		Kill();
		if (m_iSoundChannel != -1)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
		}
	}

	m_EffectMatrices = _mat::CreateTranslation(_vec3(vPos)+_vec3(0.f, 1.5f, 0.f));
	
	m_iHP = 0;
	for (size_t i = 0; i < m_iNumEffects; i++)
	{
		if (m_pEffects[i])
		{
			m_pEffects[i]->Tick(fTimeDelta);
			if (m_hasAttacked)
			{
				m_pEffects[i]->Kill();
			}
			if (m_pEffects[i]->isDead())
			{
				Safe_Release(m_pEffects[i]);
			}
			m_iHP++;
		}
	}

	if (m_iHP == 0)
	{
		Kill();
		if (m_iSoundChannel != -1)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
		}
	}

	if (not m_hasAttacked and m_pGameInstance->Attack_Player(m_pColliderCom, 150 + rand() % 50))
	{
		m_hasAttacked = true;
		_vec3 vPos = m_pTransformCom->Get_State(State::Pos);
		CTransform* BossTransform = GET_TRANSFORM("Layer_HumanBoss",LEVEL_TOWER);
		vPos.y = BossTransform->Get_State(State::Pos).y;
	
		m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SickleTrap"), TEXT("Prototype_GameObject_SickleTrap"), &vPos);
		m_pGameInstance->Play_Sound(TEXT("BP_Skill_10014_SFX_01"));
	}

	if (m_iSoundChannel != -1)
	{
		if (m_pGameInstance->Get_ChannelCurPosRatio(m_iSoundChannel) >= 0.6f)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
			m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BP_Skill_10017_SFX_01"));
		}
	}
}

void CSickle::Late_Tick(_float fTimeDelta)
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

HRESULT CSickle::Add_Components()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 2.7f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &CollDesc)))
	{
		return E_FAIL;
	}

	CEffect_Manager* pEffect_Manager = CEffect_Manager::Get_Instance();
	Safe_AddRef(pEffect_Manager);
	EffectInfo Info{};

	Info = pEffect_Manager->Get_EffectInformation(L"Boss_Att7_Base");
	Info.pMatrix = &m_EffectMatrices;
	Info.isFollow = true;
	m_pEffects[0] = pEffect_Manager->Clone_Effect(Info);

	Info = pEffect_Manager->Get_EffectInformation(L"Boss_Att7_outter");
	Info.pMatrix = &m_EffectMatrices;
	Info.isFollow = true;
	m_pEffects[1] = pEffect_Manager->Clone_Effect(Info);

	Info = pEffect_Manager->Get_EffectInformation(L"Boss_Att7_elec");
	Info.pMatrix = &m_EffectMatrices;
	Info.isFollow = true;
	m_pEffects[2] = pEffect_Manager->Clone_Effect(Info);

	Info = pEffect_Manager->Get_EffectInformation(L"Boss_Att7_Liqui");
	Info.pMatrix = &m_EffectMatrices;
	Info.isFollow = true;
	m_pEffects[3] = pEffect_Manager->Clone_Effect(Info);

	Safe_Release(pEffect_Manager);

#ifdef _DEBUG
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

CSickle* CSickle::Create(_dev pDevice, _context pContext)
{
	CSickle* pInstance = new CSickle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSickle::Clone(void* pArg)
{
	CSickle* pInstance = new CSickle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSickle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSickle::Free()
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
