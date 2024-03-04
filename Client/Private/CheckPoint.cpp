#include "CheckPoint.h"
#include "UI_Manager.h"

CCheckPoint::CCheckPoint(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CCheckPoint::CCheckPoint(const CCheckPoint& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCheckPoint::Init_Prototype()
{
	return S_OK;
}

HRESULT CCheckPoint::Init(void* pArg)
{

	TriggerInfo m_Info = *(TriggerInfo*)pArg;

	m_Matrix =  m_Info.WorldMat;
	m_iColliderSize = m_Info.fSize;
	m_iTriggerNumber = m_Info.iIndex;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	m_pTransformCom->Set_Matrix(m_Matrix);

	return S_OK;
}

void CCheckPoint::Tick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransformCom->Get_World_Matrix());

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	m_isCollision = m_pCollider->Intersect(pCollider);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pCollider);
#endif
}

HRESULT CCheckPoint::Add_Components()
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
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Trigger_Sphere"), (CComponent**)&m_pCollider, &CollDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}


CCheckPoint* CCheckPoint::Create(_dev pDevice, _context pContext)
{
	CCheckPoint* pInstance = new CCheckPoint(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCheckPoint");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCheckPoint::Clone(void* pArg)
{
	CCheckPoint* pInstance = new CCheckPoint(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCheckPoint");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCheckPoint::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);
}
