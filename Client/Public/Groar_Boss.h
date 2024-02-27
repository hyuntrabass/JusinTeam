#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CGroar_Boss final : public CGameObject
{
public:
	enum GROAR_NPC_ANIM
	{
		DIE,
		NPC_IDLE,
		TALK,
		NPC_ANIM_END
	};

	enum GROAR_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		ATTACK04,
		ATTACK05,
		ATTACK06,
		ATTACK07,
		ATTACK_RAGE,
		DIE01,
		GROAR_DEAD_SC01_MON_GROAR,
		GROAR_DEAD_SC02_MON_GROAR,
		GROAR_DEAD_SC03_MON_GROAR,
		HIT_ADD,
		IDLE,
		MON_GROAR_ASGARD_ATTACK00, // �����տ��� �ʷϻ� ����ü ������
		MON_GROAR_ASGARD_ATTACK01, // �޼տ��� �ʷϻ� ����ü ������
		MON_GROAR_ASGARD_ATTACK02, // �ʷϻ� ����ü 6�� ������
		MON_GROAR_ASGARD_ATTACK03, // �ٴ� ���� ���
		MON_GROAR_ASGARD_ATTACK04, // X
		MON_GROAR_ASGARD_ATTACK05, // �Ź� ��ȯ
		MON_GROAR_ASGARD_ATTACK06, // ������ �˼�
		MON_GROAR_ASGARD_ATTACK07, // �� X��
		MON_GROAR_ASGARD_ATTACK08, // ���¢��(����)
		MON_GROAR_ASGARD_ATTACK_RAGE,
		// 00, 01, 02, 03, 04, 05, 06, 07, 08
		MON_GROAR_ASGARD_DIE,
		MON_GROAR_ASGARD_IDLE,
		MON_GROAR_ASGARD_ROAR,
		MON_GROAR_ASGARD_SPAWN,
		MON_GROAR_ASGARD_WALK,
		ROAR,
		RUN,
		STUN,
		TURN_L,
		TURN_R,
		W_ATTACK01,
		W_ATTACK02,
		W_ATTACK04,
		W_ATTACK05,
		W_ATTACK06,
		W_ATTACK07,
		W_ATTACK08,
		W_GROAR_RAGE,
		WALK,
		ANIM_END
	};

	enum GROAR_STATE
	{
		STATE_NPC,
		STATE_SCENE01,
		STATE_SCENE02,
		STATE_BOSS,
		STATE_END
	};

	enum GROAR_BOSS_STATE
	{
		BOSS_STATE_IDLE,
		BOSS_STATE_ROAR,
		BOSS_STATE_CHASE,
		BOSS_STATE_THROW_ATTACK, // 00, 01
		BOSS_STATE_SIX_MISSILE, // 02
		BOSS_STATE_TAKE_DOWN, // 03
		BOSS_STATE_SPIDER, // 05
		BOSS_STATE_TENTACLE, // 06
		BOSS_STATE_XBEAM, // 07
		BOSS_STATE_RAGE, // 08
		BOSS_STATE_DIE,
		BOSS_STATE_END
	};

	enum GROAR_ATTACK // ���� ������
	{
		ATTACK_THROW,
		ATTACK_SIX_MISSILE,
		ATTACK_TAKE_DOWN,
		ATTACK_SPIDER,
		ATTACK_TENTACLE,
		ATTACK_YELL,
		ATTACK_XBEAM,
		ATTACK_END
	};

	enum GROAR_NPCSTATE { NPC_TALK, NPC_QUEST, NPC_END };


private:
	CGroar_Boss(_dev pDevice, _context pContext);
	CGroar_Boss(const CGroar_Boss& rhs);
	virtual ~CGroar_Boss() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

public:
	HRESULT Add_Collider();
	void Update_Collider();

private:
	HRESULT Init_Dialog();
	HRESULT Add_Parts();
	void NPC_Tick(_float fTimeDelta);
	void NPC_LateTick(_float fTimeDelta);
	void Set_Text(GROAR_NPCSTATE eState);
	void Play_TalkSound(const wstring& strTalkText);


