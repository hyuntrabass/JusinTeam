#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)

class CBlackSmith final : public CNPC
{
public:
	enum BLACKSMITH_ANIM
	{
		IDLE,
		IDLE02,
		REACTION01,
		REACTION02,
		TALK01,
		WALK01,
		ANIM_END
	};
	enum BLACKSMITH_STATE { TALK, QUEST_ING, ROSKVA_END };

private:
	CBlackSmith(_dev pDevice, _context pContext);
	CBlackSmith(const CBlackSmith& rhs);
	virtual ~CBlackSmith() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	BLACKSMITH_STATE			m_eState{ TALK };
	_bool						m_bTalking = { false };
	_bool						m_bNextDialog = { false };
	_bool						m_isColl = { false };

	_float						m_fDir{ -1.f };
	_float						m_fButtonTime{};

	wstring						m_strQuestOngoing{};
	list<wstring>				m_DialogList;
	list<wstring>				m_ChattList;

	class CTextButton* m_pLine{ nullptr };
	CTextButton* m_pArrow{ nullptr };
	class CDialogText* m_pDialogText{ nullptr };
	CTextButton* m_pSkipButton{ nullptr };
	class CTextButtonColor* m_pBackGround{ nullptr };
	class C3DUITex* m_pSpeechBubble{ nullptr };

private:
	void Set_Text(BLACKSMITH_STATE eState);

private:
	HRESULT Init_Dialog();
	HRESULT Add_Parts();


public:
	static CBlackSmith* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END