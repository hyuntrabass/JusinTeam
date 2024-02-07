#include "Trigger_Manager.h"
#include "Trigger.h"
#include "Camera_CutScene.h"

IMPLEMENT_SINGLETON(CTrigger_Manager)

CTrigger_Manager::CTrigger_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}
HRESULT CTrigger_Manager::Init()
{
	return S_OK;
}

void CTrigger_Manager::Tick(_float fTimeDelta)
{
	for (auto& iter : m_pTrigger)
	{
		//m_pGameInstance->Get_CurrentLevelIndex();
		m_isColl = iter->Get_Collision();
		//m_isPlayCutScene = false;
		if (m_isColl == true)
		{
			if (iter->Get_TriggerType() == VILLAGE_TRIGGER && iter->Get_Limited() == true)
			{
				m_isPlayCutScene = true;
				m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);
				m_strFilePath = L"../Bin/Data/Village_CutScene.dat";
				iter->Set_Limited(false);
			}
			else if (iter->Get_TriggerType() == FRONTDOOR_IN_TRIGGER && iter->Get_Limited() == false)
			{

			}
			else if (iter->Get_TriggerType() == FRONTDOOR_OUT_TRIGGER && iter->Get_Limited() == false)
			{

			}
			else if (iter->Get_TriggerType() == BACKDOOR_IN_TRIGGER && iter->Get_Limited() == false)
			{

			}
			else if (iter->Get_TriggerType() == BACKDOOR_OUT_TRIGGER && iter->Get_Limited() == false)
			{

			}
			else if (iter->Get_TriggerType() == BOSS_TRIGGER && iter->Get_Limited() == true)
			{
				m_isCollBossTrigger = true;
			}
		}
	}
}

void CTrigger_Manager::Limited_CutScene(_bool isLimited)
{
	m_isLimited = isLimited;
}

void CTrigger_Manager::Set_Trigger(CTrigger* pTrigger)
{
	m_pTrigger.push_back(pTrigger);
}

void CTrigger_Manager::Free()
{
	for (auto& iter : m_pTrigger)
	{
		Safe_Release(iter);
	}
	m_pTrigger.clear();

	Safe_Release(m_pGameInstance);
}
