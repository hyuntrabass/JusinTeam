#include "Event_Manager.h"
#include "GameInstance.h"
#include "Pop_QuestIn.h"
#include "Pop_QuestEnd.h"
#include "Pop_LevelUp.h"
#include "Pop_Skill.h"
#include "Pop_Alert.h"
#include "Tutorial.h"
#include "Quest.h"
IMPLEMENT_SINGLETON(CEvent_Manager)

CEvent_Manager::CEvent_Manager()
{

}

HRESULT CEvent_Manager::Init()
{
	m_pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(m_pGameInstance);

	if (FAILED(Init_Quest()))
	{
		return E_FAIL;
	}

	m_pAlert = (CPop_Alert*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Pop_Alert"));
	if (not m_pAlert)
	{
		return E_FAIL;
	}
	for (size_t i = 0; i < TRIGGER_END; i++)
	{
		m_QuestTrigger[i] = false;
	}

	return S_OK;
}
void CEvent_Manager::Tick(_float fTimeDelta)
{

	if (m_isEventIn)
	{
		if (m_pGameInstance->Get_LayerSize(LEVEL_STATIC, TEXT("Layer_Pop")) != 0)
		{
			m_isWaiting = true;
		}
		else
		{
			m_isWaiting = false;
		}

		if (m_EventList.empty())
		{
			m_isEventIn = false;
			return;
		}
		if (!m_isWaiting)
		{
			EVENT_TYPE eType = m_EventList.front().eType;

			switch (eType)
			{
			case LEVELUP:
			{
				CPop_LevelUp::LEVELUP_DESC Desc{};
				Desc.iLevel = m_EventList.front().iNum;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pop"), TEXT("Prototype_GameObject_PopLevelUp"), &Desc)))
				{
					return;
				}
				m_pGameInstance->Play_Sound(TEXT("LevelUp"));
				m_EventList.pop_front();
			}
			break;
			case QUESTIN:
			{
				m_pGameInstance->Play_Sound(TEXT("Quest_Start"), 0.6f);
				CPop_QuestIn::QUESTIN_DESC PopQuestInDesc = {};
				PopQuestInDesc.isMain = m_EventList.front().isMain;
				PopQuestInDesc.fExp = m_EventList.front().fExp;
				PopQuestInDesc.iMoney = m_EventList.front().iMoney;
				PopQuestInDesc.strText = m_EventList.front().strText;
				PopQuestInDesc.strQuestTitle = m_EventList.front().strQuestTitle;

				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pop"), TEXT("Prototype_GameObject_Pop_QuestIn"), &PopQuestInDesc)))
					return;
				m_EventList.pop_front();
			}
			break;
			case QUESTEND:
			{
				m_pGameInstance->Play_Sound(TEXT("Quest_Complete"), 0.6f);
				CPop_QuestEnd::QUESTEND_DESC PopQuestEndDesc = {};
				PopQuestEndDesc.fExp = m_EventList.front().fExp;
				PopQuestEndDesc.iMoney = m_EventList.front().iMoney;
				PopQuestEndDesc.strQuestTitle = m_EventList.front().strQuestTitle;
				PopQuestEndDesc.vecRewards = m_EventList.front().vecRewards;

				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pop"), TEXT("Prototype_GameObject_Pop_QuestEnd"), &PopQuestEndDesc)))
					return;
				m_EventList.pop_front();
			}
			break;
			case TUTORIAL:
			{
				CTutorial::TUTO_DESC TutoDesc = {};
				TutoDesc.eTuto = m_eCurTuto;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pop"), TEXT("Prototype_GameObject_Tutorial"), &TutoDesc)))
					return;
				m_EventList.pop_front();

				if (m_eCurTuto == T_OPENINVEN|| m_eCurTuto == T_OPENSKILL)
				{
					for (size_t i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
					{
						if (m_pGameInstance->Get_IsLoopingSound(i))
						{
							m_pGameInstance->FadeoutSound(i, fTimeDelta, 1.f, true, 0.3f);
						}
					}
				}
			}
			break;
			case UNLOCKSKILL:
			{
				CPop_Skill::SKILLIN_DESC Desc{};
				Desc.iSkillLevel = m_EventList.front().iNum;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pop"), TEXT("Prototype_GameObject_PopSkill"), &Desc)))
				{
					return;
				}
				m_pGameInstance->Play_Sound(TEXT("UI_Synthesis_Success_SFX_01"));
				m_EventList.pop_front();
			}
			break;
			}
			m_isWaiting = true;
		}
	}

	if (m_QuestTrigger[TUTO_TRIGGER] && m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY && not m_pGameInstance->Is_Playing_Video())
	{
		m_QuestTrigger[TUTO_TRIGGER] = false;
		m_pGameInstance->Level_ShutDown(LEVEL_GAMEPLAY);
	}

	m_pQuest->Tick(fTimeDelta);
	m_pAlert->Tick(fTimeDelta);
}

_bool CEvent_Manager::Find_Quest(const wstring& strQuest)
{
	auto iter = m_QuestMap.find(strQuest);
	if (iter == m_QuestMap.end())
	{
		return false;
	}
	
	return true;
}

HRESULT CEvent_Manager::Init_Quest()
{
	m_pQuest = (CQuest*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Quest"));
	if (not m_pQuest)
	{
		return E_FAIL;
	}

	vector<pair<wstring, _uint>> vecRewards;

	EVENT_DESC tDesc = {};
	tDesc.eType = QUESTIN;
	tDesc.fExp = 1.7f;
	tDesc.iNum = 1;
	tDesc.iMoney = 1000;
	tDesc.isMain = true;
	tDesc.strQuestTitle = TEXT("몬스터 처치");
	tDesc.strText = TEXT("갑판 위 몬스터 처치하기");
	vecRewards.push_back(make_pair(TEXT("[일반]탈 것 소환 카드"), 10));
	vecRewards.push_back(make_pair(TEXT("[유니크]신비한 알"), 1));
	vecRewards.push_back(make_pair(TEXT("마나하임의 갑옷"), 1));
	vecRewards.push_back(make_pair(TEXT("바이킹의 투구"), 1));
	vecRewards.push_back(make_pair(TEXT("거인의 강철 단검"), 1));
	vecRewards.push_back(make_pair(TEXT("거인의 강철 활"), 1));
	tDesc.vecRewards = vecRewards;
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	vecRewards.clear();
	tDesc.vecRewards = vecRewards;
	tDesc.eType = QUESTIN;
	tDesc.fExp = 50.f;
	tDesc.iNum = 3;
	tDesc.iMoney = 500000;
	tDesc.isMain = false;
	tDesc.strQuestTitle = TEXT("로스크바의 부탁");
	tDesc.strText = TEXT("토끼 세마리 잡기");
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	vecRewards.clear();

	tDesc.eType = QUESTIN;
	tDesc.fExp = 20.f;
	tDesc.iNum = 1;
	tDesc.iMoney = 10000;
	tDesc.isMain = false;
	tDesc.strQuestTitle = TEXT("체력포션 구매");
	tDesc.strText = TEXT("마을 상인에게 체력포션 구매하기");
	vecRewards.push_back(make_pair(TEXT("[희귀]탈 것 소환 카드"), 10));
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	vecRewards.clear();

	tDesc.vecRewards = vecRewards;
	tDesc.eType = QUESTIN;
	tDesc.fExp = 20.5f;
	tDesc.iNum = 1;
	tDesc.iMoney = 10000;
	tDesc.isMain = true;
	tDesc.strQuestTitle = TEXT("그로아씨 찾기");
	tDesc.strText = TEXT("던전에 있는 그로아 찾기");
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	vecRewards.clear();

	tDesc.eType = QUESTIN;
	tDesc.fExp = 12.3f;
	tDesc.iNum = 2;
	tDesc.iMoney = 10000;
	tDesc.isMain = false;
	tDesc.strQuestTitle = TEXT("채집하기");
	tDesc.strText = TEXT("소금광석 채집하기");
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	vecRewards.clear();

	tDesc.eType = QUESTIN;
	tDesc.fExp = 22.3f;
	tDesc.iNum = 1;
	tDesc.iMoney = 10000;
	tDesc.isMain = true;
	tDesc.strQuestTitle = TEXT("친해져요 블랙스미스");
	tDesc.strText = TEXT("블랙스미스 부탁 들어주기");
	vecRewards.push_back(make_pair(TEXT("폭군 수드리의 활"), 1));
	vecRewards.push_back(make_pair(TEXT("폭군 수드리의 단검"), 1));
	tDesc.vecRewards = vecRewards;
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	vecRewards.clear();
	
	tDesc.eType = QUESTIN;
	tDesc.fExp = 15.6f;
	tDesc.iNum = 2;
	tDesc.iMoney = 10000;
	tDesc.isMain = false;
	tDesc.strQuestTitle = TEXT("염소잡기");
	tDesc.strText = TEXT("염소 두마리 잡기");
	vecRewards.push_back(make_pair(TEXT("염소 뿔"), 3));
	tDesc.vecRewards = vecRewards;
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);
	
	vecRewards.clear();

	tDesc.vecRewards = vecRewards;
	tDesc.eType = QUESTIN;
	tDesc.fExp = 14.6f;
	tDesc.iNum = 1;
	tDesc.iMoney = 10000;
	tDesc.isMain = true;
	tDesc.strQuestTitle = TEXT("그로아의 부탁");
	tDesc.strText = TEXT("그로아 남편의 흔적찾기");
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);
	
	vecRewards.clear();

	tDesc.eType = QUESTIN;
	tDesc.fExp = 20.3f;
	tDesc.iNum = 3;//몬스터 수만큼? 
	tDesc.iMoney = 10000; 
	tDesc.isMain = true;
	tDesc.strQuestTitle = TEXT("그로아를 지켜라");
	tDesc.strText = TEXT("몬스터로부터 그로아 지키기");
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);
	

	vecRewards.clear();
	tDesc.eType = QUESTIN;
	tDesc.fExp = 10.2f;// 몬스터 수만큼 ?
	tDesc.iNum = 1;
	tDesc.iMoney = 10000;
	tDesc.isMain = true;
	tDesc.strQuestTitle = TEXT("그로아를 찾아서");
	tDesc.strText = TEXT("남편 흔적 찾는 그로아 찾기");
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	return S_OK;
}
HRESULT CEvent_Manager::Update_Quest(const wstring& strQuest)
{
	auto iter = m_QuestMap.find(strQuest);
	if (iter == m_QuestMap.end())
	{
		return E_FAIL;
	}

	if (m_pQuest->Update_Quest(strQuest))
	{
		EVENT_DESC tDesc = m_QuestMap[strQuest];
		tDesc.eType = QUESTEND;
		Set_Event(tDesc);
		m_QuestMap.erase(strQuest);

		if (!m_QuestTrigger[GROAR_MONSTER] && strQuest == TEXT("그로아를 지켜라"))
		{
			m_QuestTrigger[GROAR_MONSTER] = true;
			Set_Quest(TEXT("그로아를 찾아서"));
		}

	}
	return S_OK;
}
void CEvent_Manager::Set_LevelUp(_uint iLevel)
{
	EVENT_DESC Desc = {};
	Desc.eType = LEVELUP;
	Desc.iNum = iLevel;

	m_EventList.push_back(Desc);
	m_isEventIn = true;

}
void CEvent_Manager::Set_SkillUnlock(_uint iIndex)
{
	EVENT_DESC Desc = {};
	Desc.eType = UNLOCKSKILL;
	Desc.iNum = iIndex;

	m_EventList.push_back(Desc);
	m_isEventIn = true;
}
void CEvent_Manager::Set_Alert(const wstring strAlert)
{
	m_pAlert->Set_Alert(strAlert);
}

