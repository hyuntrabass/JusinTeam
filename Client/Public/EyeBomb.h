#pragma once

#include "VTFMonster.h"
#include "Client_Define.h"

BEGIN(Client)

class CEyeBomb final : public CVTFMonster
{
public:
	enum ANIM
	{
		Anim_Attack01,
		Anim_Attack02,
		Anim_Attack03, // 터지는거 
		Anim_Hit_add,
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

private:
	CEyeBomb(_dev pDevice, _context pContext);
	CEyeBomb(const CEyeBomb& rhs);
	virtual ~CEyeBomb() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

private:
	class CEffect_Dummy* m_pBaseEffect{ nullptr };
	class CEffect_Dummy* m_pFrameEffect{ nullptr };
	class CEffect_Dummy* m_pDimEffect{ nullptr };
	class CTextButtonColor* m_BloodTextrue{ nullptr };

private:
	_mat m_BaseEffectMat{};
	_mat m_BaseOriEffectMat{};
	_float m_fBaseEffectScale{};
	_bool m_bIsCollision{};
private:
	HRESULT Add_Components();

public:
	static CEyeBomb* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END