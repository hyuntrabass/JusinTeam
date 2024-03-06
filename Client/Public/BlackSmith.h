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
	enum BLACKSMITH_STATE { TALK, QUEST_ING, STATE_END };

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

	list<wstring>				m_TalkSounds;
	_int						m_iSoundChannel = -1;
	_bool						m_IsSoundSkip{};
	_float						m_fSoundSkipRatio{};

	class CTextButton* m_pLine{ nullptr };
	CTextButton* m_pArrow{ nullptr };
	class CDialogText* m_pDialogText{ nullptr };
	class CTextButtonColor* m_pBackGround{ nullptr };
	CTextButton* m_pSkipButton{ nullptr };
	class C3DUITex* m_pSpeechBubble{ nullptr };

private:
	void Set_Text(BLACKSMITH_STATE eState);
	void Play_TalkSound(const wstring& strTalkText);

private:
	HRESULT Init_Dialog();
	HRESULT Add_Parts();

private:
	const wstring m_strLines[18]
	{
		TEXT("무슨 볼일인가 멀대?"),
		TEXT("장비? 드디어 이 몸의 위대함을 알아본 모양이군."),
		TEXT("마음만 먹는다면야 수 백, 수 천개의 장비도 뚝딱 만들 수 있다는 말씀"),
		TEXT("다만, 아무리 멀대 너라도 아무 대가없이 장비를 내어줄 순 없지"),
		TEXT("이 몸께서 또 다른 걸작을 준비하고 있는데 말이지"),
		TEXT("몇 가지 필요한 재료가 있다."),
		TEXT("멀대, 니가 그 재료들을 성공적으로 구해온다면 긍정적으로 생각해보도록 하지."),
		TEXT("!채집하기"),
		TEXT("좋아 멀대, 이 정도면 아주 훌륭해"),
		TEXT("좋아 단단함 다음은 날카로움이지"),
		TEXT("주변을 보면 커다란 뿔이 돋은 놈들이 있을거야"),
		TEXT("그 녀석들을 처치하고 뿔을 뽑아 멀대"),
		TEXT("!염소잡기"),
		TEXT("오호 왔구나 멀대, 어디보자 재료는 흘리지않고 챙겨왔겠지?"),
		TEXT("이봐 멀대, 너는 아주 싹싹하군 투덜거리지도 않고 말이야."),
		TEXT("아주 마음에 들어 이 몸께서 아주 작은 작~은 선물을 주도록하지."),
		TEXT("어디보자 멀대 너에게 어울릴만한게"),
		TEXT("END")
	};

public:
	static CBlackSmith* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END