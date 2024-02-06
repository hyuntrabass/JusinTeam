#include "Trigger_Manager.h"
#include "Trigger.h"

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
	_uint iCurrLevel = m_pGameInstance->Get_CurrentLevelIndex();

	for (auto iter = m_pTrigger[iCurrLevel].begin(); iter != m_pTrigger[iCurrLevel].end();)
	{
		iter->second->Tick(fTimeDelta);

		if (iter->second->isDead())
		{
			Safe_Release(iter->second);
			iter = m_pTrigger[iCurrLevel].erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CTrigger_Manager::Late_Tick(_float fTimeDelta)
{
	_uint iCurrLevel = m_pGameInstance->Get_CurrentLevelIndex();

	for (auto& pTrigger : m_pTrigger[iCurrLevel])
	{
		pTrigger.second->Late_Tick(fTimeDelta);
	}
}

HRESULT CTrigger_Manager::Add_Layer_Trigger(TriggerInfo& pInfo, const _bool isStaticLevel)
{
	_uint iLevel = LEVEL_STATIC;
	if (not isStaticLevel)
	{
		iLevel = m_pGameInstance->Get_CurrentLevelIndex();
	}
	return m_pGameInstance->Add_Layer(iLevel, L"Layer_Trigger", L"Prototype_GameObject_Trigger", &pInfo);
}

void CTrigger_Manager::Create_Trigger(const wstring& strTriggerTag, _mat pMatrix)
{
	_uint iCurrLevel = m_pGameInstance->Get_CurrentLevelIndex();

	TriggerInfo Info;
	Info.WorldMat = pMatrix;

	Add_Layer_Trigger(Info);
	auto iter = m_pTrigger[iCurrLevel].find(&Info);
	if (iter == m_pTrigger[iCurrLevel].end())
	{
		CTrigger* pTrigger = Clone(Info);

		m_pTrigger[iCurrLevel].emplace(&pMatrix, pTrigger);
	}
}

CTrigger* CTrigger_Manager::Clone(TriggerInfo& pInfo)
{
	return dynamic_cast<CTrigger*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Trigger", &pInfo));
}

void CTrigger_Manager::Delete_Trigger(const void* pMatrix)
{
}

void CTrigger_Manager::Clear(_uint iLevelIndex)
{
}

void CTrigger_Manager::Free()
{
	for (size_t i = 0; i < LEVEL_END; i++)
	{
		for (auto& pTrigger : m_pTrigger[i])
		{
			Safe_Release(pTrigger.second);
		}
		m_pTrigger[i].clear();
	}

	Safe_Release(m_pGameInstance);
}
