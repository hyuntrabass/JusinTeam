#include "Thief04.h"

CThief04::CThief04(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CThief04::CThief04(const CThief04& rhs)
	: CMonster(rhs)
{
}

HRESULT CThief04::Init_Prototype()
{
    return S_OK;
}

HRESULT CThief04::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Thief04");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(10.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Speed(3.f);

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_eCurState = STATE_IDLE;

	m_iHP = 10;

    return S_OK;
}

void CThief04::Tick(_float fTimeDelta)
{
	Change_State(fTimeDelta);
	Control_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
}

void CThief04::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pAxeColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pKnifeColliderCom);
#endif
}

HRESULT CThief04::Render()
{
	__super::Render();

    return S_OK;
}

void CThief04::Change_State(_float fTimeDelta)
{
}

void CThief04::Control_State(_float fTimeDelta)
{
}

HRESULT CThief04::Add_Collider()
{
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 0.1f;
	CollDesc.vCenter = _vec3(0.35f, 0.f, 0.15f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_AxeCollider_Sphere"), (CComponent**)&m_pAxeColliderCom, &CollDesc)))
		return E_FAIL;

	CollDesc.fRadius = 0.05f;
	CollDesc.vCenter = _vec3(0.3f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_KnifeCollider_Sphere"), (CComponent**)&m_pKnifeColliderCom, &CollDesc)))
		return E_FAIL;

    return S_OK;
}

void CThief04::Update_Collider()
{
	_mat AxeMatrix = *(m_pModelCom->Get_BoneMatrix("bone_R_weapon_01"));
	AxeMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pAxeColliderCom->Update(AxeMatrix);

	_mat KnifeMatrix = *(m_pModelCom->Get_BoneMatrix("bone_L_weapon_01"));
	KnifeMatrix *= m_pTransformCom->Get_World_Matrix();

	m_pKnifeColliderCom->Update(KnifeMatrix);
}

CThief04* CThief04::Create(_dev pDevice, _context pContext)
{
	CThief04* pInstance = new CThief04(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CThief04");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CThief04::Clone(void* pArg)
{
	CThief04* pInstance = new CThief04(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CThief04");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThief04::Free()
{
	__super::Free();

	Safe_Release(m_pAxeColliderCom);
	Safe_Release(m_pKnifeColliderCom);
}
