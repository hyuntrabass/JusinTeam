#pragma once

#include "VTFMonster.h"
#include "Client_Define.h"

BEGIN(Client)

class CVoid19 final : public CVTFMonster
{
public:
	enum ANIM
	{
		Anim_attack01, // Ä§¹ñ±â
		Anim_attack02,
		Anim_hit_add,
		Anim_idle,
		Anim_roar, //idle·Î ¾²´ÂÁß
		Anim_stun, // »ç¸Á
		Anim_End
	};
	enum STATE
	{
		State_Idle,
		State_Attack,
		State_Die,
		State_End
	};

private:
	CVoid19(_dev pDevice, _context pContext);
	CVoid19(const CVoid19& rhs);
	virtual ~CVoid19() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

private:
	STATE m_eState{ State_End };
	STATE m_ePreState{ State_End };

private:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

private:
	HRESULT Add_Components();

public:
	static CVoid19* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END