void CEvent_Manager::Set_TutorialComplete(TUTO_SEQ eTuto)
{
	if (eTuto >= TUTO_END || m_eCurTuto >= TUTO_END)
	{
		return;
	}

	if (eTuto != m_eCurTuto)
	{
		return;
	}
	m_TutoComplete[eTuto] = true;
}

void CEvent_Manager::Set_TutorialSeq(TUTO_SEQ eTuto)
{
	if (m_eCurTuto == eTuto)
	{
		return;
	}

	if (!m_isTutoStarted)
	{
		if (eTuto == (TUTO_SEQ)0)
		{
			m_eCurTuto = eTuto;
			m_isEventIn = true;
			m_isTutoStarted = true;
			EVENT_DESC Desc = {};
			Desc.eType = TUTORIAL;
			m_EventList.push_back(Desc);
			return;
		}

	}
	if (m_isTutoStarted && eTuto != (TUTO_SEQ)0)
	{
		if (m_eCurTuto >= TUTO_END)
		{
			return;
		}
		if (!m_TutoComplete[(_uint)eTuto - 1])
		{
			return;
		}
		m_eCurTuto = eTuto;
		m_isEventIn = true;
		EVENT_DESC Desc = {};
		Desc.eType = TUTORIAL;
		m_EventList.push_back(Desc);
	}
}

