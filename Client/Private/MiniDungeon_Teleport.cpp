#include "MiniDungeon_Teleport.h"
#include "Trigger_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
CMiniDungeon_Teleport::CMiniDungeon_Teleport(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CMiniDungeon_Teleport::CMiniDungeon_Teleport(const CMiniDungeon_Teleport& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniDungeon_Teleport::Init_Prototype()
{
	return S_OK;
}

HRESULT CMiniDungeon_Teleport::Init(void* pArg)
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

void CMiniDungeon_Teleport::Tick(_float fTimeDelta)
{
	m_pCollider->Update(m_pTransformCom->Get_World_Matrix());

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	if (m_pCollider->Intersect(pCollider))
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_HumanBoss"), TEXT("Prototype_GameObject_Human_Boss"))))
		{
			return;
		}
		m_isFinished = true;
		LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
		*Light = g_Light_HumanBoss;
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_ENDFULLSCREEN);
		CTrigger_Manager::Get_Instance()->Teleport(TS_BossRoom);
		CTrigger_Manager::Get_Instance()->Set_BossRoom();
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pCollider);
#endif
}

HRESULT CMiniDungeon_Teleport::Add_Components()
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



CMiniDungeon_Teleport* CMiniDungeon_Teleport::Create(_dev pDevice, _context pContext)
{
	CMiniDungeon_Teleport* pInstance = new CMiniDungeon_Teleport(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMiniDungeon_Teleport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMiniDungeon_Teleport::Clone(void* pArg)
{
	CMiniDungeon_Teleport* pInstance = new CMiniDungeon_Teleport(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMiniDungeon_Teleport");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMiniDungeon_Teleport::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);
}
