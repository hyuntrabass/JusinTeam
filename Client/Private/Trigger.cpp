#include "Trigger.h"

CTrigger::CTrigger(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
	, m_pTrigger_Manager(CTrigger_Manager::Get_Instance())
{
	Safe_AddRef(m_pTrigger_Manager);
}

CTrigger::CTrigger(const CTrigger& rhs)
	: CGameObject(rhs)
	, m_pTrigger_Manager(rhs.m_pTrigger_Manager)
	, m_isLimited(rhs.m_isLimited)
	, m_eTriggerType(rhs.m_eTriggerType)
{
	Safe_AddRef(m_pTrigger_Manager);
}

HRESULT CTrigger::Init_Prototype()
{
	return S_OK;
}

HRESULT CTrigger::Init(void* pArg)
{

	TriggerInfo m_Info = *(TriggerInfo*)pArg;

	m_vPos = _vec4(m_Info.WorldMat._41, m_Info.WorldMat._42, m_Info.WorldMat._43, m_Info.WorldMat._44);
	m_iColliderSize = m_Info.fSize;
	m_iTriggerNumber = m_Info.iIndex;
	m_eTriggerType = (TriggerType)m_iTriggerNumber;
	m_isLimited = m_Info.bLimited;
	TriggerType test = m_eTriggerType;
	_bool tests = m_isLimited;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	m_pTransformCom->Set_Matrix(m_Info.WorldMat);
	//m_pTransformCom->Set_Scale(_vec3(0.01f, 0.01f, 0.01f));
	//m_pTransformCom->Set_State(State::Pos, m_vPos);
	

	m_pTrigger_Manager->Set_Trigger(this);
	m_pTrigger_Manager->Limited_CutScene(m_isLimited);
	return S_OK;
}

void CTrigger::Tick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransformCom->Get_World_Matrix());

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	m_isCollision = m_pCollider->Intersect(pCollider);
	
}

void CTrigger::Late_Tick(_float fTimeDelta)
{

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pCollider);
#endif

	//m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

}

HRESULT CTrigger::Render()
{

	return S_OK;
}

HRESULT CTrigger::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	/* For.Com_Collider_SPHERE */
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = m_iColliderSize;


	CollDesc.vCenter = _vec3(0.f,0.f,0.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Trigger_Sphere"), (CComponent**)&m_pCollider, &CollDesc)))
		return E_FAIL;

	return S_OK;
}


CTrigger* CTrigger::Create(_dev pDevice, _context pContext)
{
	CTrigger* pInstance = new CTrigger(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTrigger::Clone(void* pArg)
{
	CTrigger* pInstance = new CTrigger(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTrigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger::Free()
{
	__super::Free();

	if (m_pTrigger_Manager)
		Safe_Release(m_pTrigger_Manager);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);
}
