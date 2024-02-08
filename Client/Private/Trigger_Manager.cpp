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
		iter->Tick(fTimeDelta);

		//m_pGameInstance->Get_CurrentLevelIndex();
		m_isColl = iter->Get_Collision();
		//m_isPlayCutScene = false;
		if (m_isColl == true)
		{
			if (iter->Get_TriggerType() == VILLAGE_TRIGGER && iter->Get_Limited() == true)
			{
				m_isPlayCutScene = true;
				m_isInfinite = false;
				m_strFilePath = L"../Bin/Data/Village_CutScene.dat";
				m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);
				iter->Set_Limited(false);
			}
			else if (iter->Get_TriggerType() == FRONTDOOR_IN_TRIGGER && iter->Get_Limited() == false)
			{
				// 정문으로 들어올 시
			}
			else if (iter->Get_TriggerType() == FRONTDOOR_OUT_TRIGGER && iter->Get_Limited() == false)
			{
				// 정문으로 나갈 시
			}
			else if (iter->Get_TriggerType() == BACKDOOR_IN_TRIGGER && iter->Get_Limited() == false)
			{
				// 후문으로 들어올 시
			}
			else if (iter->Get_TriggerType() == BACKDOOR_OUT_TRIGGER && iter->Get_Limited() == false)
			{
				// 후문으로 나갈 시
			}
			else if (iter->Get_TriggerType() == BOSS_TRIGGER && iter->Get_Limited() == true)
			{
				m_isCollBossTrigger = true;
				m_isPlayCutScene = true;
				m_strFilePath = L"../Bin/Data/Boss_First_CutScene.dat";
				m_isInfinite = true;
				m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);
				iter->Set_Limited(false);
			}

		}
	}

	if (m_bStartSuicide == true)
	{
		m_isBreakLoop = true;
		m_isPlayCutScene = false;
		m_isInfinite = false;
		m_strFilePath = L"../Bin/Data/Boss_Second_CutScene.dat";
		m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);
		m_bStartSuicide = false;
	}

	if (m_bAfterSuicide)
	{
		m_isBreakLoop = true;
		m_isPlayCutScene = false;
		m_isInfinite = false;
		m_strFilePath = L"../Bin/Data/Village_CutScene.dat";
		m_pGameInstance->Set_CameraModeIndex(CM_CUTSCENE);
		m_bAfterSuicide = false;
	}
}

void CTrigger_Manager::Limited_CutScene(_bool isLimited)
{
	m_isLimited = isLimited;
}

HRESULT CTrigger_Manager::Ready_Trigger_Village()
{
	TriggerInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Village_Trigger.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/Village_Trigger.dat 트리거 불러오기 실패.");
		return E_FAIL;
	}
	_uint TriggerListSize;
	inFile.read(reinterpret_cast<char*>(&TriggerListSize), sizeof(_uint));


	for (_uint i = 0; i < TriggerListSize; ++i)
	{
		TriggerInfo TriggerInfo{};

		_uint iIndex{};
		inFile.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));
		TriggerInfo.iIndex = iIndex;

		_bool bCheck{};
		inFile.read(reinterpret_cast<char*>(&bCheck), sizeof(_bool));
		TriggerInfo.bLimited = bCheck;

		_ulong TriggerPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&TriggerPrototypeSize), sizeof(_ulong));

		wstring TriggerPrototype;
		TriggerPrototype.resize(TriggerPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&TriggerPrototype[0]), TriggerPrototypeSize * sizeof(wchar_t));

		_float TriggerSize{};
		inFile.read(reinterpret_cast<char*>(&TriggerSize), sizeof(_float));
		TriggerInfo.fSize = TriggerSize;

		_mat TriggerWorldMat;
		inFile.read(reinterpret_cast<char*>(&TriggerWorldMat), sizeof(_mat));

		TriggerInfo.WorldMat = TriggerWorldMat;

		CTrigger* pTrigger = dynamic_cast<CTrigger*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Trigger"), &TriggerInfo));
		if (not pTrigger)
		{
			MSG_BOX("트리거 생성 실패.");
			return E_FAIL;
		}

		m_pTrigger.push_back(pTrigger);
	}

	inFile.close();

	return S_OK;
}

void CTrigger_Manager::Free()
{
	for (auto& pTrigger : m_pTrigger)
	{
		Safe_Release(pTrigger);
	}
	m_pTrigger.clear();

	Safe_Release(m_pGameInstance);
}
