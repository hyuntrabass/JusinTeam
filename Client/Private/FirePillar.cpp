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

	if (m_fLifeTime >= 1.5f)
	{
		if (m_pBaseEffect && m_pFrameEffect)
		{
			Safe_Release(m_pFrameEffect);
			Safe_Release(m_pBaseEffect);
		}

		// ºÒ±âµÕ »ý¼º

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
}
