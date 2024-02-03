#include "Trigger.h"

CTrigger::CTrigger(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
	: CGameObject(rhs)
{
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

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_vPos));

	return S_OK;
}

void CTrigger::Tick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransformCom->Get_World_Matrix());
}

void CTrigger::Late_Tick(_float fTimeDelta)
{

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pCollider);
#endif

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
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
	CollDesc.vCenter = _vec3(m_vPos.x, m_vPos.y, m_vPos.z);
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

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);
}
