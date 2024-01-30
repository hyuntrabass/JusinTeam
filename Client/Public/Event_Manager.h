#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END
BEGIN(Client)
class CEvent_Manager final : public CBase
{
	DECLARE_SINGLETON(CEvent_Manager)

public:
	enum EVENT_TYPE { QUESTIN, QUESTEND, LEVELUP, TYPE_END};
	typedef struct tagEventDesc
	{
		EVENT_TYPE eType;
		_bool isMain;
		_float	fExp;
		_uint	iNum;
		_uint	iMoney;
		wstring strQuestTitle;
		wstring strText;
	}EVENT_DESC;

private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

private:
	CGameInstance*						m_pGameInstance{ nullptr };

	_bool								m_isEventIn = { false };
	_bool								m_isWaiting = { false };
	class CQuest*						m_pQuest{ nullptr };

	vector<EVENT_DESC>					m_vecPopEvents;
	map <const wstring, EVENT_DESC>		m_QuestMap;

public:
	HRESULT Init();
	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);
	HRESULT Render();

private:

	HRESULT Set_Event(EVENT_DESC pDesc);

public:
	_bool Find_Quest(const wstring& strQuest);
	HRESULT Init_Quest();
	HRESULT Set_Quest(const wstring& strQuest);
	//HRESULT Set_Event_byList(EVENT_LIST eNum);
	HRESULT Update_Quest(const wstring& strQuest);

public:
	virtual void Free() override;
};

END
