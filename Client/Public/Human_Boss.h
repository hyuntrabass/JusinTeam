#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CHuman_Boss final : public CGameObject
{
public:

	enum HUMANBOSS_ANIM
	{
	HumanBoss_Attack01, // â���� ���� 2�� ġ��
	HumanBoss_Attack01_A,
	HumanBoss_Attack02, // ö��� 3��Ÿ 
	HumanBoss_Attack02_A,
	HumanBoss_Attack03, // â ���� ������ ������
	HumanBoss_Attack03_A,
	HumanBoss_Attack04, //ö��� �ָ� ������
	HumanBoss_Attack04_A, // ���ʿ� �� ��ȯ�ϴ� ����
	HumanBoss_Attack05, // ���߿� �Ҹ����� 
	HumanBoss_Attack05_A, //â������
	HumanBoss_Attack06, // ������ �ֺ� �ֵθ�
	HumanBoss_Attack06_A,
	HumanBoss_Attack07,
	HumanBoss_Attack07_A,
	HumanBoss_Attack08,
	HumanBoss_Attack08_A,
	HumanBoss_Attack09, //�ֺ� â���� ����
	HumanBoss_Attack09_A,
	HumanBoss_Attack10, //ö�� ������ ���� �ֵθ�
	HumanBoss_Die,
	HumanBoss_Die_A,
	HumanBoss_Idle,
	HumanBoss_Idle_A,
	HumanBoss_Roar,
	HumanBoss_Roar_A,
	HumanBoss_Run,
	HumanBoss_Run_A,
	HumanBoss_Spawn,
	HumanBoss_Spawn_A,
	HumanBoss_Talk,
	HumanBoss_Talk_A,
	HumanBoss_Walk,
	HumanBoss_Walk_A,
	HumanBoss_AnimEnd
	};

	enum STATE
	{
		SPEAR1, // â ���� 2��Ÿ
		SPEAR2, // â �����鼭 �ֺ� ����
		SPEAR3,	// â �Ȱ� ���濡 ����? ����
		SPEAR4,	// â ������
		MACE1,	// ö�� ���� 3��Ÿ
		MACE2,	// ö��� �÷��̾��� �߰Ÿ� ����
		MACE3, //
		IDLE,
		WALK,
		RUN,
		DIE,
		Spwan,
		BOSS_STATE_END
	};

private:
	CHuman_Boss(_dev pDevice, _context pContext);
	CHuman_Boss(const CHuman_Boss& rhs);
	virtual ~CHuman_Boss() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

public:
	HRESULT Add_Collider();
	void Update_Collider();
	virtual void Set_Damage(_int iDamage, _uint MonAttType = 0) override;
private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };
																																																																
private:
	STATE m_ePreState = BOSS_STATE_END;
	STATE m_eState = BOSS_STATE_END;

private:
	ANIM_DESC m_Animation{};

private:
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };
	_bool m_bSecondPattern{};
	_bool m_bChangePass{};
	_float m_fHitTime{};
	_uint m_iPassIndex{};
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHuman_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END