#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

enum Buff
{
	Buff_MaxHp, //�ִ� ü��							(3~500 ����?)
	Buff_HpRegen, //ü�� ����ӵ�						(�÷��̾� HpRegenAmount 10.f �ָ��)
	Buff_MpRegen, //���� ����ӵ�						(�÷��̾� MpRegenAmount 10.f �ָ��)
	Buff_Attack, //���ݷ� ����						(20 ����?)
	Buff_Speed, //�̵��ӵ� ����						(2.f����?)
	Buff_CoolDown, //��Ÿ�� ����						(0.2f)
	Buff_BloodDrain, //����							(0.1f����)
	Buff_PoisonImmune, //�� �鿪						(true)
	Buff_MonRegenDown, //���� �����ӵ� ����			(0.7f ����)
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