_bool CEvent_Manager::Get_TutorialComplete(TUTO_SEQ eTuto)
{
	if (eTuto >= TUTO_END || eTuto < 0)
	{
		return false;
	}
	//current로 해야될수도?
	return m_TutoComplete[eTuto];
}

void CEvent_Manager::Late_Tick(_float fTimeDelta)
{
	m_pQuest->Late_Tick(fTimeDelta);
	m_pAlert->Late_Tick(fTimeDelta);
}
HRESULT CEvent_Manager::Render()
{

	return S_OK;
}

HRESULT CEvent_Manager::Set_Quest(const wstring& strQuest)
{
	auto iter = m_QuestMap.find(strQuest);
	if (iter == m_QuestMap.end())
	{
		return E_FAIL;
	}
	CQuestBox::QUESTBOX_DESC tDesc = {};
	tDesc.iNum = m_QuestMap[strQuest].iNum;
	tDesc.isMain = m_QuestMap[strQuest].isMain;
	tDesc.strQuestTitle = m_QuestMap[strQuest].strQuestTitle;
	tDesc.strText = m_QuestMap[strQuest].strText;
	tDesc.vPosition = _float2(0.f, 0.f);

	if (FAILED(m_pQuest->Set_Quest(tDesc)))
	{
		return E_FAIL;
	}


	m_EventList.push_back(m_QuestMap[strQuest]);
	m_isEventIn = true;

	return S_OK;
}
HRESULT CEvent_Manager::Set_Event(EVENT_DESC pDesc)
{
	m_isWaiting = false;
	m_EventList.push_back(pDesc);
	m_isEventIn = true;

	return S_OK;
}

void CEvent_Manager::Free()
{
	Safe_Release(m_pAlert);
	Safe_Release(m_pQuest);
	Safe_Release(m_pGameInstance);
}
