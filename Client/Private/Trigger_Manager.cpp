#include "Trigger_Manager.h"
#include "Trigger.h"
#include "Camera_CutScene.h"
#include "UI_Manager.h"
#include "Symbol.h"
#include "Camera_Manager.h"

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
	CTrigger* pTrigger = nullptr;
	_bool bColl = false;
	for (auto& iter : m_pTrigger)
	{
		iter->Tick(fTimeDelta);

		//m_pGameInstance->Get_CurrentLevelIndex();
		m_isColl = iter->Get_Collision();
		//m_isPlayCutScene = false;
		if (m_isColl == true)
		{
			bColl = true;
			if (iter->Get_TriggerType() == VILLAGE_TRIGGER && iter->Get_Limited() == true)
			{
				m_isPlayCutScene = true;
				m_isInfinite = false;
				m_strFilePath = L"../Bin/Data/Village_CutScene.dat";
				CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_CUTSCENE);
				CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::VILLAGE);
				m_isInVillage = true;
				iter->Set_Limited(false);
			}
			else if (iter->Get_TriggerType() == FRONTDOOR_IN_TRIGGER)
			{
				pTrigger = iter;
				if (iter->Get_Limited() == false)
				{
					if (m_isInVillage)
					{
						CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::FIELDEAST);
						m_pGameInstance->FadeoutSound(0, fTimeDelta, 1.f, false);
					}
					iter->Set_Limited(true);
				}
				if (not m_pGameInstance->Get_IsPlayingSound(0))
				{
					m_pGameInstance->PlayBGM(TEXT("Midgard_Field"));
					m_pGameInstance->FadeinSound(0, fTimeDelta);
					m_isInVillage = false;
				}
			}
			else if (iter->Get_TriggerType() == FRONTDOOR_OUT_TRIGGER)
			{
				pTrigger = iter;
				if (iter->Get_Limited() == false)
				{
					if (not m_isInVillage)
					{
						CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::VILLAGE);
						m_pGameInstance->FadeoutSound(0, fTimeDelta, 1.f, false);
					}
					iter->Set_Limited(true);
				}
				if (not m_pGameInstance->Get_IsPlayingSound(0))
				{
					m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));
					m_pGameInstance->FadeinSound(0, fTimeDelta);
					m_isInVillage = true;
				}
			}
			else if (iter->Get_TriggerType() == BACKDOOR_IN_TRIGGER)
			{
				pTrigger = iter;
				if (iter->Get_Limited() == false)
				{
					if (m_isInVillage)
					{
						CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::FIELDSOUTH);
						m_pGameInstance->FadeoutSound(0, fTimeDelta, 1.f, false);
					}
					iter->Set_Limited(true);
				}
				if (not m_pGameInstance->Get_IsPlayingSound(0))
				{
					m_pGameInstance->PlayBGM(TEXT("Midgard_Field"));
					m_pGameInstance->FadeinSound(0, fTimeDelta);
					m_isInVillage = false;
				}
			}
			else if (iter->Get_TriggerType() == BACKDOOR_OUT_TRIGGER)
			{
				pTrigger = iter;
				if (iter->Get_Limited() == false)
				{
					if (not m_isInVillage)
					{
						CUI_Manager::Get_Instance()->Set_Symbol(CSymbol::VILLAGE);
						m_pGameInstance->FadeoutSound(0, fTimeDelta, 1.f, false);
					}
					iter->Set_Limited(true);
				}
				if (not m_pGameInstance->Get_IsPlayingSound(0))
				{
					m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));
					m_pGameInstance->FadeinSound(0, fTimeDelta);
					m_isInVillage = true;
				}
			}
			else if (iter->Get_TriggerType() == BOSS_TRIGGER && iter->Get_Limited() == true)
			{
				pTrigger = iter;
				m_isCollBossTrigger = true;
				m_isPlayCutScene = true;
				m_strFilePath = L"../Bin/Data/Boss_First_CutScene.dat";
				m_isInfinite = true;
				CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_CUTSCENE);
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
		CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_CUTSCENE);
		m_bStartSuicide = false;
	}

	if (m_bAfterSuicide)
	{
		m_isBreakLoop = true;
		m_isPlayCutScene = false;
		m_isInfinite = false;
		m_strFilePath = L"../Bin/Data/Boss_Final_CutScene.dat";
		CCamera_Manager::Get_Instance()->Set_CameraModeIndex(CM_CUTSCENE);
		m_bAfterSuicide = false;
	}


	if (bColl && pTrigger != nullptr)
	{
		for (auto& iter : m_pTrigger)
		{
			if (iter->Get_TriggerType() == BOSS_TRIGGER || iter->Get_TriggerType() == VILLAGE_TRIGGER)
			{
				continue;
			}
			if (iter != pTrigger)
			{
				iter->Set_Limited(false);
			}
		}
	}
}

void CTrigger_Manager::Limited_CutScene(_bool isLimited)
{
	m_isLimited = isLimited;
}

void CTrigger_Manager::Teleport(const TeleportSpot eSpot)
{
	const TCHAR* pGetPath{};

	switch (eSpot)
	{
	case Client::TS_Dungeon:
	{
		pGetPath = TEXT("../Bin/Data/DungeonPos.dat");

		// 임시
		CTransform* pCamTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
		pCamTransform->Set_State(State::Pos, _vec4(2067.11f, -12.8557f, 2086.95f, 1.f));
		pCamTransform->LookAt_Dir(_vec4(0.97706846f, -0.21286753f, 0.004882995f, 0.f));

		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		//pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 4.f, 0.f));
		pPlayerTransform->Set_Position(_vec3(2070.81f, -14.8443f, 2086.87f));
		pPlayerTransform->LookAt_Dir(_vec4(0.99763946f, 0.014162573f, 0.067186668f, 0.f));

		m_pGameInstance->Set_HellHeight(-30.f);
		break;
	}
	case Client::TS_Village:
	{
		pGetPath = TEXT("../Bin/Data/Village_Player_Pos.dat");

		// 임시
		std::ifstream inFile(pGetPath, std::ios::binary);

		if (!inFile.is_open())
		{
			MSG_BOX("파일을 찾지 못했습니다.");
			return;
		}

		_vec4 Player_Pos{ 0.f };
		inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

		CTransform* pCamTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
		pCamTransform->Set_State(State::Pos, _vec4(-17.9027f, 18.f, 125.f, 1.f));
		pCamTransform->LookAt_Dir(_vec4(-0.541082f, 0.548757f, 0.637257f, 0.f));
		CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
		pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));
		pPlayerTransform->LookAt_Dir(_vec4(-0.541082f, 0.f, 0.637257f, 0.f));

		m_pGameInstance->Set_HellHeight(-70.f);
		break;
	}
	}

	// 파일을 읽어서 위치, 룩을 세팅하는 코드를 넣습니당

	//std::ifstream inFile(pGetPath, std::ios::binary);

	//if (!inFile.is_open())
	//{
	//	MSG_BOX("파일을 찾지 못했습니다.");
	//	return;
	//}

	//_vec4 Player_Pos{ 0.f };
	//inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

	//CTransform* pCamTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Com_Transform")));
	//pCamTransform->Set_State(State::Pos, _vec4(-17.9027f, 18.f, 125.f, 1.f));
	//pCamTransform->LookAt_Dir(_vec4(-0.541082f, 0.548757f, 0.637257f, 0.f));
	//CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	//pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));
	//pPlayerTransform->LookAt_Dir(_vec4(-0.541082f, 0.f, 0.637257f, 0.f));

	return;
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
