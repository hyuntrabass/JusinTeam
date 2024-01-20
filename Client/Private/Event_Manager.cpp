#include "Event_Manager.h"
#include "GameInstance.h"
#include "Pop_QuestIn.h"
#include "Pop_QuestEnd.h"
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
	return S_OK;
}
void CEvent_Manager::Tick(_float fTimeDelta)
{

	if (m_isEventIn)
	{
		if (m_pGameInstance->Get_LayerSize(LEVEL_STATIC, TEXT("Layer_Pop")) == 0)
		{
			m_isWaiting = false;
		}
		if (m_vecPopEvents.empty())
		{
			m_isEventIn = false;
		}
		if (!m_isWaiting)
		{
			EVENT_TYPE eType = m_vecPopEvents.front().eType;

			switch (eType)
			{
			case LEVELUP:
			{
				m_vecPopEvents.erase(m_vecPopEvents.begin());
			}
			break;
			case QUESTIN:
			{
				CPop_QuestIn::QUESTIN_DESC PopQuestInDesc = {};
				PopQuestInDesc.isMain = m_vecPopEvents.front().isMain;
				PopQuestInDesc.fExp = m_vecPopEvents.front().fExp;
				PopQuestInDesc.iMoney = m_vecPopEvents.front().iMoney;
				PopQuestInDesc.strText = m_vecPopEvents.front().strText;
				PopQuestInDesc.strQuestTitle = m_vecPopEvents.front().strQuestTitle;

				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pop"), TEXT("Prototype_GameObject_Pop_QuestIn"), &PopQuestInDesc)))
					return;
				m_vecPopEvents.erase(m_vecPopEvents.begin());
			}
			break;
			case QUESTEND:
			{
				CPop_QuestEnd::QUESTEND_DESC PopQuestEndDesc = {};
				PopQuestEndDesc.fExp = m_vecPopEvents.front().fExp;
				PopQuestEndDesc.iMoney = m_vecPopEvents.front().iMoney;
				PopQuestEndDesc.strQuestTitle = m_vecPopEvents.front().strQuestTitle;

				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pop"), TEXT("Prototype_GameObject_Pop_QuestEnd"), &PopQuestEndDesc)))
					return;
				m_vecPopEvents.erase(m_vecPopEvents.begin());
			}
			break;
			}
			m_isWaiting = true;
		}
	}
	m_pQuest->Tick(fTimeDelta);
}

HRESULT CEvent_Manager::Init_Quest()
{
	m_pQuest = (CQuest*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Quest"));
	if (not m_pQuest)
	{
		return E_FAIL;
	}

	EVENT_DESC tDesc = {};
	tDesc.eType = QUESTIN;
	tDesc.fExp = 1.7;
	tDesc.iNum = 1;
	tDesc.iMoney = 1000;
	tDesc.isMain = true;
	tDesc.strQuestTitle = TEXT("공격하기");
	tDesc.strText = TEXT("공격키를 눌러봐");
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	tDesc.eType = QUESTIN;
	tDesc.fExp = 1.3;
	tDesc.iNum = 1;
	tDesc.iMoney = 1000;
	tDesc.isMain = false;
	tDesc.strQuestTitle = TEXT("이동하기");
	tDesc.strText = TEXT("이동을 해봐");
	m_QuestMap.emplace(tDesc.strQuestTitle, tDesc);

	tDesc.eType = QUESTIN;
	tDesc.fExp = 2;
	tDesc.iNum = 3;
	tDesc.iMoney = 1000;
	tDesc.isMain = false;
	tDesc.strQuestTitle = TEXT("몬스터와 접촉");
	tDesc.strText = TEXT("몬스터와 3회 접촉해봐");
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
	}
	return S_OK;
}
void CEvent_Manager::Late_Tick(_float fTimeDelta)
{
	m_pQuest->Late_Tick(fTimeDelta);
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


	m_vecPopEvents.push_back(m_QuestMap[strQuest]);
	m_isEventIn = true;

	return S_OK;
}
HRESULT CEvent_Manager::Set_Event(EVENT_DESC pDesc)
{
	m_isWaiting = false;
	m_vecPopEvents.push_back(pDesc);
	m_isEventIn = true;

	return S_OK;
}

void CEvent_Manager::Free()
{
	Safe_Release(m_pQuest);
	Safe_Release(m_pGameInstance);
}
