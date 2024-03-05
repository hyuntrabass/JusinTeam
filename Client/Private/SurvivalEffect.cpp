#include "SurvivalEffect.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CSurvivalEffect::CSurvivalEffect(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSurvivalEffect::CSurvivalEffect(const CSurvivalEffect& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSurvivalEffect::Init_Prototype()
{
    return S_OK;
}

HRESULT CSurvivalEffect::Init(void* pArg)
{
	_vec3 vPos = {};

	vPos = *(_vec3*)pArg;

	m_pTransformCom->Set_Position(vPos);

	CTransform* pCannonTransform = GET_TRANSFORM("Layer_Cannon", LEVEL_VILLAGE);
	_vec4 vCannonPos = pCannonTransform->Get_State(State::Pos);

	m_pTransformCom->LookAt(vCannonPos);

	m_pTransformCom->Set_Speed(5.f);

	m_UpdateMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) /*+ _vec3(0.f, 0.3f, 0.f)*/);
	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Cannon_Parti");
	Info.pMatrix = &m_UpdateMatrix;
	Info.isFollow = true;
	m_pEffect[0] = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

    return S_OK;
}

void CSurvivalEffect::Tick(_float fTimeDelta)
{
	m_pTransformCom->Go_Straight(fTimeDelta);

	m_UpdateMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) /*+ _vec3(0.f, 0.3f, 0.f)*/);

	if (m_pEffect[0])
	{
		m_pEffect[0]->Tick(fTimeDelta);
	}
}

void CSurvivalEffect::Late_Tick(_float fTimeDelta)
{
	if (m_pEffect[0])
	{
		m_pEffect[0]->Late_Tick(fTimeDelta);
	}
}

HRESULT CSurvivalEffect::Render()
{
    return S_OK;
}

CSurvivalEffect* CSurvivalEffect::Create(_dev pDevice, _context pContext)
{
	CSurvivalEffect* pInstance = new CSurvivalEffect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSurvivalEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSurvivalEffect::Clone(void* pArg)
{
	CSurvivalEffect* pInstance = new CSurvivalEffect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSurvivalEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSurvivalEffect::Free()
{
	__super::Free();

	Safe_Release(m_pEffect[0]);
	Safe_Release(m_pEffect[1]);
}
