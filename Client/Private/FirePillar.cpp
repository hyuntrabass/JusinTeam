#include "FirePillar.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CFirePillar::CFirePillar(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CFirePillar::CFirePillar(const CFirePillar& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFirePillar::Init_Prototype()
{
	return S_OK;
}

HRESULT CFirePillar::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_mat EffectMatrix = _mat::CreateScale(5.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateTranslation(_vec3(vPlayerPos) + _vec3(0.f, 0.1f, 0.f));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
	Info.pMatrix = &EffectMatrix;
	m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
	Info.pMatrix = &EffectMatrix;
	m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	m_pTransformCom->Set_Position(vPlayerPos + _vec3(0.f, 0.1f, 0.f));

	return S_OK;
}

void CFirePillar::Tick(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;

	if (m_fLifeTime >= 7.f)
	{
		Kill();
		return;
	}

	if (m_fLifeTime >= 1.8f)
	{
		if (m_pBaseEffect && m_pFrameEffect)
		{
			Safe_Release(m_pFrameEffect);
			Safe_Release(m_pBaseEffect);

			// �ұ�� ����
			_mat EffectMatrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Explosion");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Explosion_Effect");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_FireEffect");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Floor_Explosion");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Loop");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Shine");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			EffectMatrix = _mat::CreateScale(1.8f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Tornado_Loop");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			EffectMatrix = _mat::CreateScale(1.5f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_MiniFirePillar_Loop");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			EffectMatrix = _mat::CreateScale(1.5f) * _mat::CreateRotationY(XMConvertToRadians(30.f)) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_MiniFirePillar_Loop");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			EffectMatrix = _mat::CreateScale(1.5f) * _mat::CreateRotationY(XMConvertToRadians(90.f)) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_MiniFirePillar_Loop");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			EffectMatrix = _mat::CreateScale(2.5f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)/* + _vec3(0.f, -0.1f, 0.f)*/));
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Dragon_FirePillar_Floor_Loop");
			Info.pMatrix = &EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		}
	}

	if (m_fLifeTime >= 6.f)
	{
		if (m_iSoundChannel != -1)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
		}
	}
	else if (m_fLifeTime >= 1.2f)
	{
		if (m_iSoundChannel == -1)
		{
			m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BP_Skill_10061_SFX_01"));
		}
		else if (m_pGameInstance->Get_ChannelCurPosRatio(m_iSoundChannel) >= 0.6f)
		{
			m_pGameInstance->FadeoutSound(m_iSoundChannel, fTimeDelta, 1.f, false);
			m_iSoundChannel = m_pGameInstance->Play_Sound(TEXT("BP_Skill_10061_SFX_01"), 0.5f, 0.25f);
			m_pGameInstance->FadeinSound(m_iSoundChannel, fTimeDelta);
		}
	}

	if (m_iSoundChannel != -1)
	{
		if (not m_pGameInstance->Get_IsPlayingSound(m_iSoundChannel))
		{
			m_iSoundChannel = -1;
		}
	}

	if (m_pFrameEffect && m_pBaseEffect)
	{
		m_pFrameEffect->Tick(fTimeDelta);
		m_pBaseEffect->Tick(fTimeDelta);
	}
}

void CFirePillar::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect && m_pBaseEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}

}

HRESULT CFirePillar::Render()
{
	return S_OK;
}

HRESULT CFirePillar::Add_Components()
{
	return S_OK;
}

HRESULT CFirePillar::Bind_ShaderResources()
{
	return S_OK;
}

CFirePillar* CFirePillar::Create(_dev pDevice, _context pContext)
{
	CFirePillar* pInstance = new CFirePillar(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CFirePillar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFirePillar::Clone(void* pArg)
{
	CFirePillar* pInstance = new CFirePillar(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CFirePillar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFirePillar::Free()
{
	__super::Free();

	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBaseEffect);
}
