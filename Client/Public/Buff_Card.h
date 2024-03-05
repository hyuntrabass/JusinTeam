#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

enum Buff
{
	Buff_MaxHp, //최대 체력							(3~500 정도?)
	Buff_HpRegen, //체력 재생속도						(플레이어 HpRegenAmount 10.f 주면됨)
	Buff_MpRegen, //마나 재생속도						(플레이어 MpRegenAmount 10.f 주면됨)
	Buff_Attack, //공격력 증가						(20 정도?)
	Buff_Speed, //이동속도 증가						(2.f정도?)
	Buff_CoolDown, //쿨타임 감소						(0.2f)
	Buff_BloodDrain, //흡혈							(0.1f정도)
	Buff_PoisonImmune, //독 면역						(true)
	Buff_MonRegenDown, //몬스터 생성속도 감소			(0.7f 정도)
	Buff_End
};

struct BUFFCARD_DESC
{
	Buff		eBuff;
	_vec2		vPos;
};

class CBuff_Card final : public COrthographicObject
{

private:
	CBuff_Card(_dev pDevice, _context pContext);
	CBuff_Card(const CBuff_Card& rhs);
	virtual ~CBuff_Card() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool Get_IsSelect() { return m_IsSelect; }
	Buff Get_Buff() { return m_eBuff; }
private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	RECT					m_rcRect{};
	Buff					m_eBuff{ Buff_End };
	CGameObject*			m_pItemTex{ nullptr };
	class CNineSlice*		m_pBackGround{ nullptr };
	class CTextButton*		m_pExitButton{ nullptr };
	CTextButton*			m_SelectButton{ nullptr };

private:
	_bool m_IsSelect{};

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBuff_Card* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END