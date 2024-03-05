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

private:
	CRenderer* m_pRendererCom{ nullptr };


private:
	_float					m_fStatus{};
	wstring					m_strText{};
	RECT					m_rcRect{};
	Buff					m_eBuff{ Buff_End };

	_float					m_fTime{};

	CGameObject*			m_pFade{ nullptr };
	class CNineSlice*		m_pBackGround{ nullptr };
	CNineSlice*				m_pSelect{ nullptr };
	class CTextButtonColor*	m_pBorder{ nullptr };
	CTextButtonColor*		m_pIcon{ nullptr };
	CTextButtonColor*		m_pText{ nullptr };

private:
	_bool m_isFadeEnd{};
	_bool m_isReadyFade{};
	_bool m_isReadyClick{};
	_bool m_isPicking{};
	_bool m_IsSelect{};

public:
	const _bool& Get_IsSelect() const { return m_IsSelect; }
	const _bool& Get_IsSelectEnd() const { return m_isFadeEnd; }
	const Buff& Get_Buff()const { return m_eBuff; }
	const _float& Get_Status() const { return m_fStatus; }
	void Set_Fade() { m_isReadyFade = true; }

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