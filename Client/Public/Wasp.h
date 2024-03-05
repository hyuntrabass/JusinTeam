#pragma once

#include "VTFMonster.h"
#include "Client_Define.h"

BEGIN(Client)

class CWasp final : public CVTFMonster
{
public:
	enum ANIM
	{
		Anim_Attack01,
		Anim_Attack02,
		Anim_Die,
		Anim_Hit_Add,
		Anim_Hit_L,
		Anim_Hit_R,
		Anim_Idle,
		Anim_Knockdown,
		Anim_Roar,
		Anim_Run,
		Anim_Stun,
		Anim_Walk,
		Anim_End
	};
	enum STATE
	{
		State_Idle,
		State_Attack,
		State_Attack_End,
		State_Die,
		State_End
	};

private:
	CWasp(_dev pDevice, _context pContext);
	CWasp(const CWasp& rhs);
	virtual ~CWasp() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

private:
	void Update_Trail();

private:
	CCollider* m_pAttackColliderCom = { nullptr };
	class CCommonSurfaceTrail* m_pAttack_Trail{ nullptr };
	class CCommonSurfaceTrail* m_pAttack_Distortion_Trail{ nullptr };

private:
	STATE m_eState{ State_End };
	STATE m_ePreState{ State_End };
	_vec3 m_vAttackDir{};
	_float m_fMoveDirRatio{};

private:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	HRESULT Add_Components();

public:
	static CWasp* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END