private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

	CModel* m_pNPCModelCom = { nullptr };
	CModel* m_pScene01ModelCom = { nullptr };
	CModel* m_pScene02ModelCom = { nullptr };
	CModel* m_pBossModelCom = { nullptr };

	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };
	CCollider* m_pNpcColliderCom = { nullptr };

	class CHPBoss* m_pHpBoss{ nullptr };

private:
	GROAR_STATE m_ePreState = STATE_END;
	GROAR_STATE m_eCurState = STATE_END;

	GROAR_BOSS_STATE m_eBossPreState = BOSS_STATE_END;
	GROAR_BOSS_STATE m_eBossCurState = BOSS_STATE_END;

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

private:
	ANIM_DESC m_Animation{};
	_int m_iSoundChannel = -1;

private:
	_float m_fIdleTime = {};

private:
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAttacked1 = { false };
	_bool m_bAttacked2 = { false };
	_bool m_bAttacked3 = { false };

private:
	_bool m_bCreateMissile = { false };
	_uint m_iThrowAttackCombo = {};

private:
	_bool m_bCreateSpider = { false };

private:
	_bool m_bCreateXBeam = { false };

private:
	_bool m_bTentacleOn = { false };
	_float m_fTentacleTime = {};

private:
	_float m_fRageTime = {};

private:
	_bool m_bAttack_Selected[ATTACK_END] = { false };

private:
	_bool m_bChangePass = { false };
	_uint m_iPassIndex = {};
	_float m_fHitTime = {};

private:
	_bool m_bChangePos = { false };

private:
	_bool						m_bQuest{};
	_bool						m_isReward{};
	GROAR_NPCSTATE				m_eState{ NPC_TALK };
	_bool						m_bTalking = { false };
	_bool						m_bNextDialog = { false };
	_bool						m_isColl = { false };

	_float						m_fDir{ -1.f };
	_float						m_fButtonTime{ 600.f };

	wstring						m_strQuestOngoing{};
	list<wstring>				m_vecDialog;
	vector<wstring>				m_vecChatt;
	list<wstring>				m_TalkSounds;

	class CTextButton* m_pLine{ nullptr };
	CTextButton* m_pArrow{ nullptr };
	CTextButton* m_pSkipButton{ nullptr };
	class CDialogText* m_pDialogText{ nullptr };
	class CTextButtonColor* m_pBackGround{ nullptr };
	class C3DUITex* m_pSpeechBubble{ nullptr };

private:
	const wstring m_strLines[14]
	{
		TEXT("����.."),
		TEXT("���� �ؼ� �˼��ؿ� ���� �׷ξ� ���������� �ų��Դϴ�."),
		TEXT("�ٸ��� �ƴ϶� ���ƿ��� �ʴ� ������ ã�� �־����."),
		TEXT("�𸣰ھ�� ����������.. ���ƿýð��� ��½ �����µ���.."),
		TEXT("�ƹ��� ������ �����.. ���� �׷������ �ƴѵ�.."),
		TEXT("�и� ���� ���� ������� Ʋ�������"),
		TEXT("������ ��Ź����� �ɱ��? ����.."),
		TEXT("!�׷ξ��� ��Ź"),
		TEXT("���̱���! Ȥ�� �� ������?"),
		TEXT("��..�̰�..��°�� �� ���? ��.. �� ������.. �� ��������?"),
		TEXT("���� �����ƿ�. �ٸ� ������ �������� Ȯ���ϰ� ���� ���̿���. ���̶� �ճ��̶� ������� ���̿���.."),
		TEXT("��ġ������ �� ��Ź�帱�Կ�. ������ ������ ã�µ��� �� ������ �������� �����ּ���. ��Ź�帳�ϴ�."),
		TEXT("!�׷ξƸ� ���Ѷ�"),
		TEXT("END")
	};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGroar_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END